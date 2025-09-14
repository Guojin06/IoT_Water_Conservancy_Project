#!/usr/bin/env python3
"""
工作的WebSocket服务器 - 简化版本
"""
import asyncio
import websockets
import json
import logging
from datetime import datetime
import random

# 设置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# 连接的客户端
clients = set()

async def register_client(websocket):
    """注册客户端"""
    clients.add(websocket)
    logger.info(f"客户端连接: {len(clients)} 个客户端")
    
    # 发送欢迎消息
    welcome = {
        "type": "system_status",
        "timestamp": datetime.now().isoformat(),
        "data": {
            "status": "connected",
            "message": "✅ 连接成功！",
            "clients": len(clients)
        }
    }
    await websocket.send(json.dumps(welcome, ensure_ascii=False))

async def unregister_client(websocket):
    """注销客户端"""
    clients.discard(websocket)
    logger.info(f"客户端断开: {len(clients)} 个客户端")

async def handle_client(websocket, path):
    """处理客户端连接"""
    await register_client(websocket)
    
    try:
        # 发送初始数据
        test_data = {
            "type": "sensor_data",
            "timestamp": datetime.now().isoformat(),
            "data": {
                "sensor_id": "WL001",
                "sensor_type": "water_level",
                "location": "主水库",
                "value": round(random.uniform(80, 95), 2),
                "unit": "m",
                "status": "normal"
            }
        }
        await websocket.send(json.dumps(test_data, ensure_ascii=False))
        
        # 监听客户端消息
        async for message in websocket:
            logger.info(f"收到消息: {message}")
            
            # 回应
            response = {
                "type": "response",
                "timestamp": datetime.now().isoformat(),
                "data": {
                    "status": "received",
                    "message": "消息已收到"
                }
            }
            await websocket.send(json.dumps(response, ensure_ascii=False))
            
    except websockets.exceptions.ConnectionClosed:
        logger.info("连接正常关闭")
    except Exception as e:
        logger.error(f"处理客户端错误: {e}")
    finally:
        await unregister_client(websocket)

async def send_periodic_data():
    """定期发送数据"""
    while True:
        if clients:
            # 生成传感器数据
            sensor_data = {
                "type": "sensor_data",
                "timestamp": datetime.now().isoformat(),
                "data": {
                    "sensor_id": f"WL00{random.randint(1,3)}",
                    "sensor_type": "water_level",
                    "location": "主水库",
                    "value": round(random.uniform(75, 100), 2),
                    "unit": "m",
                    "status": "normal"
                }
            }
            
            # 发送给所有客户端
            if clients:
                message = json.dumps(sensor_data, ensure_ascii=False)
                disconnected = []
                for client in clients.copy():
                    try:
                        await client.send(message)
                    except:
                        disconnected.append(client)
                
                # 清理断开的连接
                for client in disconnected:
                    clients.discard(client)
        
        await asyncio.sleep(5)  # 每5秒发送一次

async def main():
    """主函数"""
    logger.info("🚀 启动WebSocket服务器...")
    logger.info("📡 地址: ws://localhost:8081")
    
    # 启动定期数据发送任务
    data_task = asyncio.create_task(send_periodic_data())
    
    # 启动WebSocket服务器
    async with websockets.serve(handle_client, "localhost", 8081):
        logger.info("✅ 服务器已启动")
        logger.info("=" * 50)
        
        try:
            await asyncio.Future()  # 永久运行
        except KeyboardInterrupt:
            logger.info("🛑 停止服务器...")
        finally:
            data_task.cancel()

if __name__ == "__main__":
    asyncio.run(main())
