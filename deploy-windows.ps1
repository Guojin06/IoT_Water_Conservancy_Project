# 智能水利系统 - Windows Docker一键部署脚本
# PowerShell版本

param(
    [string]$ServerIP = "",
    [switch]$Local = $false
)

# 颜色输出函数
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    } else {
        $input | Write-Output
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Write-Success { Write-ColorOutput Green $args }
function Write-Warning { Write-ColorOutput Yellow $args }
function Write-Error { Write-ColorOutput Red $args }
function Write-Info { Write-ColorOutput Cyan $args }

Write-Info "🚀 智能水利系统 - Docker一键部署开始..."
Write-Info "================================================"

# 检查Docker Desktop
function Test-Docker {
    try {
        $dockerVersion = docker --version
        Write-Success "✅ Docker已安装: $dockerVersion"
        return $true
    } catch {
        Write-Error "❌ Docker未安装或未启动"
        Write-Warning "请先安装Docker Desktop: https://www.docker.com/products/docker-desktop"
        return $false
    }
}

# 检查Docker Compose
function Test-DockerCompose {
    try {
        $composeVersion = docker-compose --version
        Write-Success "✅ Docker Compose已安装: $composeVersion"
        return $true
    } catch {
        Write-Error "❌ Docker Compose未找到"
        return $false
    }
}

# 检查Git
function Test-Git {
    try {
        $gitVersion = git --version
        Write-Success "✅ Git已安装: $gitVersion"
        return $true
    } catch {
        Write-Error "❌ Git未安装"
        Write-Warning "请先安装Git: https://git-scm.com/download/win"
        return $false
    }
}

# 设置项目
function Setup-Project {
    $projectDir = "IoT_Water_Conservancy_Project"
    
    if (Test-Path $projectDir) {
        Write-Warning "📁 项目目录已存在，正在更新..."
        Set-Location $projectDir
        git pull origin main
    } else {
        Write-Info "📥 正在克隆项目..."
        git clone https://github.com/Guojin06/IoT_Water_Conservancy_Project.git
        Set-Location $projectDir
    }
    
    Write-Success "✅ 项目代码准备完成"
}

# 创建环境配置
function Setup-Environment {
    Write-Info "🔧 配置环境变量..."
    
    $envContent = @"
# 数据库配置
MYSQL_ROOT_PASSWORD=water_conservancy_2024
MYSQL_DATABASE=sensordatabase
MYSQL_USER=water_user
MYSQL_PASSWORD=water_user_2024

# Redis配置
REDIS_PASSWORD=redis_2024

# 应用配置
APP_ENV=production
JWT_SECRET_KEY=$((New-Guid).Guid.Replace('-',''))
DEBUG=false

# 网络配置
HTTP_PORT=8888
WEBSOCKET_PORT=8081
"@
    
    $envContent | Out-File -FilePath ".env" -Encoding UTF8
    Write-Success "✅ 环境配置完成"
}

# 启动服务
function Start-Services {
    Write-Info "🚀 启动服务..."
    
    # 停止可能运行的旧容器
    docker-compose down 2>$null
    
    # 构建并启动所有服务
    docker-compose up -d --build
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "✅ 服务启动完成"
        
        # 等待服务就绪
        Write-Info "⏳ 等待服务启动..."
        Start-Sleep -Seconds 10
        
        # 显示访问信息
        Write-Info ""
        Write-Success "🎉 部署成功！"
        Write-Info "================================================"
        Write-Info "📊 访问地址:"
        if ($Local) {
            Write-Info "   http://localhost:8888"
        } elseif ($ServerIP) {
            Write-Info "   http://${ServerIP}:8888"
        } else {
            Write-Info "   http://localhost:8888"
        }
        Write-Info ""
        Write-Info "👤 默认登录:"
        Write-Info "   用户名: admin"
        Write-Info "   密码: admin123"
        Write-Info ""
        Write-Info "🔧 管理命令:"
        Write-Info "   查看日志: docker-compose logs -f"
        Write-Info "   停止服务: docker-compose down"
        Write-Info "   重启服务: docker-compose restart"
        Write-Info "   查看状态: docker-compose ps"
        
    } else {
        Write-Error "❌ 服务启动失败"
        Write-Info "查看详细日志: docker-compose logs"
        exit 1
    }
}

# SSH连接到服务器部署
function Deploy-ToServer {
    param([string]$ServerIP)
    
    Write-Info "🌐 准备远程部署到服务器: $ServerIP"
    
    # 创建远程部署脚本
    $remoteScript = @"
#!/bin/bash
cd /tmp
curl -fsSL https://raw.githubusercontent.com/Guojin06/IoT_Water_Conservancy_Project/main/docker-deploy.sh -o deploy.sh
chmod +x deploy.sh
./deploy.sh
"@
    
    $remoteScript | Out-File -FilePath "remote-deploy.sh" -Encoding UTF8
    
    Write-Info "📤 上传部署脚本到服务器..."
    
    try {
        # 尝试不同的SSH认证方式
        Write-Info "尝试SSH连接..."
        
        # 方式1: 使用密码认证
        Write-Warning "如果提示输入密码，请输入服务器密码"
        ssh -o "PreferredAuthentications=password" -o "PubkeyAuthentication=no" root@$ServerIP "bash -s" < remote-deploy.sh
        
        if ($LASTEXITCODE -eq 0) {
            Write-Success "✅ 远程部署成功！"
            Write-Info "🌐 访问地址: http://${ServerIP}:8888"
        } else {
            Write-Warning "⚠️ SSH连接失败，请手动部署"
            Write-Info ""
            Write-Info "手动部署步骤："
            Write-Info "1. 登录服务器: ssh root@$ServerIP"
            Write-Info "2. 运行命令: curl -fsSL https://raw.githubusercontent.com/Guojin06/IoT_Water_Conservancy_Project/main/docker-deploy.sh | bash"
        }
    } catch {
        Write-Error "❌ 连接服务器失败: $_"
        Write-Info "请检查："
        Write-Info "1. 服务器IP地址是否正确"
        Write-Info "2. SSH端口是否开放 (默认22)"
        Write-Info "3. 用户名密码是否正确"
    }
}

# 主部署流程
function Main {
    # 检查依赖
    if (-not (Test-Docker)) { exit 1 }
    if (-not (Test-DockerCompose)) { exit 1 }
    if (-not (Test-Git)) { exit 1 }
    
    # 根据参数选择部署方式
    if ($ServerIP) {
        Deploy-ToServer -ServerIP $ServerIP
    } else {
        Setup-Project
        Setup-Environment  
        Start-Services
    }
    
    Write-Success "🎊 部署完成！欢迎使用智能水利系统！"
}

# 显示使用说明
if ($args.Contains("-h") -or $args.Contains("--help")) {
    Write-Info "智能水利系统 - Docker部署脚本"
    Write-Info ""
    Write-Info "用法:"
    Write-Info "  .\deploy-windows.ps1                    # 本地部署"
    Write-Info "  .\deploy-windows.ps1 -ServerIP x.x.x.x  # 远程部署"
    Write-Info "  .\deploy-windows.ps1 -Local             # 强制本地部署"
    Write-Info ""
    Write-Info "示例:"
    Write-Info "  .\deploy-windows.ps1 -ServerIP 8.134.136.216"
    exit 0
}

# 运行主函数
Main
