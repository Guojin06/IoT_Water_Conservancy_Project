# 🌊 智能水利物联网监控系统

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Python](https://img.shields.io/badge/Python-3.8+-green.svg)](https://python.org/)
[![Vue.js](https://img.shields.io/badge/Vue.js-3.0-green.svg)](https://vuejs.org/)

> 基于物联网架构的智能水利监控与调度系统原型，采用现代C++和微服务架构设计

## 📋 项目简介

本项目是一个完整的智能水利物联网监控系统原型，通过模拟传感器数据流和智能分析算法，实现了实时数据处理、水质分析、设备管理、智能预警等核心功能。项目采用现代C++17开发高性能后端服务，结合Vue.js构建专业监控界面，为智能水利管理提供技术验证平台。

### 🎯 核心特性

- 🚀 **高性能后端**: C++17微服务架构，支持1000+并发连接
- 📊 **实时数据处理**: 毫秒级响应，支持多传感器数据流
- 💾 **智能缓存系统**: Redis分布式缓存 + MySQL持久化存储
- 🖥️ **专业监控界面**: 双界面设计（监控大屏 + 智能分析中心）
- 🧵 **并发处理**: 线程池 + 消息队列异步处理机制
- 🤖 **AI扩展支持**: 预留智能分析和预测功能接口
- 🐳 **容器化部署**: Docker支持，便于云端部署

## 🏗️ 系统架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   感知层        │    │   处理层        │    │   应用层        │
│                 │    │                 │    │                 │
│ • 传感器模拟    │────│ • 数据处理      │────│ • Web界面       │
│ • 数据采集      │    │ • 存储管理      │    │ • API服务       │
│ • 协议转换      │    │ • 消息队列      │    │ • 监控大屏      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## 🛠️ 技术栈

### 后端技术
- **语言**: C++17
- **数据库**: MySQL 8.0 + Redis 6.0
- **构建工具**: CMake 3.16+
- **测试框架**: Google Test
- **网络库**: HTTP/TCP Socket编程
- **并发**: std::thread, 线程池, 消息队列

### 前端技术
- **框架**: Vue.js 3.0
- **可视化**: ECharts 5.4
- **UI风格**: 响应式设计
- **API调用**: Axios

### 开发工具
- **版本控制**: Git
- **容器化**: Docker + Docker Compose
- **文档**: Markdown
- **IDE**: 支持C++17的现代IDE

## 🚀 快速开始

### 环境要求

- C++17 编译器 (GCC 7.0+ / MSVC 2019+ / Clang 5.0+)
- CMake 3.16+
- Python 3.8+
- Node.js 14+ (可选，用于前端开发)

### 编译运行

1. **克隆项目**
```bash
git clone https://github.com/Guojin06/IoT_Water_Conservancy_Project.git
cd IoT_Water_Conservancy_Project/WaterConservancyIoT
```

2. **编译后端**
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

3. **运行系统**
```bash
# 启动完整系统测试
./test_system_en

# 或启动Python API服务器
python simple_server.py
```

4. **访问界面**
```
主页面: http://localhost:8888
监控大屏: http://localhost:8888/dashboard.html
智能监控中心: http://localhost:8888/tech-dashboard.html
```

## 📁 项目结构

```
WaterConservancyIoT/
├── src/                    # C++源代码
│   ├── common/            # 公共模块 (日志、配置、工具)
│   ├── sensors/           # 传感器模块
│   ├── data_processor/    # 数据处理模块
│   ├── storage/           # 存储模块
│   ├── threading/         # 多线程模块
│   ├── messaging/         # 消息队列模块
│   ├── api_server/        # API服务器模块
│   └── main.cpp           # 主程序入口
├── frontend/              # 前端界面
│   ├── index.html         # 科技风格界面
│   ├── dashboard.html     # 监控大屏
│   ├── tech-dashboard.html # 智能监控中心
│   └── main.html          # 主导航页面
├── tests/                 # 单元测试
├── docs/                  # 项目文档
├── external/              # 外部依赖库
├── CMakeLists.txt         # CMake构建配置
└── simple_server.py       # Python API服务器
```

## 🔧 核心功能模块

### 1. 传感器数据模拟
- 水位传感器: 模拟水库水位变化
- 温度传感器: 环境温度监测
- 流量传感器: 水流量实时监控
- 设备状态: 泵站、闸门等设备状态

### 2. 数据处理引擎
- 实时数据清洗和过滤
- 异常值检测和处理
- 数据聚合和统计分析
- 多线程并发处理

### 3. 存储管理系统
- Redis缓存: 热点数据快速访问
- MySQL存储: 历史数据持久化
- 数据一致性保证
- 查询性能优化

### 4. Web监控界面
- **监控大屏**: 专业水利监控界面
- **智能中心**: 科技感操作界面
- 实时数据图表展示
- 响应式布局设计

## 📊 性能指标

- **并发处理**: 支持1000+并发连接
- **响应时间**: API响应 < 100ms
- **数据处理**: 实时处理多路传感器数据流
- **缓存命中**: Redis缓存命中率 > 95%
- **系统稳定**: 7x24小时稳定运行

## 🤖 AI扩展规划

项目预留了AI智能分析模块接口，计划实现：

- 📈 **预测分析**: 水位、流量趋势预测
- 🚨 **异常检测**: 设备故障预警
- 🎯 **智能调度**: 多目标优化调度算法
- 📋 **决策支持**: 智能化运维建议

详见: [AI扩展计划文档](docs/ai-extension-plan.md)

## 🐳 部署方案

### Docker容器化部署

```bash
# 构建镜像
docker build -t water-iot-system .

# 运行容器
docker-compose up -d
```

### 云服务器部署

支持部署到主流云平台:
- 阿里云 ECS
- 腾讯云 CVM  
- AWS EC2
- Azure VM

## 📚 文档资源

- [系统设计文档](docs/system-design.md)
- [API接口文档](docs/api-documentation.md)
- [开发学习笔记](docs/lectures/)
- [AI扩展计划](docs/ai-extension-plan.md)
- [部署运维指南](docs/deployment-guide.md)

## 🧪 测试

```bash
# 运行单元测试
cd build
ctest

# 运行集成测试
./test_system_en
```

## 🤝 贡献指南

欢迎提交Issue和Pull Request！

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 📄 开源协议

本项目采用 MIT 协议 - 查看 [LICENSE](LICENSE) 文件了解详情

## 👨‍💻 作者

- **Guojin** - *项目创建者和主要开发者* - [GitHub](https://github.com/Guojin06)

## 🙏 致谢

感谢所有为这个项目提供帮助和建议的朋友们！

---

⭐ 如果这个项目对你有帮助，请给个Star支持一下！