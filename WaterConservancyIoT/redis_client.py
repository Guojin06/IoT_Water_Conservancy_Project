import redis
import json
import logging
from typing import Dict, Any, Optional
from datetime import datetime

# 配置日志
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class RedisClient:
    """
    一个用于连接和操作Redis的客户端类。
    """
    def __init__(self, host: str = 'localhost', port: int = 6379, db: int = 0):
        """
        初始化Redis连接。
        :param host: Redis服务器地址
        :param port: Redis服务器端口
        :param db: Redis数据库编号
        """
        try:
            # 使用decode_responses=True确保从Redis读取的键和值是字符串
            self.client = redis.Redis(host=host, port=port, db=db, decode_responses=True)
            # 测试连接
            self.client.ping()
            logger.info(f"✅ 成功连接到Redis服务器 at {host}:{port}")
        except redis.exceptions.ConnectionError as e:
            logger.error(f"❌ 无法连接到Redis服务器: {e}")
            logger.error("请确保Redis服务器正在运行，并且主机和端口配置正确。")
            self.client = None

    def is_connected(self) -> bool:
        """检查客户端是否成功连接。"""
        return self.client is not None

    def publish_sensor_data(self, sensor_id: str, data: Dict[str, Any]) -> None:
        """
        将传感器数据发布到指定的频道，并存储最新的数据。
        :param sensor_id: 传感器ID (e.g., 'WL001')
        :param data: 传感器数据字典
        """
        if not self.is_connected():
            logger.warning("Redis未连接，跳过数据发布。")
            return

        try:
            # 频道名称，用于发布/订阅模式
            channel_name = f"sensor:{sensor_id}:updates"
            # 用于存储最新数据的键
            latest_data_key = f"sensor:{sensor_id}:latest"
            
            message = json.dumps(data)
            
            # 1. 发布到频道，通知所有监听者（例如WebSocket服务器）
            self.client.publish(channel_name, message)
            
            # 2. 将最新数据存储在-个普通的key里，方便HTTP API等直接查询
            # 使用 HSET 存储结构化数据
            self.client.hset(latest_data_key, mapping=data)
            
            # logger.info(f"成功发布并存储传感器数据 for {sensor_id}")
        except redis.exceptions.RedisError as e:
            logger.error(f"发布/存储数据到Redis时出错 for {sensor_id}: {e}")

    def get_latest_sensor_data(self, sensor_id: str) -> Optional[Dict[str, Any]]:
        """
        获取指定传感器的最新数据。
        :param sensor_id: 传感器ID
        :return: 包含最新数据的字典，如果不存在则返回None
        """
        if not self.is_connected():
            logger.warning("Redis未连接，无法获取数据。")
            return None

        try:
            latest_data_key = f"sensor:{sensor_id}:latest"
            data = self.client.hgetall(latest_data_key)
            
            if not data:
                logger.warning(f"在Redis中未找到传感器 {sensor_id} 的最新数据。")
                return None
            
            # Redis返回的值都是字符串，需要做类型转换
            # (这里简化处理，实际应用中可能需要更复杂的类型转换逻辑)
            for key, value in data.items():
                try:
                    # 尝试转换为浮点数
                    data[key] = float(value)
                except (ValueError, TypeError):
                    # 转换失败则保持为字符串
                    pass
            return data
        except redis.exceptions.RedisError as e:
            logger.error(f"从Redis获取数据时出错 for {sensor_id}: {e}")
            return None

    def publish_statistic_data(self, data: Dict[str, Any]):
        """
        将统计数据发布到指定的频道。
        """
        channel = "statistics:updates"
        message = json.dumps({
            "type": "statistic_data",
            "timestamp": datetime.now().isoformat(),
            "data": data
        })
        try:
            self.client.publish(channel, message)
            # logger.info(f"Published to {channel}: {message}")
        except Exception as e:
            logger.error(f"发布统计数据到 Redis 频道 {channel} 失败: {e}")

def test_redis_connection():
    """一个简单的测试函数"""
    logger.info("--- 测试Redis客户端 ---")
    redis_client = RedisClient()

    if redis_client.is_connected():
        # 模拟传感器数据
        mock_data = {
            "timestamp": "2025-09-14T15:00:00.000Z",
            "value": 99.8,
            "unit": "m",
            "status": "critical"
        }
        
        sensor_id = "TEST001"
        
        logger.info(f"发布测试数据 for {sensor_id}...")
        redis_client.publish_sensor_data(sensor_id, mock_data)
        
        logger.info(f"获取测试数据 for {sensor_id}...")
        retrieved_data = redis_client.get_latest_sensor_data(sensor_id)
        
        print("\n--- 结果 ---")
        print(f"获取到的数据: {retrieved_data}")
        
        # 验证数据
        if retrieved_data and retrieved_data.get('value') == mock_data['value']:
            print("✅ 数据验证成功！")
        else:
            print("❌ 数据验证失败！")
        print("------------\n")
            
if __name__ == '__main__':
    # 允许直接运行此文件来测试Redis连接
    test_redis_connection()
