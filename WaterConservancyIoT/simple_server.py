#!/usr/bin/env python3
"""
简单的Python HTTP服务器，提供前端页面和模拟API接口
"""

import http.server
import socketserver
import json
import time
import random
import threading
from urllib.parse import urlparse, parse_qs
import os
import logging
from typing import Optional, Dict, Any

# 新增的import
from auth_service import AuthService
from mysql_client import MySQLClient
from redis_client import RedisClient

# 全局服务实例
mysql_cli = MySQLClient(host='localhost', database='sensordatabase', user='root', password='123456')
redis_cli = RedisClient()
auth_service = AuthService(mysql_cli)


class WaterIoTHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory="frontend", **kwargs)
    
    def do_POST(self):
        """处理POST请求，主要用于登录认证。"""
        parsed_path = urlparse(self.path)
        
        if parsed_path.path == '/api/auth/login':
            self.handle_login()
        else:
            self.send_error(404, "Not Found")

    def handle_login(self):
        """处理用户登录请求。"""
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        
        try:
            credentials = json.loads(post_data)
            username = credentials.get('username')
            password = credentials.get('password')

            if not username or not password:
                self.send_response(400)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({"error": "Username and password are required"}).encode())
                return

            token = auth_service.authenticate_user(username, password)

            if token:
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({"token": token}).encode())
            else:
                self.send_response(401)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({"error": "Invalid credentials"}).encode())
        except json.JSONDecodeError:
            self.send_error(400, "Bad Request: Invalid JSON")
    
    def verify_jwt(self) -> Optional[Dict[str, Any]]:
        """从请求头中提取并验证JWT。"""
        auth_header = self.headers.get('Authorization')
        if not auth_header or not auth_header.startswith('Bearer '):
            return None
        
        token = auth_header.split(" ")[1]
        payload = auth_service.decode_jwt_token(token)
        return payload

    def do_GET(self):
        """处理GET请求，包括受保护的API和静态文件。"""
        parsed_path = urlparse(self.path)
        
        # --- API路由 ---
        if parsed_path.path.startswith('/api/'):
            
            # --- 受保护的API ---
            if parsed_path.path == '/api/sensors/recent':
                payload = self.verify_jwt()
                if not payload:
                    self.send_response(401)
                    self.send_header('Content-type', 'application/json')
                    self.end_headers()
                    self.wfile.write(json.dumps({"error": "Authentication required"}).encode())
                    return
                
                # 用户已认证，可以继续处理请求
                logger.info(f"Authenticated user '{payload.get('name')}' accessed /api/sensors/recent")
                # ... (处理 /api/sensors/recent 的逻辑，例如从Redis读取数据)
                # 此处仅为演示，返回一个成功信息
                self.send_json_response({"message": "Access granted to protected sensor data!"})

            # ... (其他API路由)

            else:
                self.send_error(404, "API endpoint not found")
        
        # --- 静态文件服务 ---
        else:
            # 调用父类的方法来处理静态文件（如 index.html, dashboard.html 等）
            super().do_GET()

    def do_OPTIONS(self):
        # 处理CORS预检请求
        self.send_response(200)
        self.send_cors_headers()
        self.end_headers()
    
    def send_cors_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
    
    def handle_api_get(self, parsed_path):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.send_cors_headers()
        self.end_headers()
        
        path = parsed_path.path
        query = parse_qs(parsed_path.query)
        
        if path == '/api/status':
            response = {
                "status": "success",
                "message": "系统运行正常",
                "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
                "data": {
                    "system": "running",
                    "sensors": {
                        "processed": random.randint(150, 200),
                        "errors": random.randint(0, 3),
                        "queue_size": random.randint(0, 5),
                        "avg_time": round(random.uniform(5.0, 10.0), 1)
                    },
                    "messaging": {
                        "published": random.randint(40, 60),
                        "consumed": random.randint(35, 55),
                        "queue_size": random.randint(0, 3)
                    }
                }
            }
            
        elif path == '/api/sensors/recent':
            sensor_id = query.get('id', [''])[0]
            if not sensor_id:
                response = {"status": "error", "message": "Missing sensor id"}
            else:
                # 生成模拟数据
                data_points = []
                current_time = int(time.time())
                
                for i in range(10):
                    timestamp = current_time - (i * 30)  # 每30秒一个数据点
                    
                    if sensor_id.startswith('WL'):  # 水位传感器
                        base_value = 15.5 if sensor_id == 'WL001' else 12.3
                        value = base_value + random.uniform(-2, 2)
                    elif sensor_id.startswith('TEMP'):  # 温度传感器
                        base_value = 23.8 if sensor_id == 'TEMP001' else 24.2
                        value = base_value + random.uniform(-2, 2)
                    else:
                        value = random.uniform(10, 30)
                    
                    data_points.append({
                        "value": round(value, 2),
                        "timestamp": timestamp,
                        "status": "online"
                    })
                
                response = {
                    "status": "success",
                    "message": f"获取到 {len(data_points)} 条最近数据",
                    "data": {
                        "sensor_id": sensor_id,
                        "data": data_points
                    }
                }
        
        elif path.startswith('/api/cache/'):
            cache_key = path.replace('/api/cache/', '')
            if 'WL001' in cache_key:
                value = round(15.5 + random.uniform(-2, 2), 2)
            elif 'WL002' in cache_key:
                value = round(12.3 + random.uniform(-2, 2), 2)
            elif 'TEMP001' in cache_key:
                value = round(23.8 + random.uniform(-2, 2), 2)
            elif 'TEMP002' in cache_key:
                value = round(24.2 + random.uniform(-2, 2), 2)
            else:
                value = round(random.uniform(10, 30), 2)
            
            response = {
                "status": "success",
                "data": str(value)
            }
        
        else:
            response = {"status": "error", "message": "API endpoint not found"}
        
        self.wfile.write(json.dumps(response, ensure_ascii=False).encode('utf-8'))
    
    def handle_api_post(self, parsed_path):
        if parsed_path.path == '/api/control':
            content_length = int(self.headers['Content-Length'])
            command = self.rfile.read(content_length).decode('utf-8')
            
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.send_cors_headers()
            self.end_headers()
            
            response = {
                "status": "success",
                "message": f"指令 '{command}' 执行成功",
                "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
            }
            
            print(f"[控制指令] 收到指令: {command}")
            
            self.wfile.write(json.dumps(response, ensure_ascii=False).encode('utf-8'))
        else:
            self.send_error(404)

def start_server(port=8888):
    """启动服务器"""
    print(f"🚀 启动智能水库监测系统...")
    print(f"📡 HTTP服务器端口: {port}")
    print(f"🌐 前端界面: http://localhost:{port}")
    print(f"🔌 API接口: http://localhost:{port}/api/")
    print()
    print("📊 可用API接口:")
    print("  GET  /api/status")
    print("  GET  /api/sensors/recent?id=WL001")
    print("  GET  /api/cache/latest:WL001")
    print("  POST /api/control")
    print()
    print("✅ 系统已就绪，按 Ctrl+C 停止服务器")
    print("=" * 50)
    
    try:
        print(f"🔧 正在绑定端口 {port}...")
        httpd = socketserver.TCPServer(("", port), WaterIoTHandler)
        print(f"✅ 端口绑定成功，开始监听...")
        httpd.serve_forever()
    except OSError as e:
        print(f"❌ 端口绑定失败: {e}")
        print(f"💡 可能端口 {port} 已被占用，尝试使用其他端口")
    except KeyboardInterrupt:
        print("\n🛑 服务器已停止")
    except Exception as e:
        print(f"❌ 服务器启动失败: {e}")
    finally:
        print("🧹 清理资源...")

if __name__ == "__main__":
    # 确保在正确的目录中
    if not os.path.exists("frontend"):
        print("❌ 错误: 找不到 frontend 目录")
        print("请确保在 WaterConservancyIoT 项目根目录中运行此脚本")
        exit(1)
    
    start_server()
