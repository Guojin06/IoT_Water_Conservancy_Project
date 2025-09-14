#!/bin/bash

# 智能水库监测系统构建脚本
# 使用方法: ./scripts/build.sh [Debug|Release]

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的信息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查依赖
check_dependencies() {
    print_info "检查构建依赖..."
    
    # 检查CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake 未安装，请先安装 CMake 3.16+"
        exit 1
    fi
    
    CMAKE_VERSION=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+')
    print_info "CMake 版本: $CMAKE_VERSION"
    
    # 检查编译器
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n1 | grep -oE '[0-9]+\.[0-9]+')
        print_info "GCC 版本: $GCC_VERSION"
    elif command -v clang++ &> /dev/null; then
        CLANG_VERSION=$(clang++ --version | head -n1 | grep -oE '[0-9]+\.[0-9]+')
        print_info "Clang 版本: $CLANG_VERSION"
    else
        print_error "未找到 C++ 编译器 (g++ 或 clang++)"
        exit 1
    fi
    
    # 检查pkg-config (用于查找Redis库)
    if ! command -v pkg-config &> /dev/null; then
        print_warning "pkg-config 未安装，可能无法自动检测Redis库"
    fi
    
    print_success "依赖检查完成"
}

# 创建必要的目录
create_directories() {
    print_info "创建项目目录..."
    
    mkdir -p build
    mkdir -p bin
    mkdir -p lib
    mkdir -p logs
    mkdir -p external
    
    print_success "目录创建完成"
}

# 下载依赖
download_dependencies() {
    print_info "检查并下载第三方依赖..."
    
    # 检查网络连接
    if ! ping -c 1 github.com &> /dev/null; then
        print_warning "无法连接到GitHub，将跳过自动下载"
        return
    fi
    
    print_success "依赖检查完成"
}

# 构建项目
build_project() {
    local BUILD_TYPE=${1:-Release}
    
    print_info "开始构建项目 (构建类型: $BUILD_TYPE)..."
    
    cd build
    
    # 生成构建文件
    print_info "生成CMake构建文件..."
    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
          -DBUILD_TESTING=ON \
          ..
    
    # 编译项目
    print_info "编译项目..."
    if command -v nproc &> /dev/null; then
        JOBS=$(nproc)
    else
        JOBS=4
    fi
    
    make -j$JOBS
    
    cd ..
    
    print_success "项目构建完成！"
}

# 运行测试
run_tests() {
    print_info "运行单元测试..."
    
    cd build
    if [ -f "bin/water_iot_tests" ]; then
        ./bin/water_iot_tests
        print_success "所有测试通过！"
    else
        print_warning "未找到测试可执行文件，跳过测试"
    fi
    cd ..
}

# 显示构建信息
show_build_info() {
    print_info "构建信息:"
    echo "  项目根目录: $(pwd)"
    echo "  构建目录: $(pwd)/build"
    echo "  可执行文件: $(pwd)/build/bin/water_iot_server"
    echo "  配置文件: $(pwd)/config/"
    echo "  日志目录: $(pwd)/logs/"
    echo ""
    print_info "启动命令:"
    echo "  cd $(pwd)"
    echo "  ./build/bin/water_iot_server"
}

# 清理构建
clean_build() {
    print_info "清理构建文件..."
    rm -rf build bin lib
    print_success "清理完成"
}

# 主函数
main() {
    print_info "=== 智能水库监测系统构建脚本 ==="
    
    # 解析命令行参数
    BUILD_TYPE="Release"
    CLEAN_BUILD=false
    RUN_TESTS=true
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            Debug|debug)
                BUILD_TYPE="Debug"
                shift
                ;;
            Release|release)
                BUILD_TYPE="Release"
                shift
                ;;
            --clean)
                CLEAN_BUILD=true
                shift
                ;;
            --no-tests)
                RUN_TESTS=false
                shift
                ;;
            --help|-h)
                echo "使用方法: $0 [Debug|Release] [--clean] [--no-tests]"
                echo "  Debug|Release  : 构建类型 (默认: Release)"
                echo "  --clean        : 清理后重新构建"
                echo "  --no-tests     : 不运行测试"
                exit 0
                ;;
            *)
                print_error "未知参数: $1"
                exit 1
                ;;
        esac
    done
    
    # 执行构建步骤
    if [ "$CLEAN_BUILD" = true ]; then
        clean_build
    fi
    
    check_dependencies
    create_directories
    download_dependencies
    build_project $BUILD_TYPE
    
    if [ "$RUN_TESTS" = true ]; then
        run_tests
    fi
    
    show_build_info
    
    print_success "=== 构建完成！ ==="
}

# 运行主函数
main "$@"
