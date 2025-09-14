# AI服务配置文件示例
# 复制此文件为 config.py 并填入真实的API密钥

import os

# 推荐使用免费AI服务商（按优先级排序）

# 1. 百度千帆API配置（推荐 - 免费额度较多）
BAIDU_API_KEY = "your_baidu_api_key_here"
BAIDU_SECRET_KEY = "your_baidu_secret_key_here"
BAIDU_MODEL = "ERNIE-3.5-8K"  # 文心一言3.5

# 2. 阿里云通义千问API配置（备选 - 也有免费额度）
ALIBABA_API_KEY = "your_alibaba_api_key_here"
ALIBABA_MODEL = "qwen-turbo"

# 3. 讯飞星火API配置（备选 - 免费额度）
XUNFEI_APP_ID = "your_xunfei_app_id_here"
XUNFEI_API_SECRET = "your_xunfei_api_secret_here"
XUNFEI_API_KEY = "your_xunfei_api_key_here"

# 4. OpenAI API配置（成本较高，不推荐）
OPENAI_API_KEY = "your_openai_api_key_here"
OPENAI_MODEL = "gpt-3.5-turbo"
OPENAI_BASE_URL = "https://api.openai.com/v1"

# 5. 本地模型配置（完全免费，需要本地部署）
LOCAL_MODEL_API = "http://localhost:11434"  # Ollama默认端口
LOCAL_MODEL_NAME = "llama2-chinese"

# 数据库配置
REDIS_HOST = "localhost"
REDIS_PORT = 6379
REDIS_DB = 0

MYSQL_HOST = "localhost"
MYSQL_PORT = 3306
MYSQL_USER = "root"
MYSQL_PASSWORD = "password"
MYSQL_DATABASE = "water_iot"

# 服务配置
FLASK_PORT = 5000
FLASK_DEBUG = True

# AI分析配置
AI_ANALYSIS_ENABLED = True
AI_PROVIDER = "baidu"  # 默认使用百度千帆（免费额度多）
AI_PREDICTION_DAYS = 7  # 预测天数
AI_ANOMALY_THRESHOLD = 0.8  # 异常检测阈值
AI_CACHE_TIMEOUT = 300  # 缓存超时时间（秒）

# 免费额度配置
FREE_QUOTA_DAILY = 1000  # 每日免费调用次数
ENABLE_QUOTA_LIMIT = True  # 启用额度限制

# 备选AI服务商优先级
AI_PROVIDER_PRIORITY = [
    "baidu",      # 百度千帆 - 免费额度最多
    "alibaba",    # 阿里通义 - 免费额度较多
    "xunfei",     # 讯飞星火 - 免费额度一般
    "local",      # 本地模型 - 完全免费但需要部署
    "openai"      # OpenAI - 成本最高，最后选择
]
