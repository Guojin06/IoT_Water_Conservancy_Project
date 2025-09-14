#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试真实的百度千帆API调用
"""

import os
import sys
sys.path.append('.')

from ai_client import create_ai_client
import config

def test_real_baidu_api():
    """测试真实的百度千帆API"""
    print("🚀 开始测试真实百度千帆API...")
    print(f"🔑 应用ID: {config.BAIDU_APP_ID}")
    print(f"🔑 API Key: {config.BAIDU_API_KEY[:20]}...")
    print(f"🔑 Secret Key: {config.BAIDU_SECRET_KEY[:10]}...")
    print()
    
    # 创建AI客户端
    try:
        ai_client = create_ai_client(
            provider="baidu",
            api_key=config.BAIDU_API_KEY,
            secret_key=config.BAIDU_SECRET_KEY,
            model=config.BAIDU_MODEL,
            app_id=config.BAIDU_APP_ID,
            use_iam=config.BAIDU_USE_IAM
        )
        
        print("✅ AI客户端创建成功")
        
        # 测试API调用
        test_prompt = "请分析当前水库水位2.5米的安全状况，并给出专业建议"
        print(f"📝 测试提示词: {test_prompt}")
        print()
        
        print("🔄 正在调用百度千帆API...")
        response = ai_client.call_api(test_prompt)
        
        if response.success:
            print("✅ API调用成功！")
            print(f"📊 使用Tokens: {response.tokens_used}")
            print(f"💰 成本: ${response.cost:.6f}")
            print()
            print("📄 AI分析结果:")
            print("=" * 60)
            print(response.content)
            print("=" * 60)
        else:
            print("❌ API调用失败！")
            print(f"错误信息: {response.error_message}")
            print()
            print("返回内容:")
            print(response.content)
            
    except Exception as e:
        print(f"❌ 测试失败: {str(e)}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    test_real_baidu_api()
