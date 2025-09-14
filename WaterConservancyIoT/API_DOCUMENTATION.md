# 系统API接口一览表

本文档列出了智能水库监测系统的所有后端API接口。

## 认证接口 (`/api/auth`)

### 1. 用户登录
- **路径**: `/api/auth/login`
- **方法**: `POST`
- **描述**: 用于用户登录认证。
- **请求体 (Request Body)**:
  ```json
  {
      "username": "admin",
      "password": "password123"
  }
  ```
- **成功响应 (Success Response)** - `200 OK`:
  ```json
  {
      "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWI..."
  }
  ```
- **失败响应 (Error Response)** - `401 Unauthorized`:
  ```json
  {
      "error": "Invalid credentials"
  }
  ```

---

## 传感器数据接口 (`/api/sensors`)

### 1. 获取最新传感器数据 (受保护)
- **路径**: `/api/sensors/recent`
- **方法**: `GET`
- **描述**: 获取最新的传感器数据。**这是一个受保护的接口，需要有效的JWT认证。**
- **认证 (Authentication)**:
  - **类型**: `Bearer Token`
  - **请求头 (Header)**: `Authorization: Bearer <your_jwt_token>`
- **成功响应 (Success Response)** - `200 OK`:
  ```json
  {
      "message": "Access granted to protected sensor data!"
  }
  ```
  *(注: 当前实现仅为演示认证，后续可修改为返回真实的Redis数据)*
- **失败响应 (Error Response)** - `401 Unauthorized`:
  ```json
  {
      "error": "Authentication required"
  }
  ```

---

## 实时通信接口 (`/ws`)

### 1. WebSocket连接
- **路径**: `ws://localhost:8081`
- **描述**: 建立WebSocket连接以接收实时的传感器数据广播。
- **认证**: **当前版本未实现。** 后续可以设计为在连接时通过URL参数或首条消息传递JWT进行认证。
- **接收消息格式**:
  ```json
  {
    "type": "sensor_data",
    "timestamp": "2025-09-14T15:00:00.123Z",
    "data": {
      "sensor_id": "WL001",
      "value": 88.42,
      "unit": "m",
      // ... 其他传感器元数据
    }
  }
  ```
