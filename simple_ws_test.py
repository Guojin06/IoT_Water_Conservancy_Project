#!/usr/bin/env python3
"""
最简单的WebSocket测试服务器
"""

import asyncio
import websockets
import json
import time

print("🚀 启动WebSocket测试服务器...")

async def echo(websocket, path):
    print(f"✅ 客户端连接: {websocket.remote_address}")
    
    # 发送欢迎消息
    welcome = {
        "type": "welcome",
        "message": "WebSocket连接成功！",
        "time": time.strftime("%H:%M:%S")
    }
    await websocket.send(json.dumps(welcome))
    
    # 每秒发送测试数据
    counter = 1
    try:
        while True:
            test_data = {
                "type": "test_data", 
                "counter": counter,
                "message": f"测试数据 #{counter}",
                "time": time.strftime("%H:%M:%S")
            }
            await websocket.send(json.dumps(test_data))
            print(f"📤 发送数据: {counter}")
            counter += 1
            await asyncio.sleep(2)  # 每2秒发送一次
            
    except websockets.exceptions.ConnectionClosed:
        print("❌ 客户端断开连接")

# 启动服务器
start_server = websockets.serve(echo, "localhost", 8081)
print("📡 WebSocket服务器地址: ws://localhost:8081")
print("✅ 服务器已启动，等待连接...")

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()

