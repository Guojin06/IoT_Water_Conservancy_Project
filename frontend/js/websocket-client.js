/**
 * WebSocket客户端 - IoT水利监控系统实时通信
 */

class WebSocketClient {
    constructor(url = 'ws://localhost:8081') {
        this.url = url;
        this.ws = null;
        this.reconnectAttempts = 0;
        this.maxReconnectAttempts = 5;
        this.reconnectInterval = 3000;
        this.isConnected = false;
        this.callbacks = {
            onSensorData: [],
            onAlert: [],
            onSystemStatus: [],
            onConnect: [],
            onDisconnect: [],
            onError: []
        };
        
        this.init();
    }
    
    init() {
        this.connect();
    }
    
    connect() {
        try {
            console.log(`🔌 连接WebSocket服务器: ${this.url}`);
            this.ws = new WebSocket(this.url);
            
            this.ws.onopen = (event) => {
                console.log('✅ WebSocket连接成功');
                this.isConnected = true;
                this.reconnectAttempts = 0;
                this.updateConnectionStatus('connected');
                this.triggerCallbacks('onConnect', event);
                
                // 发送心跳
                this.startHeartbeat();
            };
            
            this.ws.onmessage = (event) => {
                this.handleMessage(event.data);
            };
            
            this.ws.onclose = (event) => {
                console.log('❌ WebSocket连接关闭', event.code, event.reason);
                this.isConnected = false;
                this.updateConnectionStatus('disconnected');
                this.triggerCallbacks('onDisconnect', event);
                
                // 尝试重连
                if (this.reconnectAttempts < this.maxReconnectAttempts) {
                    this.reconnectAttempts++;
                    console.log(`🔄 ${this.reconnectInterval/1000}秒后尝试重连 (${this.reconnectAttempts}/${this.maxReconnectAttempts})`);
                    setTimeout(() => this.connect(), this.reconnectInterval);
                } else {
                    console.log('❌ 达到最大重连次数，停止重连');
                    this.updateConnectionStatus('failed');
                }
            };
            
            this.ws.onerror = (event) => {
                console.error('❌ WebSocket错误:', event);
                this.triggerCallbacks('onError', event);
            };
            
        } catch (error) {
            console.error('❌ WebSocket连接失败:', error);
            this.updateConnectionStatus('error');
        }
    }
    
    handleMessage(data) {
        try {
            const message = JSON.parse(data);
            console.log('📨 收到消息:', message.type);
            
            switch (message.type) {
                case 'welcome':
                    console.log('👋 服务器欢迎消息:', message.message);
                    break;
                    
                case 'sensor_data':
                    this.triggerCallbacks('onSensorData', message.data);
                    this.updateSensorDisplay(message.data);
                    break;
                    
                case 'alert':
                    this.triggerCallbacks('onAlert', message.data);
                    this.showAlert(message.data);
                    break;
                    
                case 'system_status':
                    this.triggerCallbacks('onSystemStatus', message.data);
                    break;
                    
                case 'pong':
                    console.log('💓 心跳响应');
                    break;
                    
                default:
                    console.log('❓ 未知消息类型:', message.type);
            }
            
        } catch (error) {
            console.error('❌ 消息解析失败:', error);
        }
    }
    
    send(message) {
        if (this.isConnected && this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(message));
            return true;
        } else {
            console.warn('⚠️ WebSocket未连接，无法发送消息');
            return false;
        }
    }
    
    startHeartbeat() {
        setInterval(() => {
            if (this.isConnected) {
                this.send({ type: 'ping', timestamp: new Date().toISOString() });
            }
        }, 30000); // 30秒心跳
    }
    
    // 事件监听器管理
    on(event, callback) {
        if (this.callbacks[event]) {
            this.callbacks[event].push(callback);
        }
    }
    
    triggerCallbacks(event, data) {
        if (this.callbacks[event]) {
            this.callbacks[event].forEach(callback => {
                try {
                    callback(data);
                } catch (error) {
                    console.error(`❌ 回调执行失败 (${event}):`, error);
                }
            });
        }
    }
    
    // UI更新方法
    updateConnectionStatus(status) {
        const statusElement = document.getElementById('ws-status');
        if (statusElement) {
            statusElement.className = `ws-status ${status}`;
            const statusText = {
                'connected': '🟢 实时连接',
                'disconnected': '🟡 连接断开',
                'failed': '🔴 连接失败',
                'error': '🔴 连接错误'
            };
            statusElement.textContent = statusText[status] || '❓ 未知状态';
        }
    }
    
    updateSensorDisplay(sensorData) {
        console.log('📊 更新传感器显示:', sensorData.sensor_id, sensorData.value);
        // 这里可以添加更多的UI更新逻辑
    }
    
    showAlert(alertData) {
        console.log('🚨 显示告警:', alertData.title);
        // 这里可以添加告警显示逻辑
    }
    
    // 公共方法
    getSystemStatus() {
        return this.send({ type: 'get_status' });
    }
    
    subscribeSensors(sensorIds) {
        return this.send({ type: 'subscribe', sensor_ids: sensorIds });
    }
    
    disconnect() {
        if (this.ws) {
            this.ws.close();
        }
    }
}

// 全局WebSocket客户端实例
window.wsClient = null;

// 初始化WebSocket连接
function initWebSocket() {
    if (window.wsClient) {
        window.wsClient.disconnect();
    }
    
    window.wsClient = new WebSocketClient();
    
    // 设置事件监听器
    window.wsClient.on('onSensorData', (data) => {
        console.log('📊 传感器数据更新:', data.sensor_id, data.value);
    });
    
    window.wsClient.on('onAlert', (data) => {
        console.log('🚨 收到告警:', data.title);
    });
    
    window.wsClient.on('onConnect', () => {
        console.log('🎉 WebSocket连接成功');
        // 订阅所有传感器
        window.wsClient.subscribeSensors(['WL001', 'WL002', 'TEMP001', 'TEMP002', 'FLOW001', 'PH001']);
    });
}

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', initWebSocket);