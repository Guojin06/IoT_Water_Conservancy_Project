import time
import random
import logging
from datetime import datetime
from typing import Dict, Any
import json

from redis_client import RedisClient
from mysql_client import MySQLClient

# 配置日志
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class DataProducer:
    """
    模拟传感器数据并将其写入Redis（用于实时广播）和MySQL（用于持久化存储）。
    """
    def __init__(self, redis_client: RedisClient, mysql_client: MySQLClient):
        self.redis_client = redis_client
        self.mysql_client = mysql_client
        
        # --- 内部状态，用于计算累计值 ---
        self.daily_water_supply = 12.5  # 假设的初始值 (万立方米)
        self.daily_power_generation = 8.7  # 假设的初始值 (万千瓦时)
        self.last_stats_update_time = time.time()

        # 扩展传感器列表以覆盖Dashboard上的所有关键指标
        self.sensors: Dict[str, Dict[str, Any]] = {
            # --- 核心水文数据 ---
            "WL001": {"name": "当前水位", "type": "water_level", "location": "主坝", "unit": "m", "min": 155, "max": 160},
            "FL001": {"name": "当前流量", "type": "flow_rate", "location": "主泄洪口", "unit": "m³/s", "min": 2300, "max": 2400},
            
            # --- 水质监测数据 ---
            "PH001": {"name": "PH值", "type": "water_quality_ph", "location": "入水口", "unit": "pH", "min": 6.8, "max": 7.5},
            "TUR001": {"name": "浊度", "type": "water_quality_turbidity", "location": "入水口", "unit": "NTU", "min": 1.5, "max": 3.0},
            "DO001": {"name": "溶解氧", "type": "water_quality_do", "location": "中心水域", "unit": "mg/L", "min": 7.0, "max": 9.0},
            "COND001": {"name": "电导率", "type": "water_quality_conductivity", "location": "出水口", "unit": "μS/cm", "min": 200, "max": 300},
            "AN001": {"name": "氨氮", "type": "water_quality_ammonia", "location": "出水口", "unit": "mg/L", "min": 0.02, "max": 0.15},

            # --- 工况数据 ---
            "WP001": {"name": "水压", "type": "water_pressure", "location": "管道A", "unit": "MPa", "min": 15.0, "max": 15.5},

            # --- 设备状态 (状态为0:关闭/正常, 1:开启/运行, 2:告警/故障) ---
            "PUMP001": {"name": "1号水泵", "type": "device_status", "location": "1号泵房", "unit": "status", "states": ["关闭", "运行", "故障"]},
            "PUMP002": {"name": "2号水泵", "type": "device_status", "location": "2号泵房", "unit": "status", "states": ["关闭", "运行", "故障"]},
            "GATE001": {"name": "泄洪闸01", "type": "device_status", "location": "A区", "unit": "status", "states": ["关闭", "开启", "告警"]},
            "GATE002": {"name": "泄洪闸02", "type": "device_status", "location": "B区", "unit": "status", "states": ["关闭", "开启", "告警"]},
        }

    def _create_tables_if_not_exist(self):
        """检查并创建所有需要的数据库表。"""
        if not self.mysql_client.is_connected():
            return
        
        # 使用 DATABASE_DESIGN.md 中的建表语句作为权威来源
        create_sensors_table = """
        CREATE TABLE IF NOT EXISTS `sensors` (
          `sensor_id` varchar(50) NOT NULL,
          `name` varchar(100) DEFAULT NULL,
          `sensor_type` varchar(50) NOT NULL,
          `location` varchar(100) DEFAULT NULL,
          `description` text,
          `states` json DEFAULT NULL,
          PRIMARY KEY (`sensor_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
        """
        
        create_sensor_readings_table = """
        CREATE TABLE IF NOT EXISTS `sensor_readings` (
          `id` bigint NOT NULL AUTO_INCREMENT,
          `sensor_id` varchar(50) NOT NULL,
          `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
          `value` varchar(255) NOT NULL,
          `unit` varchar(20) DEFAULT NULL,
          `status` varchar(50) DEFAULT 'normal',
          PRIMARY KEY (`id`),
          KEY `idx_sensor_timestamp` (`sensor_id`,`timestamp`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
        """

        create_daily_statistics_table = """
        CREATE TABLE IF NOT EXISTS `daily_statistics` (
          `id` int NOT NULL AUTO_INCREMENT,
          `record_date` date NOT NULL,
          `total_water_supply` decimal(10,2) DEFAULT '0.00',
          `total_power_generation` decimal(10,2) DEFAULT '0.00',
          `peak_flow_rate` decimal(10,2) DEFAULT NULL,
          `average_water_level` decimal(10,2) DEFAULT NULL,
          `last_updated` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
          PRIMARY KEY (`id`),
          UNIQUE KEY `record_date` (`record_date`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
        """
        
        logger.info("正在检查并按需创建数据库表...")
        self.mysql_client.execute_query(create_sensors_table)
        self.mysql_client.execute_query(create_sensor_readings_table)
        self.mysql_client.execute_query(create_daily_statistics_table)
        logger.info("✅ 数据库表结构检查完成。")

    def initialize_sensors_in_db(self):
        """
        将配置文件中的传感器信息同步到MySQL的 `sensors` 表中。
        """
        if not self.mysql_client.is_connected():
            logger.error("MySQL未连接，无法初始化传感器信息。")
            return
        
        sensors_list = []
        for sensor_id, info in self.sensors.items():
            sensors_list.append({
                "sensor_id": sensor_id,
                "sensor_type": info["type"],
                "location": info["location"],
                "name": info["name"],
                "description": f"{info['location']} 的 {info['name']} ({info['type']}) 传感器",
                # 修正: 确保states字段被正确地序列化和插入
                "states": json.dumps(info.get("states"), ensure_ascii=False) if info.get("states") else None
            })
        self.mysql_client.upsert_sensors(sensors_list)


    def generate_single_reading(self, sensor_id: str) -> Dict[str, Any]:
        """为单个传感器生成模拟数据。"""
        sensor_info = self.sensors[sensor_id]
        
        value: Any
        status = "normal" # 默认状态
        # 针对不同类型的传感器生成数据
        if sensor_info["type"] == "device_status":
            # 为设备状态生成随机状态 (0, 1, 2)
            # 80%概率是正常状态(0或1), 20%是告警(2)
            if random.random() < 0.2:
                value = 2 # 故障/告警
                status = "alert"
            else:
                value = random.randint(0, 1) # 关闭/运行
                status = "ok"
        else:
            # 为数值型传感器生成数据
            value = round(random.uniform(sensor_info["min"], sensor_info["max"]), 2)
            # 5%的概率产生突变值
            if random.random() < 0.05:
                value = round(value * random.uniform(1.2, 1.5), 2)
                status = "warning"

        # 修正: 返回的字典中不应包含 value 的中文解释
        return {
            "timestamp": datetime.now().isoformat(),
            "value": value, # 确保 device_status 发送的是数字 0, 1, 2
            "unit": sensor_info["unit"],
            "status": status,
            "sensor_id": sensor_id # 确保 sensor_id 包含在内
        }

    def update_and_broadcast_statistics(self):
        """计算、更新、持久化并广播统计数据"""
        # 1. 模拟业务逻辑：根据当前流量计算供水量和发电量增量
        # 这个计算是简化的模拟，实际应有复杂模型
        current_flow = self.generate_single_reading("FL001")['value']
        # 假设每秒流量折算为供水量 (m³/s -> 万m³)
        water_increment = (current_flow * (time.time() - self.last_stats_update_time)) / 10000 
        # 假设发电量与流量的平方成正比
        power_increment = (current_flow / 2350) ** 2 * 0.01 

        self.daily_water_supply += water_increment
        self.daily_power_generation += power_increment

        # 2. 准备广播数据
        stats_data = {
            "daily_water_supply": round(self.daily_water_supply, 2),
            "daily_power_generation": round(self.daily_power_generation, 2)
        }

        # 3. 通过Redis发布/广播
        self.redis_client.publish_statistic_data(stats_data)
        
        # 4. 持久化到MySQL
        today = datetime.now().strftime('%Y-%m-%d')
        self.mysql_client.update_daily_statistics(today, stats_data["daily_water_supply"], stats_data["daily_power_generation"])

        logger.info(f"更新统计数据: 供水={stats_data['daily_water_supply']} 万m³, 发电={stats_data['daily_power_generation']} 万kWh")
        self.last_stats_update_time = time.time()


    def run(self, interval_seconds: int = 2):
        """
        开始无限循环，持续生成并发布数据到Redis和MySQL。
        """
        if not self.redis_client.is_connected() or not self.mysql_client.is_connected():
            logger.error("Redis或MySQL未连接，数据生产者无法启动。")
            return
            
        logger.info("🚀 启动数据生产者 (Redis & MySQL 双写模式)...")
        
        # 启动时，先确保所有表都已创建
        self._create_tables_if_not_exist()
        # 然后，确保所有传感器信息都已写入数据库
        self.initialize_sensors_in_db()
        
        logger.info(f"将为 {len(self.sensors)} 个传感器生成数据，每 {interval_seconds} 秒随机更新其中一个。")
        
        sensor_ids = list(self.sensors.keys())
        stats_update_interval = 10 # 每10秒更新一次统计数据
        last_stats_broadcast_time = time.time()

        while True:
            try:
                # --- 更新瞬时传感器数据 ---
                sensor_id_to_update = random.choice(sensor_ids)
                data = self.generate_single_reading(sensor_id_to_update)
                
                # **核心改动：双写**
                # 1. 写入Redis用于实时广播
                self.redis_client.publish_sensor_data(sensor_id_to_update, data)
                
                # 2. 写入MySQL用于持久化存储
                # 确保 sensor_id 在数据中
                data_for_mysql = data.copy()
                data_for_mysql['sensor_id'] = sensor_id_to_update
                # value 统一转为字符串以便存入 VARCHAR 列
                data_for_mysql['value'] = str(data_for_mysql['value'])
                self.mysql_client.insert_sensor_reading(data_for_mysql)

                value_display = data['value']
                if isinstance(value_display, int) and 'states' in self.sensors[sensor_id_to_update]:
                     value_display = self.sensors[sensor_id_to_update]['states'][value_display]

                logger.info(f"更新 {sensor_id_to_update} ({data['name']}): value={value_display}")
                
                # --- 定时更新统计数据 ---
                current_time = time.time()
                if current_time - last_stats_broadcast_time > stats_update_interval:
                    self.update_and_broadcast_statistics()
                    last_stats_broadcast_time = current_time

                # 随机暂停0.5到2秒
                time.sleep(random.uniform(0.5, interval_seconds))
            except KeyboardInterrupt:
                logger.info("🛑 收到停止信号，数据生产者正在关闭...")
                break
            except Exception as e:
                logger.error(f"数据生成循环中发生错误: {e}")
                time.sleep(interval_seconds * 2)

def main():
    """主函数"""
    redis_cli = RedisClient()
    mysql_cli = MySQLClient(
        host='localhost',
        database='sensordatabase',
        user='root',
        password='123456'
    )
    
    producer = DataProducer(redis_cli, mysql_cli)
    producer.run()

if __name__ == '__main__':
    main()
