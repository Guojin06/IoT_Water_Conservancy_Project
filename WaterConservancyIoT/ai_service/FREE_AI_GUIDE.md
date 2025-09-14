# 🤖 免费AI接口使用指南

## 📋 概述

本指南帮助您配置和使用免费的AI接口，替代昂贵的OpenAI API。我们支持多个免费AI服务商，按免费额度从高到低推荐。

## 🏆 推荐服务商排序

### 1. 🥇 百度千帆（推荐）
- **免费额度**: 1000次/日
- **模型**: 文心一言 ERNIE-3.5-8K
- **优势**: 免费额度最多，中文支持好
- **申请地址**: https://cloud.baidu.com/product/wenxinworkshop

**配置步骤**:
1. 注册百度云账号
2. 开通千帆大模型平台
3. 创建应用获取 API Key 和 Secret Key
4. 在 `config.py` 中配置:
```python
AI_PROVIDER = "baidu"
BAIDU_API_KEY = "你的API密钥"
BAIDU_SECRET_KEY = "你的Secret密钥"
```

### 2. 🥈 阿里云通义千问
- **免费额度**: 500次/日
- **模型**: qwen-turbo
- **优势**: 阿里云生态，稳定性好
- **申请地址**: https://dashscope.aliyuncs.com/

**配置步骤**:
1. 注册阿里云账号
2. 开通DashScope服务
3. 获取API Key
4. 在 `config.py` 中配置:
```python
AI_PROVIDER = "alibaba"
ALIBABA_API_KEY = "你的API密钥"
```

### 3. 🥉 讯飞星火
- **免费额度**: 200次/日
- **模型**: Spark-3.5
- **优势**: 语音识别技术强
- **申请地址**: https://xinghuo.xfyun.cn/

**配置步骤**:
1. 注册讯飞开放平台账号
2. 创建星火认知大模型应用
3. 获取 APPID、APISecret、APIKey
4. 在 `config.py` 中配置:
```python
AI_PROVIDER = "xunfei"
XUNFEI_APP_ID = "你的APPID"
XUNFEI_API_KEY = "你的APIKey"
XUNFEI_API_SECRET = "你的APISecret"
```

### 4. 🏠 本地模型（完全免费）
- **免费额度**: 无限制
- **模型**: Llama2-Chinese, Qwen等
- **优势**: 完全免费，数据隐私
- **要求**: 需要本地部署

**配置步骤**:
1. 安装 Ollama: https://ollama.ai/
2. 下载中文模型:
```bash
ollama pull llama2-chinese
# 或
ollama pull qwen:7b
```
3. 在 `config.py` 中配置:
```python
AI_PROVIDER = "local"
LOCAL_MODEL_API = "http://localhost:11434"
LOCAL_MODEL_NAME = "llama2-chinese"
```

## ⚙️ 配置文件设置

### 完整配置示例 (config.py)
```python
# AI服务配置
AI_ANALYSIS_ENABLED = True
AI_PROVIDER = "baidu"  # 默认使用百度千帆

# 服务商优先级（按免费额度排序）
AI_PROVIDER_PRIORITY = [
    "baidu",      # 百度千帆 - 1000次/日
    "alibaba",    # 阿里通义 - 500次/日
    "xunfei",     # 讯飞星火 - 200次/日
    "local",      # 本地模型 - 无限制
    "openai"      # OpenAI - 付费
]

# 百度千帆配置
BAIDU_API_KEY = "你的百度API密钥"
BAIDU_SECRET_KEY = "你的百度Secret密钥"
BAIDU_MODEL = "ERNIE-3.5-8K"

# 阿里云通义配置
ALIBABA_API_KEY = "你的阿里云API密钥"
ALIBABA_MODEL = "qwen-turbo"

# 讯飞星火配置
XUNFEI_APP_ID = "你的讯飞APPID"
XUNFEI_API_KEY = "你的讯飞APIKey"
XUNFEI_API_SECRET = "你的讯飞APISecret"

# 本地模型配置
LOCAL_MODEL_API = "http://localhost:11434"
LOCAL_MODEL_NAME = "llama2-chinese"

# 免费额度管理
FREE_QUOTA_DAILY = 1000
ENABLE_QUOTA_LIMIT = True
```

## 🚀 快速开始

### 1. 复制配置文件
```bash
cd WaterConservancyIoT/ai_service
cp config_example.py config.py
```

### 2. 编辑配置文件
根据上面的指南选择一个服务商，填入对应的API密钥。

### 3. 启动服务
```bash
python app.py
```

### 4. 测试AI功能
访问 http://localhost:5000/api/ai/status 查看服务状态。

## 💰 成本对比

| 服务商 | 免费额度 | 超出后价格 | 推荐指数 |
|--------|----------|------------|----------|
| 百度千帆 | 1000次/日 | ¥0.008/1k tokens | ⭐⭐⭐⭐⭐ |
| 阿里通义 | 500次/日 | ¥0.014/1k tokens | ⭐⭐⭐⭐ |
| 讯飞星火 | 200次/日 | ¥0.018/1k tokens | ⭐⭐⭐ |
| 本地模型 | 无限制 | 免费 | ⭐⭐⭐⭐⭐ |
| OpenAI | 0次 | $0.002/1k tokens | ⭐⭐ |

## 🔧 故障排除

### 常见问题

**Q: AI服务启动失败怎么办？**
A: 检查API密钥是否正确，网络是否正常。系统会自动尝试备选服务商。

**Q: 本地模型如何部署？**
A: 
1. 安装Docker
2. 运行: `docker run -d -v ollama:/root/.ollama -p 11434:11434 --name ollama ollama/ollama`
3. 下载模型: `docker exec -it ollama ollama pull llama2-chinese`

**Q: 免费额度用完了怎么办？**
A: 可以注册多个账号轮换使用，或者切换到本地模型。

**Q: 哪个服务商响应最快？**
A: 通常本地模型最快，云服务中百度千帆响应较快。

## 📈 使用统计

系统会自动记录API调用次数，避免超出免费额度。可以通过以下接口查看:
- `/api/ai/status` - 当前服务商状态
- `/api/ai/providers` - 所有支持的服务商

## 🛡️ 最佳实践

1. **多服务商备份**: 配置多个服务商，避免单点故障
2. **额度监控**: 开启额度限制，避免意外超出
3. **本地优先**: 有条件建议部署本地模型
4. **缓存结果**: 相同问题缓存结果，减少API调用
5. **错误重试**: 网络错误时自动重试和切换服务商

## 🔄 自动切换机制

系统支持智能切换AI服务商：
1. 优先使用免费额度多的服务商
2. 服务异常时自动切换到备选服务商
3. 本地模型作为最终备选
4. 所有服务商都不可用时提供演示功能

---

💡 **小贴士**: 建议同时配置2-3个服务商，确保服务的高可用性！
