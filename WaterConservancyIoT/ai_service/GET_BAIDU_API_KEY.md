# 🔑 获取百度千帆IAM认证密钥详细指南

## 📋 正确的配置方式

根据官方文档，百度千帆V2需要**IAM Access Key和Secret Key**，而不是应用API Key。

## 🔧 获取步骤

### 1. 访问百度智能云控制台
```
https://console.bce.baidu.com/
```

### 2. 进入访问管理
- 点击右上角用户名
- 选择 **"访问管理"**
- 进入 **"安全认证"** 页面

### 3. 获取IAM密钥
在安全认证页面：
- 找到 **"Access Key ID"** 和 **"Secret Access Key"**
- 如果没有，点击 **"创建Access Key"**
- 获取到的格式类似：
  - **Access Key ID**: `LTAI5t...` 或类似格式
  - **Secret Access Key**: `长字符串密钥`

### 4. 更新配置文件

将获取到的IAM密钥更新到 `config.py`：

```python
# 百度千帆V2配置（IAM认证）
BAIDU_APP_ID = "app-4u6aTq3G"              # 您的应用身份ID
BAIDU_API_KEY = "your_iam_access_key"      # IAM Access Key ID
BAIDU_SECRET_KEY = "your_iam_secret_key"   # IAM Secret Access Key
BAIDU_MODEL = "ERNIE-Bot-4.0"
BAIDU_USE_IAM = True                       # 使用IAM认证
```

## ⚡ 快速解决方案

如果您暂时无法获取应用API Key，我们可以：

1. **临时使用演示模式** - 当前系统已经很完善了
2. **稍后配置真实API** - 不影响项目其他部分的完成
3. **使用其他AI服务** - 如阿里云通义千问（配置更简单）

## 🔧 测试验证

获取正确API Key后：
```bash
python test_real_api.py
```

应该看到：
```
✅ API调用成功！
📊 使用Tokens: xxx
💰 成本: $0.00xxxx
📄 AI分析结果: [真实的AI回复]
```

## 📞 需要帮助？

如果遇到问题，请提供：
1. 您的应用API Key前缀（如：sk-abc123...）
2. 错误信息截图
3. 是否成功创建了千帆应用
