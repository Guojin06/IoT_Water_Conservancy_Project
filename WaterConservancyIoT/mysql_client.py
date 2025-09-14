import mysql.connector
from mysql.connector import Error
import logging
from typing import Dict, Any, List, Optional

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

    def execute_query(self, query: str, params: Optional[tuple] = None) -> bool:
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
        INSERT INTO sensors (sensor_id, sensor_type, location, description)
        VALUES (%s, %s, %s, %s)
        ON DUPLICATE KEY UPDATE
            sensor_type = VALUES(sensor_type),
            location = VALUES(location),
            description = VALUES(description)
        """
        for sensor in sensors_data:
            params = (
                sensor['sensor_id'],
                sensor['sensor_type'],
                sensor.get('location', ''),
                sensor.get('description', '')
            )
            self.execute_query(query, params)
        logger.info(f"成功同步 {len(sensors_data)} 个传感器信息到数据库。")


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
