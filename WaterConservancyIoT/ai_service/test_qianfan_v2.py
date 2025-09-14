#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试百度千帆V2 OpenAI兼容接口
"""

from openai import OpenAI
import requests
import json

def test_connection():
    """测试网络连接"""
    print("🔍 测试网络连接...")
    
    try:
        # 测试基本网络
        response = requests.get("https://www.baidu.com", timeout=5)
        print(f"✅ 百度官网连接正常: {response.status_code}")
        
        # 测试千帆平台
        response = requests.get("https://qianfan.baidubce.com", timeout=10)
        print(f"✅ 千帆平台连接正常: {response.status_code}")
        
        return True
    except Exception as e:
        print(f"❌ 网络连接失败: {e}")
        return False

def test_qianfan_v2_api():
    """测试千帆V2 API"""
    print("\n🚀 测试千帆V2 API...")
    
    app_id = "app-4u6aTq3G"
    base_url = "https://qianfan.baidubce.com/v2"
    
    print(f"🔑 应用身份ID: {app_id}")
    print(f"🔗 API地址: {base_url}")
    
    try:
        # 初始化客户端
        client = OpenAI(
            api_key=app_id,
            base_url=base_url,
            timeout=30.0  # 增加超时时间
        )
        
        print("✅ 客户端初始化成功")
        
        # 发送测试请求
        print("📤 发送测试请求...")
        
        response = client.chat.completions.create(
            model="ERNIE-Bot-4.0",
            messages=[
                {"role": "user", "content": "你好，请简单介绍一下你自己"}
            ],
            timeout=30
        )
        
        print("✅ API调用成功！")
        print(f"📄 响应内容: {response.choices[0].message.content}")
        
        if hasattr(response, 'usage'):
            print(f"📊 Token使用: {response.usage.total_tokens}")
        
        return True
        
    except Exception as e:
        print(f"❌ API调用失败: {e}")
        print(f"错误类型: {type(e).__name__}")
        
        # 详细错误信息
        if hasattr(e, 'response'):
            print(f"HTTP状态码: {e.response.status_code if e.response else 'N/A'}")
            if e.response:
                try:
                    error_data = e.response.json()
                    print(f"错误详情: {json.dumps(error_data, indent=2, ensure_ascii=False)}")
                except:
                    print(f"响应内容: {e.response.text}")
        
        return False

def test_alternative_endpoints():
    """测试其他可能的端点"""
    print("\n🔍 测试其他可能的端点...")
    
    endpoints = [
        "https://qianfan.baidubce.com/v2",
        "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/completions",
        "https://qianfan.baidubce.com/wenxinworkshop/service/chat"
    ]
    
    for endpoint in endpoints:
        print(f"\n测试端点: {endpoint}")
        try:
            response = requests.get(endpoint, timeout=10)
            print(f"✅ 连接成功: {response.status_code}")
            if response.headers.get('content-type', '').startswith('application/json'):
                try:
                    data = response.json()
                    print(f"响应数据: {json.dumps(data, indent=2, ensure_ascii=False)[:200]}...")
                except:
                    pass
        except Exception as e:
            print(f"❌ 连接失败: {e}")

if __name__ == "__main__":
    print("🔧 百度千帆V2 API 诊断工具")
    print("=" * 50)
    
    # 测试网络连接
    if not test_connection():
        print("\n❌ 网络连接有问题，请检查网络设置")
        exit(1)
    
    # 测试千帆V2 API
    if test_qianfan_v2_api():
        print("\n🎉 千帆V2 API测试成功！")
    else:
        print("\n🔍 API测试失败，尝试其他端点...")
        test_alternative_endpoints()
        
        print("""
🔧 故障排除建议：

1. 检查应用身份ID是否正确
2. 确认应用已在百度云控制台激活
3. 检查网络是否需要代理设置
4. 尝试使用VPN或其他网络环境
5. 联系百度千帆技术支持

📞 百度千帆技术支持：
   https://cloud.baidu.com/doc/WENXINWORKSHOP/
""")
