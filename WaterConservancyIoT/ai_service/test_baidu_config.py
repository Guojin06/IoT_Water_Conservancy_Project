#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
百度千帆API配置测试脚本
"""

import sys
import os

def test_config():
    """测试配置文件"""
    try:
        import config
        print("✅ 成功加载配置文件")
        
        # 检查百度配置
        api_key = getattr(config, 'BAIDU_API_KEY', '')
        secret_key = getattr(config, 'BAIDU_SECRET_KEY', '')
        
        print(f"📋 当前配置状态:")
        print(f"   API Key: {'✅ 已配置' if api_key and api_key != 'demo_baidu_key' else '❌ 未配置'}")
        print(f"   Secret Key: {'✅ 已配置' if secret_key and secret_key != '请添加您的Secret密钥' else '❌ 需要配置'}")
        
        if api_key and api_key != 'demo_baidu_key':
            print(f"   API Key预览: {api_key[:20]}...")
        
        if secret_key == '请添加您的Secret密钥':
            print()
            print("💡 下一步：请获取百度云的Secret Key")
            print("   1. 访问: https://console.bce.baidu.com/")
            print("   2. 用户名 → 安全认证 → Access Key管理")
            print("   3. 复制Secret Access Key")
            print("   4. 更新config.py中的BAIDU_SECRET_KEY")
            return False
        
        return True
        
    except ImportError:
        print("❌ 配置文件不存在")
        print("💡 请确保config.py文件存在")
        return False
    except Exception as e:
        print(f"❌ 配置检查失败: {e}")
        return False

def test_ai_client():
    """测试AI客户端"""
    try:
        from ai_client import create_ai_client, AIProvider
        print("✅ AI客户端模块加载成功")
        
        # 尝试创建百度客户端
        import config
        if hasattr(config, 'BAIDU_SECRET_KEY') and config.BAIDU_SECRET_KEY != '请添加您的Secret密钥':
            client = create_ai_client(
                provider="baidu",
                api_key=config.BAIDU_API_KEY,
                secret_key=config.BAIDU_SECRET_KEY
            )
            print("✅ 百度千帆客户端创建成功")
            return True
        else:
            print("⚠️ 百度千帆配置不完整，无法测试连接")
            return False
            
    except Exception as e:
        print(f"❌ AI客户端测试失败: {e}")
        return False

def main():
    """主测试函数"""
    print("🚀 百度千帆API配置测试")
    print("=" * 50)
    
    # 测试配置
    config_ok = test_config()
    print()
    
    if config_ok:
        # 测试AI客户端
        client_ok = test_ai_client()
        print()
        
        if client_ok:
            print("🎉 配置完成！可以启动AI服务了")
            print("💡 运行命令: python app.py")
        else:
            print("⚠️ 配置不完整，请完成Secret Key配置")
    else:
        print("❌ 请先完成基本配置")
    
    print("=" * 50)

if __name__ == "__main__":
    main()
