# 🤖 百度千帆API配置指南

## 📋 您当前的配置状态

✅ **API Key**: 已配置  
❌ **Secret Key**: 需要获取  

## 🔧 完整配置步骤

### 第1步：获取Secret Key

您已经有了API Key，现在需要获取对应的Secret Key：

1. 访问百度云控制台：https://console.bce.baidu.com/
2. 点击右上角用户名 → **安全认证**
3. 在"Access Key管理"页面找到您的密钥对
4. 复制 **Secret Access Key**（注意：这个只在创建时显示一次）

### 第2步：配置到项目

在 `config.py` 文件中更新：

```python
# 百度千帆API配置
BAIDU_API_KEY = "bce-v3/ALTAK-8Z1IPcYKaRKsAojjKKaFN/7075e5e44466d74152cd4b56491705edacb60cc1"  # 已配置
BAIDU_SECRET_KEY = "您的Secret密钥"  # 请替换这里
```

### 第3步：启用千帆服务

1. 访问千帆大模型平台：https://cloud.baidu.com/product/wenxinworkshop
2. 点击"立即使用"
3. 开通"文心一言"服务
4. 确认有1000次/日的免费额度

## 🚀 测试配置

配置完成后，运行以下命令测试：

### Windows (PowerShell):
```powershell
cd WaterConservancyIoT\ai_service
python app.py
```

### 或者使用我们提供的启动脚本：
```powershell
cd WaterConservancyIoT\ai_service
.\start_ai_service.bat
```

## 🔍 如果找不到Secret Key

如果您找不到Secret Key，可以创建新的密钥对：

1. 在百度云控制台的"Access Key管理"页面
2. 点击"创建Access Key"
3. 记录新的API Key和Secret Key
4. 更新config.py中的配置

## ✅ 配置成功标志

启动服务后，您应该看到：
```
✅ AI服务已启用，当前使用: baidu
```

## 🆘 常见问题

**Q: 提示"未开通千帆服务"**  
A: 需要在百度云控制台开通千帆大模型平台服务

**Q: 提示"配额不足"**  
A: 检查是否已开通免费额度，或者额度已用完

**Q: API调用失败**  
A: 检查API Key和Secret Key是否正确配置

---

💡 **小贴士**: 百度千帆每日有1000次免费调用额度，足够开发和测试使用！
