#!/usr/bin/env python3
"""
安装WebSocket实时通信依赖
"""

import subprocess
import sys
import os

def install_package(package):
    """安装Python包"""
    try:
        print(f"📦 安装 {package}...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", package])
        print(f"✅ {package} 安装成功")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ {package} 安装失败: {e}")
        return False

def main():
    """主函数"""
    print("🔧 安装WebSocket实时通信依赖...")
    print("=" * 50)
    
    # 需要安装的包
    packages = [
        'websockets',  # WebSocket服务器
        'asyncio-mqtt',  # 异步MQTT客户端（可选）
    ]
    
    success_count = 0
    
    for package in packages:
        if install_package(package):
            success_count += 1
        print()
    
    print("=" * 50)
    print(f"📊 安装结果: {success_count}/{len(packages)} 个包安装成功")
    
    if success_count == len(packages):
        print("🎉 所有依赖安装完成！")
        print("✅ 现在可以运行实时系统了")
        print()
        print("🚀 启动命令:")
        print("   python start_realtime_system.py")
    else:
        print("⚠️ 部分依赖安装失败，请检查网络连接或手动安装")
        print()
        print("🔧 手动安装命令:")
        for package in packages:
            print(f"   pip install {package}")

if __name__ == "__main__":
    main()
