# 智能水利系统部署指南

## 🚀 快速部署

### 系统要求
- **操作系统**: Windows 10/11, Linux, macOS
- **Python**: 3.9+ 
- **数据库**: MySQL 8.0+
- **缓存**: Redis 6.0+
- **浏览器**: Chrome, Firefox, Safari (支持WebSocket)

### 自动部署 (推荐)

#### Windows
```powershell
# 1. 克隆项目
git clone https://github.com/Guojin06/IoT_Water_Conservancy_Project.git
cd IoT_Water_Conservancy_Project

# 2. 运行一键部署脚本
.\deploy.ps1
```

#### Linux/macOS  
```bash
# 1. 克隆项目
git clone https://github.com/Guojin06/IoT_Water_Conservancy_Project.git
cd IoT_Water_Conservancy_Project

# 2. 运行一键部署脚本
chmod +x deploy.sh
./deploy.sh
```

## 🔧 手动部署

### 1. 环境准备

#### 安装依赖
```bash
# 创建虚拟环境
python -m venv ai_env

# 激活虚拟环境 (Windows)
ai_env\Scripts\activate

# 激活虚拟环境 (Linux/macOS)
source ai_env/bin/activate

# 安装Python依赖
pip install -r requirements.txt
```

#### 数据库设置
```bash
# 启动MySQL服务
# Windows: net start mysql
# Linux: sudo systemctl start mysql
# macOS: brew services start mysql

# 创建数据库
mysql -u root -p < WaterConservancyIoT/create_database.sql
```

#### Redis设置
```bash
# 启动Redis服务
# Windows: 下载Redis for Windows并启动
# Linux: sudo systemctl start redis
# macOS: brew services start redis
```

### 2. 配置系统

#### 数据库连接配置
编辑 `WaterConservancyIoT/config.py`:
```python
# 数据库配置
MYSQL_CONFIG = {
    'host': 'localhost',
    'database': 'sensordatabase', 
    'user': 'root',
    'password': '你的MySQL密码'
}

# Redis配置
REDIS_CONFIG = {
    'host': 'localhost',
    'port': 6379,
    'decode_responses': True
}
```

### 3. 启动服务

#### 生产环境启动
```bash
cd WaterConservancyIoT

# 启动所有服务
python start_production.py
```

#### 开发环境启动
```bash
# 方式1: 使用启动脚本 (Windows)
.\start_services.ps1

# 方式2: 手动启动各服务
# 终端1: 数据生产者
python data_producer.py

# 终端2: WebSocket服务器  
python working_websocket.py

# 终端3: HTTP服务器
python simple_server.py
```

## 🌐 访问系统

启动成功后，在浏览器中访问:

- **主界面**: http://localhost:8888
- **系统管理**: http://localhost:8888/system-management.html
- **AI分析**: http://localhost:8888/ai-analysis.html

**默认登录账户**:
- 用户名: `admin`
- 密码: `admin123`

## 📊 系统架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   数据生产者    │───▶│      Redis      │───▶│  WebSocket服务  │
│ data_producer   │    │    (实时缓存)   │    │ working_websocket│
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                                                   │
         ▼                                                   ▼
┌─────────────────┐                               ┌─────────────────┐
│      MySQL      │                               │   前端界面      │
│   (持久化存储)  │                               │ (Vue.js + 图表)  │
└─────────────────┘                               └─────────────────┘
         ▲                                                   ▲
         │                                                   │
┌─────────────────┐                               ┌─────────────────┐
│   HTTP服务器    │◀──────────────────────────────│   用户浏览器    │
│  simple_server  │        RESTful API            │                 │
└─────────────────┘                               └─────────────────┘
```

## 🔒 安全配置

### 生产环境安全检查
- [ ] 修改默认数据库密码
- [ ] 更换JWT密钥 (`simple_server.py` 中的 `JWT_SECRET_KEY`)
- [ ] 配置防火墙规则
- [ ] 启用HTTPS (如需要)
- [ ] 定期备份数据库

### 端口配置
- **HTTP服务**: 8888
- **WebSocket服务**: 8081  
- **MySQL**: 3306
- **Redis**: 6379

## 🐛 故障排除

### 常见问题

#### 1. 数据库连接失败
```bash
# 检查MySQL服务状态
# Windows: sc query mysql
# Linux: sudo systemctl status mysql

# 检查数据库是否存在
mysql -u root -p -e "SHOW DATABASES LIKE 'sensordatabase';"
```

#### 2. Redis连接失败
```bash
# 检查Redis服务状态
redis-cli ping  # 应该返回 PONG
```

#### 3. WebSocket连接失败
- 检查端口8081是否被占用: `netstat -an | grep 8081`
- 确认防火墙允许8081端口
- 检查浏览器开发者工具中的WebSocket连接状态

#### 4. 前端数据不更新
- 确认所有三个服务都在运行
- 检查浏览器控制台是否有错误
- 验证WebSocket连接状态(右上角显示"已连接")

### 日志查看
```bash
# 查看服务运行日志
tail -f logs/system.log

# 查看数据库日志
# MySQL日志位置因系统而异
```

## 📈 性能优化

### 生产环境优化建议
1. **数据库优化**: 创建适当的索引，定期清理历史数据
2. **Redis优化**: 配置合适的内存限制和过期策略
3. **网络优化**: 使用CDN加速前端资源
4. **监控系统**: 部署监控告警系统

### 扩展部署
- **负载均衡**: 使用Nginx做反向代理
- **容器化**: 提供Docker部署方案
- **集群部署**: Redis集群，MySQL主从复制

## 🆘 技术支持

如遇到部署问题，请：
1. 查看本文档的故障排除章节
2. 检查项目GitHub Issues
3. 提交新的Issue并附上详细错误信息

---
**最后更新**: 2025-09-14  
**版本**: v1.0.0
