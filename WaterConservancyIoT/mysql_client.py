import mysql.connector
from mysql.connector import Error
import logging
from typing import Dict, Any, List, Optional
import json

# 配置日志
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class MySQLClient:
    """
    一个用于连接和操作MySQL数据库的客户端类。
    """
    def __init__(self, host: str, user: str, password: str, database: str, port: int = 3306):
        """
        初始化并建立数据库连接。
        """
        self.config = {
            'host': host,
            'user': user,
            'password': password,
            'database': database,
            'port': port,
        }
        self.connection = None
        try:
            self.connection = mysql.connector.connect(**self.config)
            if self.connection.is_connected():
                logger.info(f"✅ 成功连接到MySQL数据库 '{database}' at {host}:{port}")
        except Error as e:
            logger.error(f"❌ 连接到MySQL时出错: {e}")
            self.connection = None

    def is_connected(self) -> bool:
        """检查客户端是否成功连接。"""
        return self.connection is not None and self.connection.is_connected()

    def execute_query(self, query: str, params: Optional[tuple] = None, multi: bool = False) -> bool:
        """
        执行一个通用的SQL查询 (INSERT, UPDATE, DELETE)。
        :param query: SQL查询语句
        :param params: 查询参数（用于防止SQL注入）
        :return: 操作是否成功
        """
        if not self.is_connected():
            logger.warning("MySQL未连接，跳过查询执行。")
            return False
        
        cursor = None
        try:
            cursor = self.connection.cursor()
            if multi:
                cursor.executemany(query, params)
            else:
                cursor.execute(query, params or ())
            self.connection.commit()
            return True
        except Error as e:
            logger.error(f"执行查询时出错: {e}")
            if self.connection:
                self.connection.rollback()
            return False
        finally:
            if cursor:
                cursor.close()
    
    def upsert_sensors(self, sensors_data: List[Dict[str, Any]]) -> None:
        """
        插入或更新传感器信息。如果传感器ID已存在，则更新；否则插入新记录。
        :param sensors_data: 包含传感器信息的字典列表
        """
        query = """
            INSERT INTO sensors (sensor_id, sensor_type, location, description, name)
            VALUES (%s, %s, %s, %s, %s)
            ON DUPLICATE KEY UPDATE
            sensor_type = VALUES(sensor_type),
            location = VALUES(location),
            description = VALUES(description),
            name = VALUES(name);
        """
        # 将字典列表转换为元组列表
        values = [(s['sensor_id'], s['sensor_type'], s['location'], s['description'], s['name']) for s in sensors_data]
        try:
            # 使用 executemany 来批量插入/更新
            self.execute_query(query, values, multi=True)
            logger.info(f"成功同步 {len(values)} 个传感器信息到数据库。")
        except Error as e:
            logger.error(f"同步传感器信息失败: {e}")


    def insert_sensor_reading(self, reading_data: Dict[str, Any]) -> None:
        """
        插入一条新的传感器读数记录。
        :param reading_data: 包含传感器读数信息的字典
        """
        query = """
        INSERT INTO sensor_readings (sensor_id, `timestamp`, `value`, unit, status)
        VALUES (%s, %s, %s, %s, %s)
        """
        params = (
            reading_data['sensor_id'],
            reading_data['timestamp'],
            reading_data['value'],
            reading_data.get('unit', ''),
            reading_data.get('status', 'normal')
        )
        self.execute_query(query, params)
        # logger.info(f"成功插入传感器读数 for {reading_data['sensor_id']}")

    def update_daily_statistics(self, date_str: str, water_supply: float, power_generation: float):
        """
        更新每日的统计数据，如供水量和发电量。
        如果当天的记录已存在，则更新；否则，创建新记录。
        """
        query = """
            INSERT INTO daily_statistics (record_date, total_water_supply, total_power_generation)
            VALUES (%s, %s, %s)
            ON DUPLICATE KEY UPDATE
            total_water_supply = VALUES(total_water_supply),
            total_power_generation = VALUES(total_power_generation);
        """
        try:
            self.execute_query(query, (date_str, water_supply, power_generation))
            logger.info(f"成功更新 {date_str} 的统计数据。")
        except Error as e:
            logger.error(f"更新每日统计数据失败 (date: {date_str}): {e}")

    def get_all_sensors(self):
        """从数据库中查询并返回所有传感器的列表。"""
        query = "SELECT sensor_id, name, sensor_type, location, description, states FROM sensors ORDER BY sensor_id"
        try:
            results = self.execute_query(query, fetch="all")
            sensors = []
            if results:
                for row in results:
                    sensors.append({
                        "sensor_id": row[0],
                        "name": row[1],
                        "sensor_type": row[2],
                        "location": row[3],
                        "description": row[4],
                        "states": json.loads(row[5]) if row[5] else None,
                    })
            return sensors
        except Error as e:
            logger.error(f"查询所有传感器失败: {e}")
            return []

    def get_user_by_username(self, username: str) -> Optional[Dict[str, Any]]:
        """根据用户名查询用户信息"""
        query = "SELECT id, username, password_hash, role FROM users WHERE username = %s"
        cursor = None
        try:
            cursor = self.connection.cursor(dictionary=True)
            cursor.execute(query, (username,))
            return cursor.fetchone()
        except Error as e:
            logger.error(f"查询用户 '{username}' 失败: {e}")
            return None
        finally:
            if cursor:
                cursor.close()

    def create_user(self, username: str, password_hash: str, role: str) -> bool:
        """创建一个新用户"""
        query = "INSERT INTO users (username, password_hash, role) VALUES (%s, %s, %s)"
        return self.execute_query(query, (username, password_hash, role))

    def close(self):
        """关闭数据库连接。"""
        if self.is_connected():
            self.connection.close()
            logger.info("MySQL连接已关闭。")

def test_mysql_connection():
    """一个简单的测试函数"""
    logger.info("--- 测试MySQL客户端 ---")
    
    # **重要**: 请根据你的实际配置修改这里的参数
    db_client = MySQLClient(
        host='localhost',
        database='sensordatabase',
        user='root', # 你的MySQL用户名
        password='123456'  # 你的MySQL密码
    )

    if db_client.is_connected():
        # 1. 准备并插入传感器元数据
        mock_sensors = [
            {"sensor_id": "TEST001", "sensor_type": "water_level_test", "location": "测试水库"},
            {"sensor_id": "TEST002", "sensor_type": "flow_rate_test", "location": "测试管道"}
        ]
        db_client.upsert_sensors(mock_sensors)

        # 2. 准备并插入一条读数记录
        mock_reading = {
            "sensor_id": "TEST001",
            "timestamp": "2025-09-14T15:30:00",
            "value": 123.45,
            "unit": "m",
            "status": "high_alert"
        }
        db_client.insert_sensor_reading(mock_reading)
        
        logger.info("✅ 数据库写入测试完成。请检查 `sensordatabase` 数据库中的 `sensors` 和 `sensor_readings` 表以验证数据。")

        db_client.close()

if __name__ == '__main__':
    test_mysql_connection()
