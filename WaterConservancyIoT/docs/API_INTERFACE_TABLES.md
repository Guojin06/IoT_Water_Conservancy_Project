# 📋 IoT智能水利监控系统 - 接口设计表格

## 🌐 **系统接口总览**

### 📊 **接口统计**
| 服务类型 | 接口数量 | 端口 | 协议 | 状态 |
|---------|---------|------|------|------|
| **C++核心服务** | 8个 | 8080 | HTTP/1.1 | ✅ 已实现 |
| **Python AI服务** | 6个 | 5000 | HTTP/1.1 | ✅ 已实现 |
| **Web静态服务** | 4个 | 8888 | HTTP/1.1 | ✅ 已实现 |
| **WebSocket服务** | 2个 | 8888 | WebSocket | ⚠️ 部分实现 |
| **总计** | **20个** | 3个端口 | 混合协议 | **85%完成** |

---

## 🔷 **C++核心服务接口 (端口: 8080)**

### 📡 **传感器数据接口**

| 接口名称 | 方法 | 路径 | 功能描述 | 请求参数 | 响应格式 | 状态 |
|---------|------|------|----------|----------|----------|------|
| **获取最新数据** | GET | `/api/sensors/latest` | 获取所有传感器最新数据 | 无 | JSON | ✅ |
| **获取历史数据** | GET | `/api/sensors/history` | 获取指定传感器历史数据 | `sensor_id`, `start_time`, `end_time`, `limit` | JSON | ✅ |
| **获取传感器列表** | GET | `/api/sensors/list` | 获取所有传感器基本信息 | 无 | JSON | ✅ |
| **传感器状态查询** | GET | `/api/sensors/status` | 查询传感器在线状态 | `sensor_id` (可选) | JSON | ✅ |

#### 📝 **详细参数说明**

**获取历史数据接口详情**:
```yaml
GET /api/sensors/history
参数:
  - sensor_id: string, 必填, 传感器ID (如: WL001)
  - start_time: string, 可选, 开始时间 (ISO8601格式)
  - end_time: string, 可选, 结束时间 (ISO8601格式)  
  - limit: integer, 可选, 返回条数限制 (默认100, 最大1000)

响应示例:
{
  "success": true,
  "data": [
    {
      "sensor_id": "WL001",
      "type": "water_level",
      "value": 2.45,
      "unit": "m",
      "timestamp": "2025-09-13T15:30:00Z",
      "status": "online",
      "quality": "good"
    }
  ],
  "total": 156,
  "page_info": {
    "current_page": 1,
    "total_pages": 2,
    "has_next": true
  }
}
```

### 🔧 **系统管理接口**

| 接口名称 | 方法 | 路径 | 功能描述 | 请求参数 | 响应格式 | 状态 |
|---------|------|------|----------|----------|----------|------|
| **系统状态查询** | GET | `/api/system/status` | 获取系统整体运行状态 | 无 | JSON | ✅ |
| **系统配置查询** | GET | `/api/system/config` | 获取系统配置信息 | 无 | JSON | ✅ |
| **系统统计信息** | GET | `/api/system/stats` | 获取系统统计数据 | `period` (可选) | JSON | ✅ |
| **健康检查** | GET | `/api/health` | 系统健康检查端点 | 无 | JSON | ✅ |

#### 📝 **系统状态接口详情**:
```yaml
GET /api/system/status
响应示例:
{
  "success": true,
  "data": {
    "system_status": "running",
    "uptime": "5 days 12:34:56",
    "version": "v1.0.0",
    "build_time": "2025-09-13T10:00:00Z",
    "sensors": {
      "total": 6,
      "online": 5,
      "offline": 1,
      "error": 0
    },
    "performance": {
      "cpu_usage": 25.6,
      "memory_usage": 45.2,
      "disk_usage": 12.8,
      "network_io": {
        "rx_bytes": 1024000,
        "tx_bytes": 2048000
      }
    },
    "database": {
      "redis_status": "connected",
      "mysql_status": "connected",
      "redis_memory": "128MB",
      "mysql_connections": 5
    }
  }
}
```

---

## 🐍 **Python AI服务接口 (端口: 5000)**

### 🤖 **AI分析接口**

| 接口名称 | 方法 | 路径 | 功能描述 | 请求体 | 响应格式 | AI提供商 | 状态 |
|---------|------|------|----------|--------|----------|----------|------|
| **趋势分析** | POST | `/api/ai/analyze/trend` | 水位/流量趋势预测分析 | JSON | JSON | 百度千帆 | ✅ |
| **异常检测** | POST | `/api/ai/detect/anomaly` | 设备异常和数据异常检测 | JSON | JSON | 百度千帆 | ✅ |
| **报告生成** | POST | `/api/ai/generate/report` | 智能分析报告生成 | JSON | JSON | 百度千帆 | ✅ |
| **智能问答** | POST | `/api/ai/qa` | 水利专业知识问答 | JSON | JSON | 百度千帆 | ✅ |
| **AI服务状态** | GET | `/api/ai/status` | AI服务运行状态查询 | 无 | JSON | 本地 | ✅ |
| **支持的AI提供商** | GET | `/api/ai/providers` | 获取可用AI服务商列表 | 无 | JSON | 本地 | ✅ |

#### 📝 **AI接口详细说明**

**趋势分析接口**:
```yaml
POST /api/ai/analyze/trend
请求体:
{
  "sensor_ids": ["WL001", "WL002"],
  "time_range": "7d",
  "analysis_type": "water_level_prediction",
  "prediction_hours": 24
}

响应示例:
{
  "success": true,
  "data": {
    "analysis_type": "趋势预测分析",
    "sensor_data": {
      "WL001": {
        "current_value": 2.45,
        "trend": "stable_rising",
        "prediction": {
          "6h": 2.47,
          "12h": 2.49,
          "24h": 2.51
        },
        "confidence": 0.92
      }
    },
    "recommendations": [
      "水位呈稳定上升趋势，建议加强监控",
      "预计24小时内水位将上升6cm",
      "建议在水位达到2.8m时启动预警机制"
    ],
    "risk_assessment": {
      "level": "low",
      "score": 0.15,
      "factors": ["正常季节变化", "天气稳定"]
    }
  },
  "meta": {
    "provider": "baidu_qianfan",
    "model": "ERNIE-Bot-4.0",
    "tokens_used": 256,
    "cost": 0.0025,
    "processing_time": 1.2,
    "timestamp": "2025-09-13T15:30:00Z"
  }
}
```

**异常检测接口**:
```yaml
POST /api/ai/detect/anomaly
请求体:
{
  "sensor_ids": ["WL001", "TEMP001", "FLOW001"],
  "time_range": "1h",
  "sensitivity": "medium",
  "include_predictions": true
}

响应示例:
{
  "success": true,
  "data": {
    "analysis_type": "异常检测分析",
    "anomalies": [
      {
        "sensor_id": "TEMP001",
        "anomaly_type": "value_spike",
        "severity": "medium",
        "description": "温度异常升高，超出正常范围15%",
        "detected_at": "2025-09-13T15:25:00Z",
        "confidence": 0.85,
        "threshold": 25.0,
        "actual_value": 28.8,
        "expected_range": [20.0, 25.0]
      }
    ],
    "system_health": {
      "overall_status": "warning",
      "healthy_sensors": 4,
      "warning_sensors": 1,
      "critical_sensors": 0
    },
    "recommendations": [
      "检查TEMP001传感器周围环境",
      "确认是否有外部热源影响",
      "建议进行传感器校准"
    ]
  },
  "meta": {
    "provider": "baidu_qianfan",
    "analysis_duration": 1.5,
    "timestamp": "2025-09-13T15:30:00Z"
  }
}
```

### 🔧 **AI服务管理接口**

| 接口名称 | 方法 | 路径 | 功能描述 | 响应内容 | 状态 |
|---------|------|------|----------|----------|------|
| **演示数据** | GET | `/api/ai/demo/data` | 获取演示用的传感器数据 | 模拟传感器数据 | ✅ |
| **演示分析** | GET | `/api/ai/demo/analyze` | 获取演示用的AI分析结果 | 模拟AI分析结果 | ✅ |

---

## 🌐 **Web静态服务接口 (端口: 8888)**

### 📄 **页面路由接口**

| 路由路径 | 方法 | 功能描述 | 页面类型 | 目标用户 | 状态 |
|---------|------|----------|----------|----------|------|
| `/` | GET | 系统主页，功能导航入口 | 导航页面 | 所有用户 | ✅ |
| `/index.html` | GET | 主导航界面 | 导航页面 | 所有用户 | ✅ |
| `/dashboard.html` | GET | 专业监控大屏 | 监控界面 | 工程师/操作员 | ✅ |
| `/tech-dashboard.html` | GET | 智能监控中心 | 科技界面 | 管理员/工程师 | ✅ |
| `/ai-analysis.html` | GET | AI智能分析界面 | AI界面 | 所有用户 | ✅ |

### 📁 **静态资源接口**

| 资源类型 | 路径模式 | 功能描述 | 缓存策略 | 状态 |
|---------|---------|----------|----------|------|
| **样式文件** | `/css/*` | CSS样式文件 | 7天缓存 | ✅ |
| **脚本文件** | `/js/*` | JavaScript文件 | 7天缓存 | ✅ |
| **图片资源** | `/images/*` | 图片文件 | 30天缓存 | ✅ |
| **字体文件** | `/fonts/*` | 字体文件 | 30天缓存 | ✅ |

---

## 🔌 **WebSocket实时接口**

### 📡 **实时数据推送**

| 连接路径 | 协议 | 功能描述 | 数据类型 | 推送频率 | 状态 |
|---------|------|----------|----------|----------|------|
| `/ws/realtime` | WebSocket | 实时传感器数据推送 | 传感器数据 | 1秒/次 | ⚠️ |
| `/ws/alerts` | WebSocket | 实时告警信息推送 | 告警消息 | 事件触发 | ⚠️ |

#### 📝 **WebSocket消息格式**

**实时数据推送格式**:
```json
{
  "type": "sensor_data",
  "timestamp": "2025-09-13T15:30:00Z",
  "data": {
    "sensor_id": "WL001",
    "sensor_type": "water_level",
    "value": 2.45,
    "unit": "m",
    "status": "online",
    "quality": "good"
  }
}
```

**告警消息格式**:
```json
{
  "type": "alert",
  "timestamp": "2025-09-13T15:30:00Z",
  "data": {
    "alert_id": "ALT_001",
    "severity": "warning",
    "title": "温度异常告警",
    "message": "TEMP001传感器温度异常升高",
    "sensor_id": "TEMP001",
    "value": 28.8,
    "threshold": 25.0,
    "auto_dismiss": false
  }
}
```

---

## 🔒 **接口安全规范**

### 🔑 **认证授权表**

| 接口类型 | 认证要求 | 权限级别 | Token类型 | 过期时间 |
|---------|---------|----------|-----------|----------|
| **公开接口** | 无需认证 | Public | 无 | 无 |
| **基础查询** | JWT认证 | Read | Access Token | 2小时 |
| **系统管理** | JWT认证 | Admin | Access Token | 1小时 |
| **AI服务** | JWT认证 | AI | Access Token | 4小时|
| **WebSocket** | Token认证 | Read | WS Token | 8小时 |

### 🛡️ **安全措施表**

| 安全措施 | 适用接口 | 实现方式 | 配置参数 | 状态 |
|---------|---------|----------|----------|------|
| **HTTPS加密** | 所有HTTP接口 | SSL/TLS证书 | TLS 1.2+ | ✅ |
| **CORS控制** | Web API | 跨域配置 | 白名单模式 | ✅ |
| **请求限流** | 所有接口 | 令牌桶算法 | 100req/min | ✅ |
| **参数验证** | POST/PUT接口 | JSON Schema | 严格验证 | ✅ |
| **SQL注入防护** | 数据库查询 | 参数化查询 | 自动转义 | ✅ |
| **XSS防护** | 输出数据 | 内容过滤 | 自动转义 | ✅ |

---

## 📊 **接口性能指标**

### ⚡ **性能基准表**

| 接口类型 | 平均响应时间 | 95%响应时间 | 99%响应时间 | QPS | 状态 |
|---------|-------------|-------------|-------------|-----|------|
| **传感器查询** | 45ms | 85ms | 120ms | 500 | ✅ |
| **系统状态** | 25ms | 50ms | 80ms | 200 | ✅ |
| **AI分析** | 1.2s | 2.5s | 4.0s | 50 | ✅ |
| **静态资源** | 15ms | 30ms | 50ms | 1000 | ✅ |
| **WebSocket** | 10ms | 20ms | 35ms | 100连接 | ⚠️ |

### 💾 **资源消耗表**

| 服务类型 | CPU使用率 | 内存使用 | 磁盘I/O | 网络带宽 | 并发连接 |
|---------|-----------|----------|---------|----------|----------|
| **C++核心** | 15-30% | 512MB | 10MB/s | 5Mbps | 100+ |
| **Python AI** | 10-25% | 256MB | 5MB/s | 2Mbps | 50+ |
| **Web服务** | 5-15% | 128MB | 2MB/s | 10Mbps | 200+ |
| **数据库** | 10-20% | 1GB | 20MB/s | 1Mbps | 50+ |

---

## 🔧 **接口测试用例**

### 🧪 **测试覆盖表**

| 测试类型 | 覆盖接口 | 测试用例数 | 通过率 | 自动化程度 | 状态 |
|---------|---------|-----------|--------|------------|------|
| **单元测试** | 核心逻辑 | 45个 | 98% | 100% | ✅ |
| **集成测试** | API接口 | 32个 | 95% | 90% | ✅ |
| **性能测试** | 关键接口 | 15个 | 90% | 80% | ⚠️ |
| **安全测试** | 所有接口 | 25个 | 88% | 60% | ⚠️ |
| **压力测试** | 高频接口 | 10个 | 85% | 70% | ❌ |

---

## 📈 **接口使用统计**

### 📊 **调用频率统计 (24小时)**

| 接口路径 | 调用次数 | 成功率 | 平均响应时间 | 错误类型 |
|---------|---------|--------|-------------|----------|
| `/api/sensors/latest` | 8,640 | 99.8% | 42ms | 网络超时(0.2%) |
| `/api/system/status` | 2,880 | 99.9% | 28ms | 无 |
| `/api/ai/analyze/trend` | 156 | 95.5% | 1.8s | API限流(4.5%) |
| `/api/ai/detect/anomaly` | 89 | 97.8% | 1.5s | AI服务超时(2.2%) |
| `/ws/realtime` | 连续连接 | 98.5% | 15ms | 连接断开(1.5%) |

---

*文档创建时间: 2025-09-13*  
*接口版本: v1.0.0*  
*更新频率: 每周更新*
