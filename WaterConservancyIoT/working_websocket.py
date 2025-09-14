#!/usr/bin/env python3
"""
工作的WebSocket服务器 - 简化版本
"""
import asyncio
import websockets
import json
import logging
import redis.asyncio as redis # 使用异步版本的redis库
from redis_client import RedisClient # 仍然使用同步客户端进行初始连接检查
from datetime import datetime

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

clients = set()

async def handle_connection(websocket):
    """处理单个客户端连接。"""
    logger.info(f"客户端连接: {websocket.remote_address}")
    clients.add(websocket)
    try:
        welcome_message = {"type": "system_status", "data": {"status": "connected", "message": "Welcome!"}}
        await websocket.send(json.dumps(welcome_message))
        await websocket.wait_closed()
    except websockets.exceptions.ConnectionClosed as e:
        logger.warning(f"与客户端 {websocket.remote_address} 的连接关闭: {e}")
    except Exception as e:
        logger.error(f"与客户端 {websocket.remote_address} 通信时出错: {e}", exc_info=True)
    finally:
        logger.info(f"客户端断开连接: {websocket.remote_address}")
        clients.remove(websocket)

async def redis_listener():
    """
    监听Redis频道并向所有WebSocket客户端广播数据。
    """
    # 必须使用异步redis库来实现发布/订阅监听
    r = redis.Redis(host='localhost', port=6379, decode_responses=True)
    pubsub = r.pubsub()
    
    # 订阅所有传感器更新频道
    await pubsub.psubscribe("sensor:*:updates")
    logger.info("已订阅Redis频道 'sensor:*:updates'")
    
    while True:
        try:
            message = await pubsub.get_message(ignore_subscribe_messages=True, timeout=1.0)
            if message:
                channel = message['channel']
                data_str = message['data']
                logger.info(f"从Redis频道 '{channel}' 收到新数据")
                
                # 重新构造要发送给前端的数据包
                # data_str 是一个JSON字符串，我们直接转发它
                try:
                    sensor_data = json.loads(data_str)
                    sensor_id = channel.split(':')[1] # 从 'sensor:WL001:updates' 中提取 'WL001'
                    
                    frontend_message = {
                        "type": "sensor_data",
                        "timestamp": sensor_data.get("timestamp", datetime.now().isoformat()),
                        "data": {
                            "sensor_id": sensor_id,
                            **sensor_data
                        }
                    }
                    
                    if clients:
                        message_to_send = json.dumps(frontend_message)
                        await asyncio.gather(
                            *[client.send(message_to_send) for client in clients]
                        )
                except (json.JSONDecodeError, IndexError) as e:
                    logger.error(f"处理来自Redis的消息时出错: {e} - 原始数据: {data_str}")

        except Exception as e:
            logger.error(f"Redis监听循环出错: {e}", exc_info=True)
            # 在重试前等待一段时间
            await asyncio.sleep(5)

async def main():
    """主函数，启动服务器和Redis监听器。"""
    logger.info("--- 启动WebSocket服务器 (Redis集成版) ---")

    # 在启动前，先检查一下同步连接是否正常，给用户一个即时反馈
    sync_redis_client = RedisClient()
    if not sync_redis_client.is_connected():
        logger.critical("无法连接到Redis。WebSocket服务器将无法广播数据。请先启动Redis。")
        return

    # 启动Redis监听任务
    listener_task = asyncio.create_task(redis_listener())
    
    # 启动WebSocket服务器
    server = await websockets.serve(handle_connection, "localhost", 8081)
    
    logger.info("✅ WebSocket服务器已成功启动于 ws://localhost:8081")
    logger.info("等待客户端连接...")
    
    await server.wait_closed()
    listener_task.cancel()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("服务器正在关闭。")
    except Exception as e:
        logger.critical(f"服务器启动失败: {e}", exc_info=True)
