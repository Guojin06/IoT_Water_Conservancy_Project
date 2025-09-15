# SSH连接问题解决指南

## 🔧 "Too many authentication failures" 错误解决方案

### 方法1: 强制密码认证
```bash
ssh -o PreferredAuthentications=password -o PubkeyAuthentication=no root@8.134.136.216
```

### 方法2: 限制认证尝试
```bash
ssh -o PasswordAuthentication=yes -o PreferredAuthentications=password root@8.134.136.216
```

### 方法3: 使用SSH配置文件
创建 `~/.ssh/config` 文件：
```
Host water-server
    HostName 8.134.136.216
    User root
    PreferredAuthentications password
    PubkeyAuthentication no
    PasswordAuthentication yes
```

然后连接：
```bash
ssh water-server
```

### 方法4: 清理SSH缓存后重连
```bash
ssh-keygen -R 8.134.136.216
ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@8.134.136.216
```

## 🚀 一键部署解决方案

### Windows用户（推荐）
```powershell
# 远程部署到你的服务器
.\deploy-windows.ps1 -ServerIP 8.134.136.216

# 或本地测试
.\deploy-windows.ps1 -Local
```

### 直接在服务器执行（如果SSH连通）
```bash
# 登录服务器后执行
curl -fsSL https://raw.githubusercontent.com/Guojin06/IoT_Water_Conservancy_Project/main/docker-deploy.sh | bash
```

### 手动上传文件部署
如果SSH完全连不上，可以：
1. 通过云服务器控制台的"远程连接"功能
2. 或使用SFTP工具上传文件
3. 或通过云盘分享链接下载

## 🔍 连接问题诊断

### 检查服务器状态
```bash
# 检查SSH服务
sudo systemctl status ssh

# 检查端口是否开放
netstat -tlnp | grep :22

# 查看SSH日志
sudo tail -f /var/log/auth.log
```

### 检查防火墙
```bash
# Ubuntu/Debian
sudo ufw status
sudo ufw allow ssh

# CentOS/RHEL
sudo firewall-cmd --list-all
sudo firewall-cmd --permanent --add-service=ssh
```

## 🌐 云服务器控制台连接

大多数云服务商都提供Web控制台连接：
- 阿里云：ECS实例 -> 远程连接 -> Workbench
- 腾讯云：CVM实例 -> 登录 -> 标准登录
- 华为云：ECS实例 -> 远程登录 -> CloudShell
- AWS：EC2实例 -> Connect -> EC2 Instance Connect

## 📋 部署验证清单

部署完成后检查：
- [ ] 访问 http://8.134.136.216:8888 可以打开
- [ ] 可以用 admin/admin123 登录
- [ ] 仪表板显示实时数据
- [ ] 系统管理页面正常
- [ ] WebSocket连接正常

## 🆘 如果所有方法都失败

请提供以下信息：
1. 云服务商名称
2. 错误的完整输出
3. 服务器控制台截图
4. SSH客户端类型（PuTTY/OpenSSH等）
