import time
import random
import logging
from datetime import datetime
from typing import Dict, Any

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
        self.sensors: Dict[str, Dict[str, Any]] = {
            "WL001": {"type": "water_level", "location": "主水库", "min": 80, "max": 95},
            "WL002": {"type": "water_level", "location": "副水库", "min": 60, "max": 75},
            "FL001": {"type": "flow_rate", "location": "泄洪口A", "min": 100, "max": 150},
            "WQ001": {"type": "water_quality_ph", "location": "入水口", "min": 6.5, "max": 8.5},
            "WQ002": {"type": "water_quality_turbidity", "location": "出水口", "min": 0, "max": 5},
        }

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
                "description": f"{info['location']} 的 {info['type']} 传感器"
            })
        self.mysql_client.upsert_sensors(sensors_list)


    def generate_single_reading(self, sensor_id: str) -> Dict[str, Any]:
        """为单个传感器生成模拟数据。"""
        sensor_info = self.sensors[sensor_id]
        value = round(random.uniform(sensor_info["min"], sensor_info["max"]), 2)
        
        # 偶尔产生一些异常数据
        if random.random() < 0.05: # 5%的概率
            value = round(value * random.uniform(0.5, 1.5), 2)

        return {
            "timestamp": datetime.now().isoformat(),
            "value": value,
            "unit": {"water_level": "m", "flow_rate": "m³/s", "water_quality_ph": "pH", "water_quality_turbidity": "NTU"}[sensor_info["type"]],
            "status": "normal" if sensor_info["min"] <= value <= sensor_info["max"] else "alert",
            "sensor_type": sensor_info["type"],
            "location": sensor_info["location"]
        }

    def run(self, interval_seconds: int = 5):
        """
        开始无限循环，持续生成并发布数据到Redis和MySQL。
        """
        if not self.redis_client.is_connected() or not self.mysql_client.is_connected():
            logger.error("Redis或MySQL未连接，数据生产者无法启动。")
            return
            
        logger.info("🚀 启动数据生产者 (Redis & MySQL 双写模式)...")
        
        # 启动时，先确保所有传感器信息都已写入数据库
        self.initialize_sensors_in_db()
        
        logger.info(f"将为 {len(self.sensors)} 个传感器生成数据，每 {interval_seconds} 秒更新一次。")
        
        while True:
            try:
                for sensor_id in self.sensors.keys():
                    data = self.generate_single_reading(sensor_id)
                    
                    # **核心改动：双写**
                    # 1. 写入Redis用于实时广播
                    self.redis_client.publish_sensor_data(sensor_id, data)
                    
                    # 2. 写入MySQL用于持久化存储
                    mysql_data = data.copy()
                    mysql_data['sensor_id'] = sensor_id
                    self.mysql_client.insert_sensor_reading(mysql_data)

                    logger.info(f"Published data for {sensor_id} to Redis & MySQL: value={data['value']}")
                
                time.sleep(interval_seconds)
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
