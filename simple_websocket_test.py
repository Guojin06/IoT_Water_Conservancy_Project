#!/usr/bin/env python3
"""
简单的WebSocket测试服务器
"""
import asyncio
import websockets
import json
import logging
from datetime import datetime

# 设置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class SimpleWebSocketServer:
    def __init__(self):
        self.clients = set()
    
    async def register_client(self, websocket):
        """注册客户端"""
        self.clients.add(websocket)
        logger.info(f"客户端连接: {websocket.remote_address}, 总数: {len(self.clients)}")
        
        # 发送欢迎消息
        welcome = {
            "type": "system_status",
            "timestamp": datetime.now().isoformat(),
            "data": {
                "status": "connected",
                "message": "连接成功！",
                "clients": len(self.clients)
            }
        }
        await websocket.send(json.dumps(welcome, ensure_ascii=False))
    
    async def unregister_client(self, websocket):
        """注销客户端"""
        self.clients.discard(websocket)
        logger.info(f"客户端断开: {websocket.remote_address}, 剩余: {len(self.clients)}")
    
    async def handle_client(self, websocket, path):
        """处理客户端连接"""
        await self.register_client(websocket)
        
        try:
            # 发送测试数据
            test_data = {
                "type": "sensor_data",
                "timestamp": datetime.now().isoformat(),
                "data": {
                    "sensor_id": "WL001",
                    "sensor_type": "water_level",
                    "location": "主水库",
                    "value": 85.5,
                    "unit": "m",
                    "status": "normal"
                }
            }
            await websocket.send(json.dumps(test_data, ensure_ascii=False))
            
            # 等待客户端消息
            async for message in websocket:
                logger.info(f"收到消息: {message}")
                
                # 回应客户端
                response = {
                    "type": "response",
                    "timestamp": datetime.now().isoformat(),
                    "data": {
                        "status": "received",
                        "original_message": message
                    }
                }
                await websocket.send(json.dumps(response, ensure_ascii=False))
                
        except websockets.exceptions.ConnectionClosed:
            logger.info("连接正常关闭")
        except Exception as e:
            logger.error(f"处理客户端错误: {e}")
        finally:
            await self.unregister_client(websocket)

async def main():
    """主函数"""
    server = SimpleWebSocketServer()
    
    logger.info("🚀 启动简单WebSocket测试服务器...")
    logger.info("📡 地址: ws://localhost:8081")
    
    async with websockets.serve(server.handle_client, "localhost", 8081):
        logger.info("✅ 服务器已启动")
        await asyncio.Future()  # 永久运行

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n👋 服务器已停止")
