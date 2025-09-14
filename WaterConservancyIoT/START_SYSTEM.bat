@echo off
chcp 65001 >nul
echo 🚀 启动智能水利物联网监控系统
echo ===============================================
echo.

echo 📋 系统组件：
echo   🔥 C++核心系统 - 传感器数据处理
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

echo 🎯 第2步：启动Web服务器
echo -----------------------------------------------
start "Web服务器" cmd /k "python simple_server.py"
timeout /t 3 /nobreak >nul
echo ✅ Web服务器已启动 (http://localhost:8888)
echo.

echo 🎯 第3步：启动AI智能分析服务
echo -----------------------------------------------
cd ai_service
start "AI服务" cmd /k "ai_env\Scripts\activate && python app.py"
timeout /t 3 /nobreak >nul
echo ✅ AI服务已启动 (http://localhost:5000)
cd ..
echo.

echo 🎉 系统启动完成！
echo ===============================================
echo 📱 访问地址：
echo   🏠 主页面：http://localhost:8888
echo   📊 监控大屏：http://localhost:8888/dashboard.html
echo   🖥️ 智能中心：http://localhost:8888/tech-dashboard.html
echo   🤖 AI分析：http://localhost:8888/ai-analysis.html
echo.
echo 🔧 管理地址：
echo   📈 API状态：http://localhost:8888/api/status
echo   🤖 AI状态：http://localhost:5000/api/ai/status
echo.
echo ⚡ 系统特性：
echo   • C++高性能数据处理
echo   • 实时传感器数据模拟
echo   • 专业监控界面
echo   • 百度千帆AI智能分析
echo   • 响应式Web设计
echo.
echo 💡 提示：关闭此窗口不会停止服务
echo    要停止系统，请分别关闭各个服务窗口
echo.
start http://localhost:8888
pause
