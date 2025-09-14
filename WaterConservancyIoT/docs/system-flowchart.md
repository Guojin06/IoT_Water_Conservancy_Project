# 智能水利物联网系统 - 系统流程图

## 系统架构流程图

```mermaid
flowchart TD
    %% 感知层
    subgraph 感知层 ["🌊 感知层 (Physical Layer)"]
        WL1[水位传感器 WL001<br/>主水库大坝]
        WL2[水位传感器 WL002<br/>副水库]
        TEMP1[温度传感器 TEMP001<br/>入水口]
        TEMP2[温度传感器 TEMP002<br/>出水口]
        FLOW1[流量传感器<br/>模拟数据]
        DEV1[设备状态<br/>泵站/闸门]
    end

    %% 数据采集层
    subgraph 数据采集层 ["📡 数据采集层 (Data Collection)"]
        SIM[传感器模拟器<br/>SensorSimulator]
        COLLECTOR[数据采集器<br/>多线程采集]
        VALIDATOR[数据验证器<br/>格式验证]
    end

    %% 数据处理层
    subgraph 数据处理层 ["⚙️ 数据处理层 (Processing Layer)"]
        QUEUE[消息队列<br/>MessageQueue]
        PROCESSOR[数据处理引擎<br/>WaterDataProcessor]
        CLEANER[数据清洗<br/>异常值处理]
        FILTER[数据过滤<br/>噪声过滤]
        AGGREGATOR[数据聚合<br/>统计分析]
        THREAD_POOL[线程池<br/>4个工作线程]
    end

    %% 存储层
    subgraph 存储层 ["💾 存储层 (Storage Layer)"]
        REDIS[Redis缓存<br/>实时数据缓存]
        CACHE[热点数据<br/>快速访问]
        PERSISTENT[数据持久化<br/>历史数据]
        BACKUP[数据备份<br/>容灾备份]
    end

    %% 服务层
    subgraph 服务层 ["🔌 服务层 (Service Layer)"]
        HTTP_SERVER[HTTP服务器<br/>端口:8080]
        API_GATEWAY[API网关<br/>RESTful接口]
        AUTH[身份认证<br/>安全控制]
        RATE_LIMIT[流量控制<br/>防护机制]
    end

    %% AI分析层
    subgraph AI分析层 ["🤖 AI分析层 (AI Analysis)"]
        AI_SERVICE[AI服务<br/>端口:5000]
        BAIDU_API[百度千帆API<br/>1000次/日免费]
        TREND_ANALYSIS[趋势分析<br/>水位预测]
        ANOMALY_DETECTION[异常检测<br/>故障预警]
        REPORT_GEN[报告生成<br/>智能分析]
    end

    %% 应用层
    subgraph 应用层 ["🖥️ 应用层 (Application Layer)"]
        WEB_MAIN[主导航界面<br/>index.html]
        DASHBOARD[监控大屏<br/>dashboard.html]
        TECH_DASH[智能监控中心<br/>tech-dashboard.html]
        AI_INTERFACE[AI分析界面<br/>ai-analysis.html]
    end

    %% 用户层
    subgraph 用户层 ["👥 用户层 (User Layer)"]
        OPERATOR[系统操作员]
        ENGINEER[水利工程师]
        MANAGER[管理人员]
        VISITOR[访客用户]
    end

    %% 数据流连接
    WL1 --> SIM
    WL2 --> SIM
    TEMP1 --> SIM
    TEMP2 --> SIM
    FLOW1 --> SIM
    DEV1 --> SIM

    SIM --> COLLECTOR
    COLLECTOR --> VALIDATOR
    VALIDATOR --> QUEUE

    QUEUE --> PROCESSOR
    PROCESSOR --> CLEANER
    CLEANER --> FILTER
    FILTER --> AGGREGATOR
    THREAD_POOL -.-> PROCESSOR

    AGGREGATOR --> REDIS
    REDIS --> CACHE
    REDIS --> PERSISTENT
    PERSISTENT --> BACKUP

    REDIS --> HTTP_SERVER
    HTTP_SERVER --> API_GATEWAY
    API_GATEWAY --> AUTH
    AUTH --> RATE_LIMIT

    REDIS --> AI_SERVICE
    AI_SERVICE --> BAIDU_API
    BAIDU_API --> TREND_ANALYSIS
    BAIDU_API --> ANOMALY_DETECTION
    BAIDU_API --> REPORT_GEN

    RATE_LIMIT --> WEB_MAIN
    RATE_LIMIT --> DASHBOARD
    RATE_LIMIT --> TECH_DASH
    AI_SERVICE --> AI_INTERFACE

    WEB_MAIN --> OPERATOR
    DASHBOARD --> ENGINEER
    TECH_DASH --> MANAGER
    AI_INTERFACE --> VISITOR

    %% 样式定义
    classDef sensorClass fill:#e1f5fe,stroke:#01579b,stroke-width:2px
    classDef processClass fill:#f3e5f5,stroke:#4a148c,stroke-width:2px
    classDef storageClass fill:#e8f5e8,stroke:#1b5e20,stroke-width:2px
    classDef serviceClass fill:#fff3e0,stroke:#e65100,stroke-width:2px
    classDef aiClass fill:#fce4ec,stroke:#880e4f,stroke-width:2px
    classDef appClass fill:#f1f8e9,stroke:#33691e,stroke-width:2px
    classDef userClass fill:#e3f2fd,stroke:#0d47a1,stroke-width:2px

    class WL1,WL2,TEMP1,TEMP2,FLOW1,DEV1 sensorClass
    class SIM,COLLECTOR,VALIDATOR,QUEUE,PROCESSOR,CLEANER,FILTER,AGGREGATOR,THREAD_POOL processClass
    class REDIS,CACHE,PERSISTENT,BACKUP storageClass
    class HTTP_SERVER,API_GATEWAY,AUTH,RATE_LIMIT serviceClass
    class AI_SERVICE,BAIDU_API,TREND_ANALYSIS,ANOMALY_DETECTION,REPORT_GEN aiClass
    class WEB_MAIN,DASHBOARD,TECH_DASH,AI_INTERFACE appClass
    class OPERATOR,ENGINEER,MANAGER,VISITOR userClass
```

## 技术架构特点

### 🏗️ 分层架构设计
- **感知层**: 多种传感器数据采集
- **处理层**: 高性能C++数据处理引擎
- **存储层**: Redis缓存 + 数据持久化
- **服务层**: RESTful API接口
- **AI层**: 智能分析与预测
- **应用层**: 专业监控界面

### ⚡ 性能优化
- **并发处理**: 4个工作线程并行处理
- **缓存机制**: Redis热点数据缓存
- **异步处理**: 消息队列解耦
- **负载均衡**: 多传感器负载分散

### 🔒 安全可靠
- **数据验证**: 多层数据校验
- **异常处理**: 完善的错误处理机制
- **容灾备份**: 数据持久化和备份
- **访问控制**: API安全认证

### 🤖 智能特性
- **免费AI**: 百度千帆1000次/日免费额度
- **趋势预测**: 水位变化趋势分析
- **异常检测**: 设备故障预警
- **智能报告**: 自动生成分析报告
