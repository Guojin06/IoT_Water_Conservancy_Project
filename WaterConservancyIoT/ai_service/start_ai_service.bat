@echo off
chcp 65001 >nul
echo 🚀 启动AI智能分析服务...
echo.
echo 💡 免费AI服务商推荐：
echo   1. 百度千帆 - 1000次/日免费
echo   2. 阿里通义 - 500次/日免费  
echo   3. 本地模型 - 完全免费
echo.
echo 📖 配置指南请查看: FREE_AI_GUIDE.md
echo.
cd /d "%~dp0"
python app.py
pause
