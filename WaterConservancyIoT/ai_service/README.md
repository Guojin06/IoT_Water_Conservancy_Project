# AI智能分析服务

## 🚀 **快速开始**

### **1. 安装依赖**
```bash
cd ai_service
pip install -r requirements.txt
```

### **2. 配置API密钥**
```bash
# 复制配置文件
cp config_example.py config.py

# 编辑 config.py，填入你的API密钥
# OPENAI_API_KEY = "your_openai_api_key_here"
# AI_ANALYSIS_ENABLED = True
```

### **3. 启动服务**
```bash
python app.py
```

服务将在 `http://localhost:5000` 启动

---

## 🔑 **获取免费API密钥**

### **OpenAI API (推荐)**
1. 访问：https://platform.openai.com/
2. 注册账号并登录
3. 进入 API Keys 页面
4. 创建新的API密钥
5. **新用户有$5免费额度**

### **百度千帆大模型**
1. 访问：https://cloud.baidu.com/product/wenxinworkshop
2. 注册百度云账号
3. 创建应用获取API Key和Secret Key
4. **新用户有免费调用额度**

### **阿里云通义千问**
1. 访问：https://dashscope.aliyun.com/
2. 注册阿里云账号
3. 开通服务获取API Key
4. **有免费试用额度**

---

## 📡 **API接口说明**

### **服务状态**
```http
GET /api/ai/status
```

### **趋势分析**
```http
POST /api/ai/analyze/trend
Content-Type: application/json

{
    "sensor_data": [
        {
            "timestamp": "2024-01-01T12:00:00",
            "water_level": 15.2,
            "temperature": 20.5,
            "sensor_id": "WL001"
        }
    ]
}
```

### **异常检测**
```http
POST /api/ai/detect/anomaly
Content-Type: application/json

{
    "current_data": {
        "water_level": 15.2,
        "temperature": 20.5,
        "timestamp": "2024-01-01T12:00:00"
    },
    "historical_data": [...]
}
```

### **智能问答**
```http
POST /api/ai/qa
Content-Type: application/json

{
    "question": "当前水位是否正常？",
    "context_data": {
        "current_data": {...},
        "recent_data": [...]
    }
}
```

---

## 🎯 **演示模式**

如果没有配置API密钥，服务会运行在演示模式下：

- ✅ 所有接口正常工作
- 🤖 返回模拟的AI分析结果
- 💡 适合开发测试和功能演示
- 🆓 完全免费，无需API密钥

### **演示接口**
```http
GET /api/ai/demo/data      # 获取演示数据
GET /api/ai/demo/analyze   # 获取演示分析结果
```

---

## 🛠️ **开发说明**

### **项目结构**
```
ai_service/
├── app.py              # Flask主应用
├── ai_client.py        # AI客户端封装
├── config_example.py   # 配置文件示例
├── requirements.txt    # Python依赖
└── README.md          # 说明文档
```

### **支持的AI服务商**
- ✅ OpenAI (GPT-3.5-turbo, GPT-4)
- ✅ 百度千帆 (文心一言)
- ✅ 阿里云通义千问
- 🔄 Hugging Face (开发中)

### **扩展功能**
- 🔄 多模型集成
- 🔄 自定义训练模型
- 🔄 边缘计算支持
- 🔄 实时流处理

---

## ⚡ **性能优化**

### **缓存策略**
- Redis缓存AI分析结果
- 相同问题5分钟内直接返回缓存
- 减少API调用成本

### **并发处理**
- 异步处理AI请求
- 支持多个并发分析任务
- 请求队列管理

### **错误处理**
- 自动重试机制
- 降级到本地算法
- 详细错误日志

---

## 💰 **成本控制**

### **免费额度使用建议**
```
OpenAI $5免费额度约可支持：
• 2500次基础分析请求
• 1000次详细报告生成
• 5000次简单问答

建议使用策略：
• 开发测试时使用演示模式
• 生产环境配置真实API
• 启用缓存减少重复调用
```

### **成本监控**
- 实时跟踪API调用次数
- 成本预警机制
- 使用量统计报告

---

## 🔧 **故障排除**

### **常见问题**

**Q: 服务启动失败？**
A: 检查Python版本(>=3.7)和依赖安装

**Q: AI分析返回错误？**
A: 检查API密钥配置和网络连接

**Q: 响应速度慢？**
A: 启用Redis缓存，或使用本地模型

**Q: API额度用完？**
A: 切换到其他服务商或使用演示模式

### **调试模式**
```bash
# 启用详细日志
export FLASK_DEBUG=true
python app.py
```

---

## 🌟 **最佳实践**

1. **开发阶段**：使用演示模式，无需API密钥
2. **测试阶段**：使用少量免费额度验证功能
3. **生产阶段**：选择合适的AI服务商和付费方案
4. **监控运维**：启用缓存和成本监控

---

## 📞 **技术支持**

- 📧 邮箱：support@wateriot.com
- 💬 QQ群：123456789
- 📱 微信：WaterIoT_Support
- 🌐 官网：https://wateriot.com

---

## 📄 **许可证**

本项目采用 MIT 许可证，详见 LICENSE 文件。
