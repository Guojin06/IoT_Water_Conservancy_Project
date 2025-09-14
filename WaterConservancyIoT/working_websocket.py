#!/usr/bin/env python3
"""
工作的WebSocket服务器 - 简化版本
"""
import asyncio
import websockets
import json
import logging
import redis.asyncio as aioredis  # 明确使用 aioredis
from datetime import datetime

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

clients = set()

async def handle_connection(websocket: websockets.WebSocketServerProtocol):
    """
    处理单个WebSocket连接，将其注册到客户端集合中，并等待其断开。
    """
    logger.info(f"客户端 {websocket.remote_address} 已连接。")
    clients.add(websocket)
    try:
        # 保持连接开放，直到客户端断开
        await websocket.wait_closed()
    finally:
        logger.info(f"客户端 {websocket.remote_address} 已断开。")
        clients.remove(websocket)

async def redis_listener():
    """
    监听Redis频道并将消息向所有已注册的WebSocket客户端广播。
    这是一个独立的后台任务。
    """
    while True: # 添加外层循环，确保在出错后能自动重连
        try:
            # 修正: 使用 aioredis.from_url
            async with aioredis.from_url("redis://localhost", decode_responses=True) as redis_client:
                pubsub = redis_client.pubsub()
                
                await pubsub.psubscribe("sensor:*:updates", "statistics:updates")
                logger.info("已重新订阅 Redis 频道: 'sensor:*:updates' 和 'statistics:updates'")
                
                while True:
                    message = await pubsub.get_message(ignore_subscribe_messages=True, timeout=None) # timeout=None 会一直等待
                    if message and clients:
                        data_str = message['data']
                        # 直接转发从Redis收到的原始JSON字符串
                        await asyncio.gather(
                            *[client.send(data_str) for client in clients]
                        )
        except asyncio.CancelledError:
            logger.info("Redis监听任务被取消。")
            break
        except Exception as e:
            logger.error(f"Redis监听循环出错: {e}，将在5秒后重试...")
            await asyncio.sleep(5)

async def main():
    """主函数，启动WebSocket服务器和Redis监听器。"""
    logger.info("--- 启动WebSocket服务器 (Redis集成版) ---")
    
    # 修正: 检查Redis连接也必须是异步的
    try:
        r = aioredis.from_url("redis://localhost")
        if await r.ping(): # 修正: 添加 await
            logger.info("✅ 成功连接到Redis服务器 at localhost:6379")
        await r.close()
    except Exception as e:
        logger.critical(f"❌ 无法连接到Redis服务器: {e}。请确保Redis正在运行。")
        return

    listener_task = asyncio.create_task(redis_listener())
    
    async with websockets.serve(handle_connection, "localhost", 8081):
        logger.info("🚀 WebSocket服务器已在 ws://localhost:8081 启动")
        await asyncio.Future()

if __name__ == '__main__':
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("服务器手动停止。")
