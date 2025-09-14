@echo off
chcp 65001 >nul
echo 🤖 启动AI智能分析服务
echo ===============================================
cd /d "%~dp0ai_service"
call ai_env\Scripts\activate.bat
echo ✅ 虚拟环境已激活
echo 🚀 启动AI服务...
python app.py
pause
