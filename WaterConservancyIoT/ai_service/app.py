#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
AI智能分析服务
Flask API服务器
"""

import json
import time
from datetime import datetime, timedelta
from flask import Flask, request, jsonify
from flask_cors import CORS
import logging
import traceback
from typing import Dict, List, Any

# 导入自定义模块
from ai_client import AIClient, WaterAIAnalyzer, create_ai_client, AIProvider, get_recommended_provider

# 尝试导入配置文件
try:
    import config
    print("✅ 成功加载配置文件 config.py")
except ImportError:
    print("⚠️  未找到 config.py，使用默认配置")
    print("💡 请复制 config_example.py 为 config.py 并填入真实的API密钥")
    
    # 默认配置
    class config:
        # 推荐使用免费AI服务商
        AI_PROVIDER = "baidu"  # 默认使用百度千帆
        BAIDU_API_KEY = "your_baidu_api_key_here"
        BAIDU_SECRET_KEY = "your_baidu_secret_key_here"
        BAIDU_MODEL = "ERNIE-3.5-8K"
        
        # 备选配置
        ALIBABA_API_KEY = "your_alibaba_api_key_here"
        ALIBABA_MODEL = "qwen-turbo"
        
        # OpenAI配置（不推荐，成本高）
        OPENAI_API_KEY = "your_openai_api_key_here"
        OPENAI_MODEL = "gpt-3.5-turbo"
        OPENAI_BASE_URL = "https://api.openai.com/v1"
        
        FLASK_PORT = 5000
        FLASK_DEBUG = True
        AI_ANALYSIS_ENABLED = False  # 默认关闭AI分析

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# 创建Flask应用
app = Flask(__name__)
CORS(app)  # 允许跨域请求

# 全局变量
ai_client = None
ai_analyzer = None

def init_ai_service():
    """初始化AI服务 - 自动选择可用的免费AI服务商"""
    global ai_client, ai_analyzer
    
    try:
        if not hasattr(config, 'AI_ANALYSIS_ENABLED') or not config.AI_ANALYSIS_ENABLED:
            logger.warning("AI分析功能已禁用")
            return False
        
        # 获取AI服务商优先级列表
        provider_priority = getattr(config, 'AI_PROVIDER_PRIORITY', ['baidu', 'alibaba', 'local', 'openai'])
        
        # 尝试按优先级初始化AI服务
        for provider in provider_priority:
            try:
                success = _try_init_provider(provider)
                if success:
                    logger.info(f"✅ AI服务初始化成功，使用服务商: {provider}")
                    return True
            except Exception as e:
                logger.warning(f"⚠️ {provider} 服务商初始化失败: {str(e)}")
                continue
        
        logger.warning("⚠️ 所有AI服务商都无法使用，将提供演示功能")
        return False
        
    except Exception as e:
        logger.error(f"❌ AI服务初始化失败: {str(e)}")
        return False

def _try_init_provider(provider: str) -> bool:
    """尝试初始化指定的AI服务商"""
    global ai_client, ai_analyzer
    
    if provider == 'baidu':
        if not _check_baidu_config():
            return False
        ai_client = create_ai_client(
            provider="baidu",
            api_key=config.BAIDU_API_KEY,
            secret_key=config.BAIDU_SECRET_KEY,
            model=config.BAIDU_MODEL,
            app_id=getattr(config, 'BAIDU_APP_ID', ''),
            use_iam=getattr(config, 'BAIDU_USE_IAM', False),
            base_url=getattr(config, 'BAIDU_BASE_URL', '')
        )
        
    elif provider == 'alibaba':
        if not _check_alibaba_config():
            return False
        ai_client = create_ai_client(
            provider="alibaba",
            api_key=config.ALIBABA_API_KEY,
            model=config.ALIBABA_MODEL
        )
        
    elif provider == 'local':
        ai_client = create_ai_client(provider="local")
        
    elif provider == 'openai':
        if not _check_openai_config():
            return False
        ai_client = create_ai_client(
            provider="openai",
            api_key=config.OPENAI_API_KEY,
            base_url=config.OPENAI_BASE_URL,
            model=config.OPENAI_MODEL
        )
    else:
        return False
    
    # 创建AI分析器
    ai_analyzer = WaterAIAnalyzer(ai_client)
    return True

def _check_baidu_config() -> bool:
    """检查百度API配置"""
    return (hasattr(config, 'BAIDU_API_KEY') and 
            config.BAIDU_API_KEY != "your_baidu_api_key_here" and
            hasattr(config, 'BAIDU_SECRET_KEY') and
            config.BAIDU_SECRET_KEY != "your_baidu_secret_key_here")

def _check_alibaba_config() -> bool:
    """检查阿里云API配置"""
    return (hasattr(config, 'ALIBABA_API_KEY') and 
            config.ALIBABA_API_KEY != "your_alibaba_api_key_here")

def _check_openai_config() -> bool:
    """检查OpenAI API配置"""
    return (hasattr(config, 'OPENAI_API_KEY') and 
            config.OPENAI_API_KEY != "your_openai_api_key_here")

@app.route('/')
def index():
    """首页"""
    return jsonify({
        'service': 'AI智能分析服务',
        'version': '1.0.0',
        'status': 'running',
        'ai_enabled': ai_analyzer is not None,
        'endpoints': [
            '/api/ai/analyze/trend',
            '/api/ai/detect/anomaly',
            '/api/ai/generate/report',
            '/api/ai/qa',
            '/api/ai/status'
        ],
        'timestamp': datetime.now().isoformat()
    })

@app.route('/api/ai/status')
def ai_status():
    """AI服务状态"""
    current_provider = None
    current_model = None
    
    if ai_client:
        current_provider = ai_client.provider.value
        current_model = ai_client.model
    
    return jsonify({
        'ai_enabled': ai_analyzer is not None,
        'provider': current_provider,
        'model': current_model,
        'status': 'ready' if ai_analyzer else 'not_configured',
        'recommended_providers': get_recommended_provider()
    })

@app.route('/api/ai/providers')
def list_providers():
    """获取支持的AI服务商列表"""
    return jsonify({
        'success': True,
        'providers': get_recommended_provider(),
        'current_provider': ai_client.provider.value if ai_client else None,
        'message': '推荐使用免费额度较多的服务商'
    })

@app.route('/api/ai/analyze/trend', methods=['POST'])
def analyze_trend():
    """水位趋势分析"""
    try:
        if not ai_analyzer:
            return jsonify({
                'success': False,
                'error': 'AI服务未启用或配置错误',
                'message': '请检查API密钥配置'
            }), 503
            
        data = request.get_json()
        if not data or 'sensor_data' not in data:
            return jsonify({
                'success': False,
                'error': '缺少必要的传感器数据'
            }), 400
        
        sensor_data = data['sensor_data']
        
        # 调用AI分析
        result = ai_analyzer.analyze_water_level_trend(sensor_data)
        
        return jsonify({
            'success': result.success,
            'analysis': result.content if result.success else None,
            'provider': result.provider,
            'cost': result.cost,
            'tokens_used': result.tokens_used,
            'response_time': result.response_time,
            'error': result.error_message if not result.success else None,
            'timestamp': datetime.now().isoformat()
        })
        
    except Exception as e:
        logger.error(f"趋势分析错误: {str(e)}")
        return jsonify({
            'success': False,
            'error': '服务内部错误',
            'message': str(e)
        }), 500

@app.route('/api/ai/detect/anomaly', methods=['POST'])
def detect_anomaly():
    """异常检测分析"""
    try:
        if not ai_analyzer:
            return jsonify({
                'success': False,
                'error': 'AI服务未启用或配置错误'
            }), 503
            
        data = request.get_json()
        if not data or 'current_data' not in data:
            return jsonify({
                'success': False,
                'error': '缺少当前数据'
            }), 400
        
        current_data = data['current_data']
        historical_data = data.get('historical_data', [])
        
        # 调用AI异常检测
        result = ai_analyzer.detect_anomaly(current_data, historical_data)
        
        return jsonify({
            'success': result.success,
            'analysis': result.content if result.success else None,
            'provider': result.provider,
            'cost': result.cost,
            'tokens_used': result.tokens_used,
            'response_time': result.response_time,
            'error': result.error_message if not result.success else None,
            'timestamp': datetime.now().isoformat()
        })
        
    except Exception as e:
        logger.error(f"异常检测错误: {str(e)}")
        return jsonify({
            'success': False,
            'error': '服务内部错误',
            'message': str(e)
        }), 500

@app.route('/api/ai/generate/report', methods=['POST'])
def generate_report():
    """生成智能报告"""
    try:
        if not ai_analyzer:
            return jsonify({
                'success': False,
                'error': 'AI服务未启用或配置错误'
            }), 503
            
        data = request.get_json()
        if not data or 'data_summary' not in data:
            return jsonify({
                'success': False,
                'error': '缺少数据摘要'
            }), 400
        
        data_summary = data['data_summary']
        
        # 调用AI报告生成
        result = ai_analyzer.generate_report(data_summary)
        
        return jsonify({
            'success': result.success,
            'report': result.content if result.success else None,
            'provider': result.provider,
            'cost': result.cost,
            'tokens_used': result.tokens_used,
            'response_time': result.response_time,
            'error': result.error_message if not result.success else None,
            'timestamp': datetime.now().isoformat()
        })
        
    except Exception as e:
        logger.error(f"报告生成错误: {str(e)}")
        return jsonify({
            'success': False,
            'error': '服务内部错误',
            'message': str(e)
        }), 500

@app.route('/api/ai/qa', methods=['POST'])
def question_answer():
    """智能问答"""
    try:
        if not ai_analyzer:
            return jsonify({
                'success': False,
                'error': 'AI服务未启用或配置错误'
            }), 503
            
        data = request.get_json()
        if not data or 'question' not in data:
            return jsonify({
                'success': False,
                'error': '缺少问题内容'
            }), 400
        
        question = data['question']
        context_data = data.get('context_data', {})
        
        # 调用AI问答
        result = ai_analyzer.answer_question(question, context_data)
        
        return jsonify({
            'success': result.success,
            'answer': result.content if result.success else None,
            'question': question,
            'provider': result.provider,
            'cost': result.cost,
            'tokens_used': result.tokens_used,
            'response_time': result.response_time,
            'error': result.error_message if not result.success else None,
            'timestamp': datetime.now().isoformat()
        })
        
    except Exception as e:
        logger.error(f"智能问答错误: {str(e)}")
        return jsonify({
            'success': False,
            'error': '服务内部错误',
            'message': str(e)
        }), 500

@app.route('/api/ai/demo/data', methods=['GET'])
def get_demo_data():
    """获取演示数据"""
    # 生成模拟的传感器数据
    now = datetime.now()
    demo_data = []
    
    for i in range(24):  # 24小时数据
        timestamp = now - timedelta(hours=i)
        demo_data.append({
            'timestamp': timestamp.isoformat(),
            'sensor_id': 'WL001',
            'water_level': 15.2 + (i * 0.1) + ((-1) ** i * 0.3),  # 模拟波动
            'temperature': 20.5 + (i * 0.05),
            'flow_rate': 120.0 + (i * 2),
            'quality_score': 0.95 - (i * 0.001)
        })
    
    return jsonify({
        'success': True,
        'data': demo_data,
        'count': len(demo_data),
        'description': '24小时模拟传感器数据'
    })

@app.route('/api/ai/demo/analyze', methods=['GET'])
def demo_analyze():
    """演示AI分析功能（不需要真实API密钥）"""
    # 模拟AI分析结果
    mock_analysis = {
        'trend_analysis': {
            'trend': '上升',
            'confidence': 0.85,
            'prediction': '未来3天水位将继续上升0.5-0.8米',
            'risk_level': '中等',
            'recommendations': [
                '建议加强监控频率',
                '准备应急预案',
                '检查排水设施'
            ]
        },
        'anomaly_detection': {
            'status': '正常',
            'anomaly_score': 0.15,
            'last_anomaly': '2天前检测到轻微异常',
            'details': '所有指标均在正常范围内'
        }
    }
    
    return jsonify({
        'success': True,
        'demo': True,
        'analysis': mock_analysis,
        'message': '这是演示数据，实际使用需要配置AI API密钥',
        'timestamp': datetime.now().isoformat()
    })

@app.errorhandler(404)
def not_found(error):
    return jsonify({
        'error': 'API接口不存在',
        'message': '请检查请求路径是否正确'
    }), 404

@app.errorhandler(500)
def internal_error(error):
    return jsonify({
        'error': '服务器内部错误',
        'message': '请联系管理员'
    }), 500

if __name__ == '__main__':
    print("🚀 启动AI智能分析服务...")
    print(f"📍 服务地址: http://localhost:{config.FLASK_PORT}")
    print("📋 可用接口:")
    print("  - GET  /                    # 服务状态")
    print("  - GET  /api/ai/status       # AI服务状态")
    print("  - GET  /api/ai/providers    # 支持的AI服务商")
    print("  - POST /api/ai/analyze/trend # 趋势分析")
    print("  - POST /api/ai/detect/anomaly # 异常检测")
    print("  - POST /api/ai/generate/report # 报告生成")
    print("  - POST /api/ai/qa           # 智能问答")
    print("  - GET  /api/ai/demo/data    # 演示数据")
    print("  - GET  /api/ai/demo/analyze # 演示分析")
    print()
    
    # 显示免费AI服务商推荐
    print("💡 推荐使用免费AI服务商（按免费额度排序）:")
    for provider, description in get_recommended_provider():
        print(f"   {provider}: {description}")
    print()
    
    # 初始化AI服务
    ai_initialized = init_ai_service()
    if ai_initialized:
        current_provider = ai_client.provider.value if ai_client else "未知"
        print(f"✅ AI服务已启用，当前使用: {current_provider}")
    else:
        print("⚠️  AI服务未启用，仅提供演示功能")
        print("💡 要启用AI功能，请:")
        print("   1. 复制 config_example.py 为 config.py")
        print("   2. 选择一个免费AI服务商并填入API密钥:")
        print("      - 百度千帆: https://cloud.baidu.com/product/wenxinworkshop")
        print("      - 阿里通义: https://dashscope.aliyuncs.com/")
        print("      - 本地模型: 安装Ollama (https://ollama.ai/)")
        print("   3. 设置 AI_ANALYSIS_ENABLED = True")
    
    print()
    print("🌟 服务启动中...")
    
    # 启动Flask应用
    app.run(
        host='0.0.0.0',
        port=config.FLASK_PORT,
        debug=config.FLASK_DEBUG
    )
