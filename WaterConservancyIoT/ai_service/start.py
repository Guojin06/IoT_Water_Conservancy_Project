#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
AI服务启动脚本
自动检查环境和配置
"""

import os
import sys
import subprocess
import importlib.util

def check_python_version():
    """检查Python版本"""
    if sys.version_info < (3, 7):
        print("❌ Python版本过低，需要Python 3.7+")
        print(f"   当前版本: {sys.version}")
        return False
    print(f"✅ Python版本: {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}")
    return True

def check_dependencies():
    """检查依赖包"""
    required_packages = [
        'flask',
        'flask_cors',
        'requests',
        'openai'
    ]
    
    missing_packages = []
    
    for package in required_packages:
        try:
            importlib.import_module(package.replace('-', '_'))
            print(f"✅ {package}")
        except ImportError:
            missing_packages.append(package)
            print(f"❌ {package}")
    
    if missing_packages:
        print(f"\n⚠️  缺少依赖包: {', '.join(missing_packages)}")
        print("请运行以下命令安装:")
        print(f"pip install {' '.join(missing_packages)}")
        return False
    
    return True

def check_config():
    """检查配置文件"""
    config_file = 'config.py'
    example_file = 'config_example.py'
    
    if not os.path.exists(config_file):
        if os.path.exists(example_file):
            print(f"⚠️  未找到配置文件 {config_file}")
            print(f"💡 请复制 {example_file} 为 {config_file} 并配置API密钥")
            
            # 询问是否自动创建配置文件
            try:
                choice = input("是否创建默认配置文件? (y/n): ").lower().strip()
                if choice in ['y', 'yes', '是']:
                    import shutil
                    shutil.copy(example_file, config_file)
                    print(f"✅ 已创建 {config_file}")
                    print("💡 请编辑配置文件填入API密钥，或使用演示模式")
                    return True
                else:
                    return False
            except KeyboardInterrupt:
                print("\n\n👋 再见!")
                return False
        else:
            print(f"❌ 未找到配置文件 {config_file} 和 {example_file}")
            return False
    
    # 检查配置内容
    try:
        import config
        print(f"✅ 配置文件 {config_file}")
        
        # 检查关键配置
        if hasattr(config, 'AI_ANALYSIS_ENABLED') and config.AI_ANALYSIS_ENABLED:
            if hasattr(config, 'OPENAI_API_KEY') and config.OPENAI_API_KEY != "your_openai_api_key_here":
                print("✅ AI服务已启用，API密钥已配置")
            else:
                print("⚠️  AI服务已启用，但API密钥未配置")
        else:
            print("ℹ️  AI服务未启用，将运行演示模式")
            
    except ImportError as e:
        print(f"❌ 配置文件导入失败: {e}")
        return False
    
    return True

def start_service():
    """启动AI服务"""
    try:
        print("\n🚀 启动AI智能分析服务...")
        print("📍 服务地址: http://localhost:5000")
        print("🌐 前端访问: http://localhost:8888/ai-analysis.html")
        print("⏹️  按 Ctrl+C 停止服务\n")
        
        # 启动Flask应用
        from app import app
        import config
        
        app.run(
            host='0.0.0.0',
            port=getattr(config, 'FLASK_PORT', 5000),
            debug=getattr(config, 'FLASK_DEBUG', True)
        )
        
    except KeyboardInterrupt:
        print("\n\n👋 服务已停止")
    except Exception as e:
        print(f"\n❌ 服务启动失败: {e}")
        return False
    
    return True

def main():
    """主函数"""
    print("🔍 AI智能分析服务启动检查\n")
    
    # 检查Python版本
    if not check_python_version():
        sys.exit(1)
    
    print("\n📦 检查依赖包:")
    if not check_dependencies():
        print("\n💡 安装依赖包:")
        print("pip install -r requirements.txt")
        sys.exit(1)
    
    print("\n⚙️  检查配置文件:")
    if not check_config():
        sys.exit(1)
    
    print("\n" + "="*50)
    print("✅ 环境检查通过，准备启动服务")
    print("="*50)
    
    # 启动服务
    start_service()

if __name__ == '__main__':
    main()
