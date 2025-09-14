
# AI服务配置文件
# 这是一个演示配置，使用免费AI服务商

import os

# 推荐使用免费AI服务商（按优先级排序）

# AI分析总开关
AI_ANALYSIS_ENABLED = True
AI_PROVIDER = "baidu"  # 默认使用百度千帆（免费额度最多）

# 服务商优先级（系统会按顺序尝试）
AI_PROVIDER_PRIORITY = [
    "baidu",      # 百度千帆 - 免费额度最多 1000次/日
    "alibaba",    # 阿里通义 - 免费额度较多 500次/日
    "local",      # 本地模型 - 完全免费但需要部署
    "xunfei",     # 讯飞星火 - 免费额度一般 200次/日
    "openai"      # OpenAI - 成本最高，最后选择
]

# 1. 百度千帆API配置（推荐 - 免费额度1000次/日）
# 新方式：使用应用身份ID + IAM认证
# 申请地址: https://cloud.baidu.com/product/wenxinworkshop

# 百度千帆V2配置（需要IAM Access Key/Secret Key）
BAIDU_APP_ID = "app-4u6aTq3G"  # 您的应用身份ID（用于标识）
BAIDU_API_KEY = ""  # 需要填入IAM Access Key ID
BAIDU_SECRET_KEY = ""  # 需要填入IAM Secret Access Key
BAIDU_MODEL = "ERNIE-Bot-4.0"  # V2推荐模型
BAIDU_USE_IAM = True  # 使用IAM认证
BAIDU_BASE_URL = "https://qianfan.baidubce.com/v2"  # V2 API地址

# 2. 阿里云通义千问API配置（备选 - 免费额度500次/日）
# 申请地址: https://dashscope.aliyuncs.com/
ALIBABA_API_KEY = "demo_alibaba_key"  # 请替换为真实的API密钥
ALIBABA_MODEL = "qwen-turbo"

# 3. 讯飞星火API配置（备选 - 免费额度200次/日）
# 申请地址: https://xinghuo.xfyun.cn/
XUNFEI_APP_ID = "demo_xunfei_app_id"
XUNFEI_API_SECRET = "demo_xunfei_secret"
XUNFEI_API_KEY = "demo_xunfei_key"

# 4. 本地模型配置（完全免费，需要本地部署Ollama）
# 安装指南: https://ollama.ai/
LOCAL_MODEL_API = "http://localhost:11434"  # Ollama默认端口
LOCAL_MODEL_NAME = "llama2-chinese"

# 5. OpenAI API配置（不推荐，成本较高）
OPENAI_API_KEY = "demo_openai_key"
OPENAI_MODEL = "gpt-3.5-turbo"
OPENAI_BASE_URL = "https://api.openai.com/v1"

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
AI_PREDICTION_DAYS = 7  # 预测天数
AI_ANOMALY_THRESHOLD = 0.8  # 异常检测阈值
AI_CACHE_TIMEOUT = 300  # 缓存超时时间（秒）

# 免费额度配置
FREE_QUOTA_DAILY = 1000  # 每日免费调用次数
ENABLE_QUOTA_LIMIT = True  # 启用额度限制

# 使用提示
if BAIDU_API_KEY and BAIDU_SECRET_KEY:
    print("""
🤖 AI服务配置说明：
  
✅ 已配置百度千帆V2 API，使用IAM认证
🔑 应用身份ID: {}
🔑 IAM Access Key: {}...
💡 系统将提供真实的AI分析功能

🏆 当前配置：
  • 百度千帆V2: IAM认证 ✅
  • 应用身份: {}
  • 模型: {}
  • 认证方式: IAM Access/Secret Key
  • 状态: 就绪 🚀
  
📖 详细配置指南请查看: GET_BAIDU_API_KEY.md
""".format(BAIDU_APP_ID, BAIDU_API_KEY[:15], BAIDU_APP_ID, BAIDU_MODEL))
else:
    print("""
🤖 AI服务配置说明：
  
⚠️ 百度千帆V2 API配置不完整
🔑 应用身份ID: {}
❌ 缺少IAM Access Key和Secret Key

📋 配置步骤：
1. 访问百度智能云控制台
2. 进入"访问管理" → "安全认证"
3. 获取Access Key ID和Secret Access Key
4. 更新config.py文件：
   BAIDU_API_KEY = "your_iam_access_key"
   BAIDU_SECRET_KEY = "your_iam_secret_key"

💡 当前将使用演示模式
📖 详细配置指南请查看: GET_BAIDU_API_KEY.md
""".format(BAIDU_APP_ID))
