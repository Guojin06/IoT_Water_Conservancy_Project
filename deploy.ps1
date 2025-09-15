# 智能水利系统 - Windows 一键部署脚本
# 作者: AI Assistant
# 版本: 1.0.0

Write-Host "🚀 智能水利系统 - 一键部署脚本" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan

# 设置错误处理
$ErrorActionPreference = "Stop"

# 检查是否为管理员权限
$isAdmin = [bool](([System.Security.Principal.WindowsIdentity]::GetCurrent()).groups -match "S-1-5-32-544")
if (-not $isAdmin) {
    Write-Host "⚠️  建议以管理员权限运行此脚本" -ForegroundColor Yellow
}

# 检查系统要求
Write-Host "`n📋 检查系统要求..." -ForegroundColor Green

# 检查Python
try {
    $pythonVersion = python --version 2>&1
    Write-Host "✅ Python: $pythonVersion" -ForegroundColor Green
} catch {
    Write-Host "❌ Python未安装。请先安装Python 3.9+: https://www.python.org/" -ForegroundColor Red
    exit 1
}

# 检查Git
try {
    $gitVersion = git --version 2>&1
    Write-Host "✅ Git: $gitVersion" -ForegroundColor Green
} catch {
    Write-Host "❌ Git未安装。请先安装Git: https://git-scm.com/" -ForegroundColor Red
    exit 1
}

# 检查MySQL
$mysqlRunning = Get-Service -Name "mysql*" -ErrorAction SilentlyContinue | Where-Object {$_.Status -eq "Running"}
if ($mysqlRunning) {
    Write-Host "✅ MySQL服务正在运行" -ForegroundColor Green
} else {
    Write-Host "⚠️  MySQL服务未运行。请确保MySQL已安装并启动" -ForegroundColor Yellow
}

# 检查Redis (可选)
try {
    $redisTest = redis-cli ping 2>&1
    if ($redisTest -eq "PONG") {
        Write-Host "✅ Redis服务正在运行" -ForegroundColor Green
    } else {
        Write-Host "⚠️  Redis未运行，将尝试启动" -ForegroundColor Yellow
    }
} catch {
    Write-Host "⚠️  Redis未安装。请手动安装Redis或使用内存缓存模式" -ForegroundColor Yellow
}

Write-Host "`n🔧 开始部署..." -ForegroundColor Green

# 进入项目目录
Set-Location -Path "WaterConservancyIoT"

# 创建虚拟环境
Write-Host "📦 创建Python虚拟环境..." -ForegroundColor Blue
if (Test-Path "ai_service\ai_env") {
    Write-Host "虚拟环境已存在，跳过创建" -ForegroundColor Yellow
} else {
    python -m venv ai_service\ai_env
    Write-Host "✅ 虚拟环境创建完成" -ForegroundColor Green
}

# 激活虚拟环境并安装依赖
Write-Host "📦 安装Python依赖..." -ForegroundColor Blue
& .\ai_service\ai_env\Scripts\Activate.ps1
python -m pip install --upgrade pip
pip install redis mysql-connector-python websockets aioredis bcrypt PyJWT

Write-Host "✅ Python依赖安装完成" -ForegroundColor Green

# 数据库设置
Write-Host "`n🗄️  配置数据库..." -ForegroundColor Blue

$dbPassword = Read-Host "请输入MySQL root密码"
try {
    mysql -u root -p$dbPassword -e "SELECT 1;" 2>&1 | Out-Null
    Write-Host "✅ 数据库连接成功" -ForegroundColor Green
    
    # 创建数据库
    Write-Host "创建数据库和表结构..." -ForegroundColor Blue
    mysql -u root -p$dbPassword < create_database.sql
    Write-Host "✅ 数据库初始化完成" -ForegroundColor Green
    
} catch {
    Write-Host "❌ 数据库连接失败，请检查MySQL服务和密码" -ForegroundColor Red
    Write-Host "可以稍后手动执行: mysql -u root -p < create_database.sql" -ForegroundColor Yellow
}

# 更新配置文件中的数据库密码
Write-Host "更新配置文件..." -ForegroundColor Blue
$configFiles = @("mysql_client.py", "data_producer.py", "simple_server.py")
foreach ($file in $configFiles) {
    if (Test-Path $file) {
        (Get-Content $file) -replace "password='123456'", "password='$dbPassword'" | Set-Content $file
    }
}

# 创建启动脚本快捷方式
Write-Host "`n🔗 创建启动快捷方式..." -ForegroundColor Blue
$desktopPath = [Environment]::GetFolderPath("Desktop")
$shortcutPath = "$desktopPath\智能水利系统.lnk"

$shell = New-Object -ComObject WScript.Shell
$shortcut = $shell.CreateShortcut($shortcutPath)
$shortcut.TargetPath = "powershell.exe"
$shortcut.Arguments = "-ExecutionPolicy Bypass -File `"$(Get-Location)\start_services.ps1`""
$shortcut.WorkingDirectory = Get-Location
$shortcut.IconLocation = "shell32.dll,23"
$shortcut.Description = "启动智能水利系统"
$shortcut.Save()

Write-Host "✅ 桌面快捷方式已创建" -ForegroundColor Green

# 完成部署
Write-Host "`n🎉 部署完成！" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "🌐 访问地址: http://localhost:8888" -ForegroundColor Yellow
Write-Host "👤 默认账户: admin / admin123" -ForegroundColor Yellow
Write-Host "" 
Write-Host "启动系统:" -ForegroundColor White
Write-Host "1. 双击桌面 '智能水利系统' 快捷方式" -ForegroundColor White
Write-Host "2. 或运行: .\start_services.ps1" -ForegroundColor White
Write-Host ""
Write-Host "📖 更多信息请查看 DEPLOY.md" -ForegroundColor Cyan

# 询问是否立即启动
$startNow = Read-Host "是否立即启动系统? (y/N)"
if ($startNow -eq "y" -or $startNow -eq "Y") {
    Write-Host "`n🚀 启动系统..." -ForegroundColor Green
    & .\start_services.ps1
} else {
    Write-Host "`n📝 系统已准备就绪，随时可以启动！" -ForegroundColor Green
}
