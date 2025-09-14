@echo off
chcp 65001 >nul
echo 🤖 AI智能分析模块环境设置
echo.
echo 💡 注意：这是可选的AI扩展模块，主项目是C++系统
echo.

if not exist ai_env (
    echo 📦 创建虚拟环境...
    python -m venv ai_env
    echo ✅ 虚拟环境创建完成
) else (
    echo ✅ 虚拟环境已存在
)

echo.
echo 🔧 激活虚拟环境并安装依赖...
call ai_env\Scripts\activate.bat
pip install flask flask-cors requests
echo.
echo ✅ 依赖安装完成！
echo.
echo 🚀 启动AI服务...
python app.py
pause
