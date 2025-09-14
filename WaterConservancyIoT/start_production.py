#!/usr/bin/env python3
"""
智能水利系统 - 生产环境启动脚本
统一管理所有服务的启动、停止和监控
"""

import subprocess
import sys
import time
import os
import signal
import logging
from concurrent.futures import ThreadPoolExecutor
import threading

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('logs/system.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

class ProductionManager:
    """生产环境服务管理器"""
    
    def __init__(self):
        self.processes = {}
        self.executor = ThreadPoolExecutor(max_workers=4)
        self.shutdown_event = threading.Event()
        
        # 服务配置
        self.services = {
            'data_producer': {
                'script': 'data_producer.py',
                'description': '数据生产者',
                'critical': True
            },
            'websocket_server': {
                'script': 'working_websocket.py', 
                'description': 'WebSocket服务器',
                'critical': True
            },
            'http_server': {
                'script': 'simple_server.py',
                'description': 'HTTP服务器',
                'critical': True
            }
        }
    
    def check_environment(self):
        """检查运行环境"""
        logger.info("🔍 检查运行环境...")
        
        # 检查Python版本
        if sys.version_info < (3, 9):
            logger.error("❌ Python版本必须 >= 3.9")
            return False
        
        # 检查必要的文件
        required_files = ['data_producer.py', 'working_websocket.py', 'simple_server.py']
        for file in required_files:
            if not os.path.exists(file):
                logger.error(f"❌ 缺少文件: {file}")
                return False
        
        # 检查依赖包
        try:
            import redis
            import mysql.connector
            import websockets
            import bcrypt
            import jwt
            logger.info("✅ 所有依赖包检查通过")
        except ImportError as e:
            logger.error(f"❌ 缺少依赖包: {e}")
            logger.info("💡 请运行: pip install -r requirements.txt")
            return False
        
        # 检查端口占用
        import socket
        ports = [8081, 8888]
        for port in ports:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                if s.connect_ex(('localhost', port)) == 0:
                    logger.warning(f"⚠️  端口 {port} 已被占用")
        
        logger.info("✅ 环境检查完成")
        return True
    
    def start_service(self, name, config):
        """启动单个服务"""
        try:
            logger.info(f"🚀 启动 {config['description']}...")
            
            # 启动进程
            process = subprocess.Popen([
                sys.executable, config['script']
            ], stdout=subprocess.PIPE, stderr=subprocess.PIPE, 
               universal_newlines=True)
            
            self.processes[name] = {
                'process': process,
                'config': config,
                'start_time': time.time()
            }
            
            logger.info(f"✅ {config['description']} 启动成功 (PID: {process.pid})")
            return True
            
        except Exception as e:
            logger.error(f"❌ {config['description']} 启动失败: {e}")
            return False
    
    def monitor_service(self, name):
        """监控单个服务"""
        while not self.shutdown_event.is_set():
            if name not in self.processes:
                break
                
            process_info = self.processes[name]
            process = process_info['process']
            
            # 检查进程是否还在运行
            if process.poll() is not None:
                logger.error(f"❌ {process_info['config']['description']} 意外退出")
                
                # 如果是关键服务，尝试重启
                if process_info['config'].get('critical', False):
                    logger.info(f"🔄 尝试重启 {process_info['config']['description']}...")
                    if self.start_service(name, process_info['config']):
                        logger.info(f"✅ {process_info['config']['description']} 重启成功")
                    else:
                        logger.error(f"❌ {process_info['config']['description']} 重启失败")
            
            time.sleep(5)  # 每5秒检查一次
    
    def start_all_services(self):
        """启动所有服务"""
        logger.info("🚀 启动智能水利系统...")
        logger.info("=" * 50)
        
        success_count = 0
        for name, config in self.services.items():
            if self.start_service(name, config):
                success_count += 1
                # 启动监控线程
                self.executor.submit(self.monitor_service, name)
                time.sleep(2)  # 错开启动时间
        
        if success_count == len(self.services):
            logger.info("🎉 所有服务启动成功！")
            logger.info("🌐 访问地址: http://localhost:8888")
            logger.info("👤 默认账户: admin / admin123")
            logger.info("=" * 50)
            return True
        else:
            logger.error(f"❌ 仅 {success_count}/{len(self.services)} 个服务启动成功")
            return False
    
    def stop_all_services(self):
        """停止所有服务"""
        logger.info("🛑 正在停止所有服务...")
        
        # 设置停止事件
        self.shutdown_event.set()
        
        # 停止所有进程
        for name, process_info in self.processes.items():
            try:
                process = process_info['process']
                logger.info(f"🛑 停止 {process_info['config']['description']}...")
                
                # 优雅地终止进程
                process.terminate()
                
                # 等待最多10秒
                try:
                    process.wait(timeout=10)
                    logger.info(f"✅ {process_info['config']['description']} 已停止")
                except subprocess.TimeoutExpired:
                    # 强制杀死进程
                    process.kill()
                    logger.warning(f"⚠️  强制终止 {process_info['config']['description']}")
                    
            except Exception as e:
                logger.error(f"❌ 停止服务时出错: {e}")
        
        # 关闭线程池
        self.executor.shutdown(wait=True)
        logger.info("✅ 所有服务已停止")
    
    def show_status(self):
        """显示服务状态"""
        logger.info("📊 服务状态:")
        logger.info("-" * 40)
        
        for name, process_info in self.processes.items():
            process = process_info['process']
            config = process_info['config']
            uptime = time.time() - process_info['start_time']
            
            if process.poll() is None:
                status = "🟢 运行中"
            else:
                status = "🔴 已停止"
            
            logger.info(f"{config['description']}: {status} (运行时间: {uptime:.0f}秒)")
    
    def run(self):
        """运行生产环境"""
        try:
            # 创建日志目录
            os.makedirs('logs', exist_ok=True)
            
            # 检查环境
            if not self.check_environment():
                sys.exit(1)
            
            # 启动所有服务
            if not self.start_all_services():
                logger.error("❌ 系统启动失败")
                sys.exit(1)
            
            # 主循环
            logger.info("✅ 系统正在运行，按 Ctrl+C 退出...")
            try:
                while True:
                    time.sleep(10)
                    # 可以在这里添加定期检查逻辑
                    
            except KeyboardInterrupt:
                logger.info("🛑 收到停止信号...")
            
        except Exception as e:
            logger.error(f"❌ 系统运行出错: {e}")
        finally:
            self.stop_all_services()

def main():
    """主函数"""
    print("""
    ╔══════════════════════════════════════════════╗
    ║          智能水利系统 - 生产环境启动器          ║
    ║                 Version 1.0.0                ║
    ╚══════════════════════════════════════════════╝
    """)
    
    manager = ProductionManager()
    
    # 注册信号处理器
    def signal_handler(signum, frame):
        logger.info("🛑 收到停止信号，正在优雅关闭...")
        manager.stop_all_services()
        sys.exit(0)
    
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    # 运行系统
    manager.run()

if __name__ == "__main__":
    main()
