#!/usr/bin/env python3
"""
简化版WebSocket演示服务器
"""

import asyncio
import websockets
import json
import time
import random
from datetime import datetime

# 传感器数据
sensors = {
    'WL001': {'name': '主水库水位', 'unit': 'm', 'value': 15.5},
    'TEMP001': {'name': '入水口温度', 'unit': '°C', 'value': 23.8},
    'FLOW001': {'name': '主管道流量', 'unit': 'm³/h', 'value': 45.6}
}

connected_clients = set()

async def handle_client(websocket, path):
    """处理客户端连接"""
    connected_clients.add(websocket)
    print(f"✅ 新客户端连接: {websocket.remote_address}")
    print(f"📊 当前连接数: {len(connected_clients)}")
    
    try:
        # 发送欢迎消息
        welcome = {
            'type': 'welcome',
            'message': '欢迎连接IoT水利监控系统',
            'timestamp': datetime.now().isoformat()
        }
        await websocket.send(json.dumps(welcome, ensure_ascii=False))
        
        # 保持连接
        async for message in websocket:
            print(f"📨 收到消息: {message}")
            
    except websockets.exceptions.ConnectionClosed:
        pass
    finally:
        connected_clients.discard(websocket)
        print(f"❌ 客户端断开: {websocket.remote_address}")
        print(f"📊 当前连接数: {len(connected_clients)}")

async def broadcast_sensor_data():
    """广播传感器数据"""
    while True:
        if connected_clients:
            for sensor_id, info in sensors.items():
                # 生成随机数据
                base_value = info['value']
                new_value = base_value + random.uniform(-2, 2)
                sensors[sensor_id]['value'] = new_value
                
                # 创建数据消息
                data = {
                    'type': 'sensor_data',
                    'timestamp': datetime.now().isoformat(),
                    'data': {
                        'sensor_id': sensor_id,
                        'sensor_name': info['name'],
                        'value': round(new_value, 2),
                        'unit': info['unit'],
                        'status': 'online'
                    }
                }
                
                # 广播给所有客户端
                message = json.dumps(data, ensure_ascii=False)
                disconnected = set()
                
                for client in connected_clients:
                    try:
                        await client.send(message)
                    except websockets.exceptions.ConnectionClosed:
                        disconnected.add(client)
                
                # 清理断开的连接
                connected_clients -= disconnected
        
        await asyncio.sleep(2)  # 每2秒发送一次数据

async def main():
    """主函数"""
    print("🚀 启动简化版WebSocket演示服务器...")
    print("📡 WebSocket地址: ws://localhost:8081")
    print("🔌 等待客户端连接...")
    print("=" * 50)
    
    # 启动数据广播任务
    broadcast_task = asyncio.create_task(broadcast_sensor_data())
    
    # 启动WebSocket服务器
    async with websockets.serve(handle_client, "localhost", 8081):
        print("✅ WebSocket服务器已启动")
        await asyncio.Future()  # 永久运行

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n🛑 服务器已停止")
    except Exception as e:
        print(f"❌ 服务器错误: {e}")
