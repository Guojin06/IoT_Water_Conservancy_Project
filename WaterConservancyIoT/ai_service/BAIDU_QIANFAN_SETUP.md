# 🚀 百度千帆API配置指南

## 📋 当前问题
您提供的是百度云Access Key，但千帆API需要专用的应用API Key。

## 🔧 获取百度千帆API Key步骤

### 1. 访问千帆控制台
打开：https://console.bce.baidu.com/qianfan/ais/console/applicationConsole/application

### 2. 创建应用
1. 点击"创建应用"
2. 填写应用名称（如：水利监控AI）
3. 选择应用类型：API调用
4. 提交创建

### 3. 获取API密钥
创建成功后，您会看到：
- **API Key**: 以 `sk-` 开头的密钥
- **Secret Key**: 对应的密钥

### 4. 更新配置
将获得的密钥更新到 `config.py`：
```python
BAIDU_API_KEY = "sk-xxxxxxxxxx"  # 您的API Key
BAIDU_SECRET_KEY = "xxxxxxxxxx"   # 您的Secret Key
```

## 💰 费用说明
- 新用户有免费额度
- ERNIE-3.5模型：前1000次免费
- 超出后按量计费，价格很低

## ✅ 配置完成后
重启AI服务即可使用真实的百度千帆AI！
