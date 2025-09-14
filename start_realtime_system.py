#!/usr/bin/env python3
"""
启动完整的实时水利监控系统
包括HTTP服务器、WebSocket服务器和AI服务
"""

import subprocess
import sys
import time
import os
from threading import Thread

def start_http_server():
    """启动HTTP服务器"""
    print("🌐 启动HTTP服务器...")
    try:
        subprocess.run([sys.executable, "simple_server.py"], cwd=".")
    except KeyboardInterrupt:
        print("🛑 HTTP服务器已停止")
    except Exception as e:
        print(f"❌ HTTP服务器启动失败: {e}")

def start_websocket_server():
    """启动WebSocket服务器"""
    print("🔌 启动WebSocket服务器...")
    try:
        subprocess.run([sys.executable, "websocket_server.py"], cwd=".")
    except KeyboardInterrupt:
        print("🛑 WebSocket服务器已停止")
    except Exception as e:
        print(f"❌ WebSocket服务器启动失败: {e}")

def start_ai_service():
    """启动AI服务"""
    print("🤖 启动AI服务...")
    try:
        # 检查是否存在虚拟环境
        ai_env_path = "ai_service/ai_env"
        if os.path.exists(ai_env_path):
            if sys.platform == "win32":
                python_path = os.path.join(ai_env_path, "Scripts", "python.exe")
            else:
                python_path = os.path.join(ai_env_path, "bin", "python")
            
            if os.path.exists(python_path):
                subprocess.run([python_path, "app.py"], cwd="ai_service")
            else:
                subprocess.run([sys.executable, "app.py"], cwd="ai_service")
        else:
            subprocess.run([sys.executable, "app.py"], cwd="ai_service")
    except KeyboardInterrupt:
        print("🛑 AI服务已停止")
    except Exception as e:
        print(f"❌ AI服务启动失败: {e}")

def check_dependencies():
    """检查依赖"""
    print("🔍 检查系统依赖...")
    
    # 检查WebSocket依赖
    try:
        import websockets
        print("✅ websockets库已安装")
    except ImportError:
        print("❌ 缺少websockets库，请运行: pip install websockets")
        return False
    
    # 检查其他依赖
    dependencies = ['asyncio', 'json', 'time', 'random', 'threading', 'datetime']
    for dep in dependencies:
        try:
            __import__(dep)
            print(f"✅ {dep}模块可用")
        except ImportError:
            print(f"❌ 缺少{dep}模块")
            return False
    
    return True

def main():
    """主函数"""
    print("=" * 60)
    print("🚀 IoT智能水利监控系统 - 实时版本")
    print("=" * 60)
    print()
    
    if not check_dependencies():
        print("❌ 依赖检查失败，请安装缺少的依赖后重试")
        return
    
    print("📋 系统服务清单:")
    print("  1. HTTP服务器 (端口: 8888) - 前端页面和API")
    print("  2. WebSocket服务器 (端口: 8081) - 实时数据推送")
    print("  3. AI分析服务 (端口: 5000) - 智能分析功能")
    print()
    
    print("🌐 访问地址:")
    print("  前端界面: http://localhost:8888")
    print("  AI分析接口: http://localhost:5000")
    print("  WebSocket: ws://localhost:8081")
    print()
    
    print("🚀 正在启动所有服务...")
    print("=" * 60)
    
    # 创建线程启动各个服务
    threads = []
    
    # 启动HTTP服务器
    http_thread = Thread(target=start_http_server, daemon=True)
    http_thread.start()
    threads.append(http_thread)
    time.sleep(1)
    
    # 启动WebSocket服务器
    ws_thread = Thread(target=start_websocket_server, daemon=True)
    ws_thread.start()
    threads.append(ws_thread)
    time.sleep(1)
    
    # 启动AI服务
    ai_thread = Thread(target=start_ai_service, daemon=True)
    ai_thread.start()
    threads.append(ai_thread)
    time.sleep(2)
    
    print()
    print("✅ 所有服务已启动！")
    print("🎉 系统就绪，可以开始使用")
    print("=" * 60)
    print()
    print("💡 使用说明:")
    print("  - 打开浏览器访问: http://localhost:8888")
    print("  - 选择监控大屏查看实时数据")
    print("  - 实时数据每秒自动更新")
    print("  - 异常情况会自动弹出告警")
    print()
    print("⚠️  注意: 按 Ctrl+C 停止所有服务")
    print("=" * 60)
    
    try:
        # 保持主线程运行
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n🛑 正在停止所有服务...")
        print("👋 系统已关闭，再见！")

if __name__ == "__main__":
    main()
