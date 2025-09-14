@echo off
chcp 65001 >nul
echo 🚀 启动完整智能水利物联网系统
echo ===============================================
echo.

echo 📋 系统组件：
echo   🔥 C++核心系统 - 数据处理
echo   🐍 Python Web服务 - 前端界面  
echo   🤖 AI智能分析 - 百度千帆API
echo.

echo 🎯 第1步：测试C++核心系统
echo -----------------------------------------------
test_system.exe
if %errorlevel% neq 0 (
    echo ❌ C++系统测试失败
    pause
    exit /b 1
)
echo ✅ C++核心系统测试通过！
echo.

echo 🎯 第2步：启动Python Web服务
echo -----------------------------------------------
start "Web服务器" /min cmd /c "python simple_server.py"
echo ⏳ 等待Web服务启动...
timeout /t 3 /nobreak >nul
echo ✅ Web服务已启动 (http://localhost:8888)
echo.

echo 🎯 第3步：启动AI智能分析服务  
echo -----------------------------------------------
start "AI服务" /min cmd /c "cd ai_service && call ai_env\Scripts\activate.bat && python app.py"
echo ⏳ 等待AI服务启动...
timeout /t 5 /nobreak >nul
echo ✅ AI服务已启动 (http://localhost:5000)
echo.

echo 🎉 系统启动完成！
echo ===============================================
echo 📱 现在可以访问：
echo   🏠 主页面：http://localhost:8888
echo   📊 监控大屏：http://localhost:8888/dashboard.html
echo   🖥️ 智能中心：http://localhost:8888/tech-dashboard.html  
echo   🤖 AI分析：http://localhost:8888/ai-analysis.html
echo.
echo 🔧 管理接口：
echo   📈 系统状态：http://localhost:8888/api/status
echo   🤖 AI状态：http://localhost:5000/api/ai/status
echo.
echo 💡 提示：系统已在后台运行，关闭此窗口不影响服务
echo.

echo 🌐 自动打开主页面...
start http://localhost:8888
echo.
echo ✨ 享受您的智能水利物联网系统！
pause
