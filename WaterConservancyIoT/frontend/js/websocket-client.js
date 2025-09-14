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
                this.stopHeartbeat();
                
                // 自动重连
                if (this.reconnectAttempts < this.maxReconnectAttempts) {
                    this.reconnectAttempts++;
                    console.log(`🔄 ${this.reconnectInterval/1000}秒后尝试第${this.reconnectAttempts}次重连...`);
                    setTimeout(() => this.connect(), this.reconnectInterval);
                }
            };
            
            this.ws.onerror = (error) => {
                console.error('❌ WebSocket错误:', error);
                this.updateConnectionStatus('error');
                this.triggerCallbacks('onError', error);
            };
            
        } catch (error) {
            console.error('❌ 创建WebSocket连接失败:', error);
            this.updateConnectionStatus('error');
        }
    }
    
    disconnect() {
        if (this.ws) {
            this.ws.close();
            this.stopHeartbeat();
        }
    }
    
    send(message) {
        if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(message));
            return true;
        }
        console.warn('⚠️ WebSocket未连接，无法发送消息');
        return false;
    }
    
    handleMessage(data) {
        try {
            const message = JSON.parse(data);
            console.log('📨 收到消息:', message.type);
            
            switch (message.type) {
                case 'sensor_data':
                    this.triggerCallbacks('onSensorData', message);
                    this.updateSensorDisplay(message);
                    break;
                case 'alert':
                    this.triggerCallbacks('onAlert', message);
                    this.showAlert(message);
                    break;
                case 'system_status':
                    this.triggerCallbacks('onSystemStatus', message);
                    break;
                case 'pong':
                    console.log('💓 收到心跳响应');
                    break;
                default:
                    console.log('📋 未知消息类型:', message.type);
            }
        } catch (error) {
            console.error('❌ 解析WebSocket消息失败:', error);
        }
    }
    
    updateConnectionStatus(status) {
        const statusElement = document.getElementById('ws-status');
        if (statusElement) {
            const statusMap = {
                'connected': { text: '🟢 已连接', class: 'status-connected' },
                'connecting': { text: '🟡 连接中...', class: 'status-connecting' },
                'disconnected': { text: '🔴 已断开', class: 'status-disconnected' },
                'error': { text: '❌ 连接错误', class: 'status-error' }
            };
            
            const statusInfo = statusMap[status] || statusMap['disconnected'];
            statusElement.textContent = statusInfo.text;
            statusElement.className = `ws-status ${statusInfo.class}`;
        }
    }
    
    updateSensorDisplay(message) {
        if (message.sensors && Array.isArray(message.sensors)) {
            message.sensors.forEach(sensor => {
                const element = document.getElementById(`sensor-${sensor.id}`);
                if (element) {
                    const valueElement = element.querySelector('.sensor-value');
                    const statusElement = element.querySelector('.sensor-status');
                    const timeElement = element.querySelector('.sensor-time');
                    
                    if (valueElement) {
                        valueElement.textContent = `${sensor.value}${sensor.unit}`;
                    }
                    if (statusElement) {
                        statusElement.textContent = sensor.status;
                        statusElement.className = `sensor-status status-${sensor.status.toLowerCase()}`;
                    }
                    if (timeElement) {
                        timeElement.textContent = new Date().toLocaleTimeString();
                    }
                }
            });
        }
    }
    
    showAlert(message) {
        console.warn('🚨 告警:', message.message);
        
        // 创建告警通知
        const alertContainer = document.getElementById('alerts-container') || document.body;
        const alertElement = document.createElement('div');
        alertElement.className = 'alert-notification';
        alertElement.innerHTML = `
            <div class="alert-content">
                <span class="alert-icon">⚠️</span>
                <span class="alert-message">${message.message}</span>
                <span class="alert-time">${new Date().toLocaleTimeString()}</span>
                <button class="alert-close" onclick="this.parentElement.parentElement.remove()">×</button>
            </div>
        `;
        
        alertContainer.appendChild(alertElement);
        
        // 3秒后自动移除
        setTimeout(() => {
            if (alertElement.parentNode) {
                alertElement.parentNode.removeChild(alertElement);
            }
        }, 5000);
    }
    
    startHeartbeat() {
        this.heartbeatInterval = setInterval(() => {
            if (this.ws && this.ws.readyState === WebSocket.OPEN) {
                this.send({ type: 'ping', timestamp: Date.now() });
            }
        }, 30000); // 每30秒发送心跳
    }
    
    stopHeartbeat() {
        if (this.heartbeatInterval) {
            clearInterval(this.heartbeatInterval);
            this.heartbeatInterval = null;
        }
    }
    
    // 事件回调管理
    on(event, callback) {
        if (this.callbacks[event]) {
            this.callbacks[event].push(callback);
        }
    }
    
    off(event, callback) {
        if (this.callbacks[event]) {
            const index = this.callbacks[event].indexOf(callback);
            if (index > -1) {
                this.callbacks[event].splice(index, 1);
            }
        }
    }
    
    triggerCallbacks(event, data) {
        if (this.callbacks[event]) {
            this.callbacks[event].forEach(callback => {
                try {
                    callback(data);
                } catch (error) {
                    console.error(`回调执行错误 (${event}):`, error);
                }
            });
        }
    }
}

// 全局WebSocket客户端实例
let wsClient = null;

// 页面加载完成后自动初始化
document.addEventListener('DOMContentLoaded', function() {
    console.log('🚀 初始化WebSocket客户端...');
    wsClient = new WebSocketClient();
    
    // 暴露到全局作用域供其他脚本使用
    window.wsClient = wsClient;
});

// 页面卸载时清理连接
window.addEventListener('beforeunload', function() {
    if (wsClient) {
        wsClient.disconnect();
    }
});
