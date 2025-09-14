#!/usr/bin/env python3
"""
WebSocket实时通信服务器
支持传感器数据实时推送和告警通知
"""

import asyncio
import websockets
import json
import time
import random
import threading
from datetime import datetime
from typing import Set, Dict, Any
import logging

# 配置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class SensorDataGenerator:
    """传感器数据生成器"""
    
    def __init__(self):
        self.sensors = {
            'WL001': {'name': '主水库水位', 'type': 'water_level', 'unit': 'm', 'base_value': 15.5},
            'WL002': {'name': '副水库水位', 'type': 'water_level', 'unit': 'm', 'base_value': 12.3},
            'TEMP001': {'name': '入水口温度', 'type': 'temperature', 'unit': '°C', 'base_value': 23.8},
            'TEMP002': {'name': '出水口温度', 'type': 'temperature', 'unit': '°C', 'base_value': 24.2},
            'FLOW001': {'name': '主管道流量', 'type': 'flow_rate', 'unit': 'm³/h', 'base_value': 45.6},
            'PH001': {'name': '水质pH值', 'type': 'ph_level', 'unit': 'pH', 'base_value': 7.2}
        }
        self.last_values = {}
        self.alert_thresholds = {
            'water_level': {'min': 10.0, 'max': 20.0},
            'temperature': {'min': 15.0, 'max': 30.0},
            'flow_rate': {'min': 20.0, 'max': 80.0},
            'ph_level': {'min': 6.5, 'max': 8.5}
        }
    
    def generate_sensor_data(self, sensor_id: str) -> Dict[str, Any]:
        """生成单个传感器数据"""
        if sensor_id not in self.sensors:
            return None
            
        sensor_info = self.sensors[sensor_id]
        base_value = sensor_info['base_value']
        
        # 生成带趋势的随机值
        if sensor_id in self.last_values:
            # 基于上一个值生成，增加连续性
            last_value = self.last_values[sensor_id]
            change = random.uniform(-0.5, 0.5)
            new_value = last_value + change
            # 确保值在合理范围内
            new_value = max(base_value * 0.7, min(base_value * 1.3, new_value))
        else:
            new_value = base_value + random.uniform(-2, 2)
        
        self.last_values[sensor_id] = new_value
        
        # 确定数据质量
        quality = 'good'
        if random.random() < 0.05:  # 5%概率数据质量异常
            quality = random.choice(['warning', 'poor'])
        
        # 确定传感器状态
        status = 'online'
        if random.random() < 0.02:  # 2%概率离线
            status = 'offline'
        
        return {
            'sensor_id': sensor_id,
            'sensor_name': sensor_info['name'],
            'sensor_type': sensor_info['type'],
            'value': round(new_value, 2),
            'unit': sensor_info['unit'],
            'status': status,
            'quality': quality,
            'timestamp': datetime.now().isoformat()
        }
    
    def check_alerts(self, sensor_data: Dict[str, Any]) -> Dict[str, Any]:
        """检查是否需要生成告警"""
        sensor_type = sensor_data['sensor_type']
        value = sensor_data['value']
        
        if sensor_type not in self.alert_thresholds:
            return None
            
        threshold = self.alert_thresholds[sensor_type]
        
        alert = None
        if value < threshold['min']:
            alert = {
                'alert_id': f"ALT_{int(time.time())}",
                'severity': 'warning',
                'title': f"{sensor_data['sensor_name']}数值过低",
                'message': f"{sensor_data['sensor_name']}当前值{value}{sensor_data['unit']}，低于最小阈值{threshold['min']}{sensor_data['unit']}",
                'sensor_id': sensor_data['sensor_id'],
                'value': value,
                'threshold': threshold['min'],
                'auto_dismiss': False,
                'timestamp': datetime.now().isoformat()
            }
        elif value > threshold['max']:
            alert = {
                'alert_id': f"ALT_{int(time.time())}",
                'severity': 'critical' if value > threshold['max'] * 1.2 else 'warning',
                'title': f"{sensor_data['sensor_name']}数值过高",
                'message': f"{sensor_data['sensor_name']}当前值{value}{sensor_data['unit']}，超过最大阈值{threshold['max']}{sensor_data['unit']}",
                'sensor_id': sensor_data['sensor_id'],
                'value': value,
                'threshold': threshold['max'],
                'auto_dismiss': False,
                'timestamp': datetime.now().isoformat()
            }
        
        return alert

class WebSocketServer:
    """WebSocket服务器"""
    
    def __init__(self, host='localhost', port=8081):
        self.host = host
        self.port = port
        self.clients: Set[websockets.WebSocketServerProtocol] = set()
        self.data_generator = SensorDataGenerator()
        self.running = False
        
    async def register_client(self, websocket):
        """注册新客户端"""
        self.clients.add(websocket)
        logger.info(f"新客户端连接: {websocket.remote_address}, 总连接数: {len(self.clients)}")
        
        # 发送欢迎消息
        welcome_msg = {
            'type': 'welcome',
            'message': '欢迎连接到IoT水利监控系统WebSocket服务',
            'timestamp': datetime.now().isoformat(),
            'server_info': {
                'version': '1.0.0',
                'features': ['real_time_data', 'alerts', 'system_status']
            }
        }
        await websocket.send(json.dumps(welcome_msg, ensure_ascii=False))
    
    async def unregister_client(self, websocket):
        """注销客户端"""
        self.clients.discard(websocket)
        logger.info(f"客户端断开: {websocket.remote_address}, 剩余连接数: {len(self.clients)}")
    
    async def broadcast_message(self, message: Dict[str, Any]):
        """广播消息给所有客户端"""
        if not self.clients:
            return
            
        message_str = json.dumps(message, ensure_ascii=False)
        disconnected_clients = set()
        
        for client in self.clients.copy():
            try:
                await client.send(message_str)
            except websockets.exceptions.ConnectionClosed:
                disconnected_clients.add(client)
            except Exception as e:
                logger.error(f"发送消息失败: {e}")
                disconnected_clients.add(client)
        
        # 清理断开的连接
        for client in disconnected_clients:
            self.clients.discard(client)
    
    async def handle_client_message(self, websocket, message: str):
        """处理客户端消息"""
        try:
            data = json.loads(message)
            msg_type = data.get('type', 'unknown')
            
            if msg_type == 'ping':
                # 心跳检测
                pong_msg = {
                    'type': 'pong',
                    'timestamp': datetime.now().isoformat()
                }
                await websocket.send(json.dumps(pong_msg, ensure_ascii=False))
                
            elif msg_type == 'subscribe':
                # 订阅特定传感器数据
                sensor_ids = data.get('sensor_ids', [])
                response = {
                    'type': 'subscription_confirmed',
                    'sensor_ids': sensor_ids,
                    'timestamp': datetime.now().isoformat()
                }
                await websocket.send(json.dumps(response, ensure_ascii=False))
                
            elif msg_type == 'get_status':
                # 获取系统状态
                status_msg = {
                    'type': 'system_status',
                    'data': {
                        'server_status': 'running',
                        'connected_clients': len(self.clients),
                        'active_sensors': len(self.data_generator.sensors),
                        'uptime': '运行中',
                        'last_update': datetime.now().isoformat()
                    },
                    'timestamp': datetime.now().isoformat()
                }
                await websocket.send(json.dumps(status_msg, ensure_ascii=False))
                
        except json.JSONDecodeError:
            error_msg = {
                'type': 'error',
                'message': '无效的JSON格式',
                'timestamp': datetime.now().isoformat()
            }
            await websocket.send(json.dumps(error_msg, ensure_ascii=False))
        except Exception as e:
            logger.error(f"处理客户端消息失败: {e}")
    
    async def client_handler(self, websocket, path=None):
        """客户端连接处理器"""
        await self.register_client(websocket)
        
        try:
            async for message in websocket:
                await self.handle_client_message(websocket, message)
        except websockets.exceptions.ConnectionClosed:
            pass
        except Exception as e:
            logger.error(f"客户端处理错误: {e}")
        finally:
            await self.unregister_client(websocket)
    
    async def data_publisher(self):
        """数据发布循环"""
        logger.info("开始数据发布循环...")
        
        while self.running:
            try:
                # 生成所有传感器数据
                for sensor_id in self.data_generator.sensors.keys():
                    sensor_data = self.data_generator.generate_sensor_data(sensor_id)
                    
                    if sensor_data:
                        # 发送传感器数据
                        data_message = {
                            'type': 'sensor_data',
                            'timestamp': datetime.now().isoformat(),
                            'data': sensor_data
                        }
                        await self.broadcast_message(data_message)
                        
                        # 检查告警
                        alert = self.data_generator.check_alerts(sensor_data)
                        if alert:
                            alert_message = {
                                'type': 'alert',
                                'timestamp': datetime.now().isoformat(),
                                'data': alert
                            }
                            await self.broadcast_message(alert_message)
                            logger.warning(f"发送告警: {alert['title']}")
                
                # 等待1秒后继续
                await asyncio.sleep(1)
                
            except Exception as e:
                logger.error(f"数据发布错误: {e}")
                await asyncio.sleep(1)
    
    async def start_server(self):
        """启动WebSocket服务器"""
        self.running = True
        
        logger.info(f"🚀 启动WebSocket服务器...")
        logger.info(f"📡 WebSocket地址: ws://{self.host}:{self.port}")
        logger.info(f"🔌 支持的消息类型: sensor_data, alert, system_status")
        logger.info(f"📊 监控传感器数量: {len(self.data_generator.sensors)}")
        
        # 启动数据发布任务
        publisher_task = asyncio.create_task(self.data_publisher())
        
        # 创建处理函数包装器
        async def handler(websocket, path):
            await self.client_handler(websocket, path)
        
        # 启动WebSocket服务器
        async with websockets.serve(handler, self.host, self.port):
            logger.info("✅ WebSocket服务器已启动，等待客户端连接...")
            logger.info("=" * 50)
            
            # 保持服务器运行
            try:
                await asyncio.Future()  # 永久等待
            except KeyboardInterrupt:
                logger.info("\n🛑 收到停止信号...")
            finally:
                self.running = False
                publisher_task.cancel()
                try:
                    await publisher_task
                except asyncio.CancelledError:
                    pass
                logger.info("🧹 WebSocket服务器已停止")

def main():
    """主函数"""
    server = WebSocketServer(host='localhost', port=8081)
    
    try:
        asyncio.run(server.start_server())
    except KeyboardInterrupt:
        print("\n👋 再见!")
    except Exception as e:
        logger.error(f"服务器启动失败: {e}")

if __name__ == "__main__":
    main()
