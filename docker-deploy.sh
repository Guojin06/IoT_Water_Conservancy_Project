#!/bin/bash

# 智能水利系统 - Docker一键部署脚本
# 适用于Ubuntu/CentOS/Debian等Linux系统

set -e  # 遇到错误立即退出

echo "🚀 智能水利系统 - Docker一键部署开始..."
echo "================================================"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 检查是否为root用户
check_root() {
    if [[ $EUID -eq 0 ]]; then
        echo -e "${YELLOW}⚠️  检测到root用户，建议创建普通用户运行${NC}"
        read -p "是否继续? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
}

# 检查系统类型
detect_os() {
    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        OS=$NAME
        VER=$VERSION_ID
    else
        echo -e "${RED}❌ 无法检测操作系统类型${NC}"
        exit 1
    fi
    echo -e "${GREEN}✅ 检测到系统: $OS $VER${NC}"
}

# 安装Docker
install_docker() {
    if command -v docker &> /dev/null; then
        echo -e "${GREEN}✅ Docker已安装: $(docker --version)${NC}"
    else
        echo -e "${YELLOW}📦 正在安装Docker...${NC}"
        
        # 安装Docker官方脚本
        curl -fsSL https://get.docker.com -o get-docker.sh
        sudo sh get-docker.sh
        
        # 启动Docker服务
        sudo systemctl start docker
        sudo systemctl enable docker
        
        # 添加当前用户到docker组
        sudo usermod -aG docker $USER
        
        echo -e "${GREEN}✅ Docker安装完成${NC}"
        echo -e "${YELLOW}⚠️  请重新登录以应用用户组更改${NC}"
    fi
}

# 安装Docker Compose
install_docker_compose() {
    if command -v docker-compose &> /dev/null; then
        echo -e "${GREEN}✅ Docker Compose已安装: $(docker-compose --version)${NC}"
    else
        echo -e "${YELLOW}📦 正在安装Docker Compose...${NC}"
        
        # 下载最新版本的Docker Compose
        COMPOSE_VERSION=$(curl -s https://api.github.com/repos/docker/compose/releases/latest | grep 'tag_name' | cut -d\" -f4)
        sudo curl -L "https://github.com/docker/compose/releases/download/${COMPOSE_VERSION}/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
        sudo chmod +x /usr/local/bin/docker-compose
        
        echo -e "${GREEN}✅ Docker Compose安装完成${NC}"
    fi
}

# 检查Git
check_git() {
    if ! command -v git &> /dev/null; then
        echo -e "${YELLOW}📦 正在安装Git...${NC}"
        if [[ "$OS" == *"Ubuntu"* ]] || [[ "$OS" == *"Debian"* ]]; then
            sudo apt-get update && sudo apt-get install -y git
        elif [[ "$OS" == *"CentOS"* ]] || [[ "$OS" == *"Red Hat"* ]]; then
            sudo yum install -y git
        fi
    fi
    echo -e "${GREEN}✅ Git已就绪${NC}"
}

# 克隆或更新项目
setup_project() {
    PROJECT_DIR="IoT_Water_Conservancy_Project"
    
    if [[ -d "$PROJECT_DIR" ]]; then
        echo -e "${YELLOW}📁 项目目录已存在，正在更新...${NC}"
        cd $PROJECT_DIR
        git pull origin main
    else
        echo -e "${YELLOW}📥 正在克隆项目...${NC}"
        git clone https://github.com/Guojin06/IoT_Water_Conservancy_Project.git
        cd $PROJECT_DIR
    fi
    
    echo -e "${GREEN}✅ 项目代码准备完成${NC}"
}

# 配置环境变量
setup_environment() {
    echo -e "${YELLOW}🔧 配置环境变量...${NC}"
    
    # 创建环境变量文件
    cat > .env << EOF
# 数据库配置
MYSQL_ROOT_PASSWORD=water_conservancy_2024
MYSQL_DATABASE=sensordatabase
MYSQL_USER=water_user
MYSQL_PASSWORD=water_user_2024

# Redis配置
REDIS_PASSWORD=redis_2024

# 应用配置
APP_ENV=production
JWT_SECRET_KEY=$(openssl rand -hex 32)
DEBUG=false

# 网络配置
HTTP_PORT=8888
WEBSOCKET_PORT=8081
EOF
    
    echo -e "${GREEN}✅ 环境配置完成${NC}"
}

# 启动服务
start_services() {
    echo -e "${YELLOW}🚀 启动服务...${NC}"
    
    # 构建并启动所有服务
    docker-compose up -d --build
    
    echo -e "${GREEN}✅ 服务启动完成${NC}"
    echo ""
    echo "🎉 部署成功！"
    echo "================================================"
    echo -e "${BLUE}📊 访问地址:${NC}"
    echo -e "   主页面: http://$(curl -s ifconfig.me):8888"
    echo -e "   本地访问: http://localhost:8888"
    echo ""
    echo -e "${BLUE}👤 默认登录:${NC}"
    echo -e "   用户名: admin"
    echo -e "   密码: admin123"
    echo ""
    echo -e "${BLUE}🔧 管理命令:${NC}"
    echo -e "   查看日志: docker-compose logs -f"
    echo -e "   停止服务: docker-compose down"
    echo -e "   重启服务: docker-compose restart"
    echo -e "   查看状态: docker-compose ps"
}

# 主要部署流程
main() {
    echo -e "${BLUE}开始部署智能水利系统...${NC}"
    
    check_root
    detect_os
    check_git
    install_docker
    install_docker_compose
    setup_project
    setup_environment
    start_services
    
    echo -e "${GREEN}🎊 部署完成！欢迎使用智能水利系统！${NC}"
}

# 运行主函数
main "$@"
