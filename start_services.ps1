# Set working directory
Set-Location -Path "D:\IoT_Water_Conservancy_Project\WaterConservancyIoT"

# Command to start the Data Producer
$producerCommand = "
. .\ai_service\ai_env\Scripts\Activate.ps1
python data_producer.py
"

# Command to start the WebSocket Server (Redis version)
$websocketCommand = "
. .\ai_service\ai_env\Scripts\Activate.ps1
python working_websocket.py
"

# Command to start the HTTP Server
$httpCommand = "
. .\ai_service\ai_env\Scripts\Activate.ps1
python simple_server.py
"

# Start all services in new PowerShell windows
Write-Host "Starting all services in new terminal windows..."
Start-Process powershell -ArgumentList "-NoExit", "-Command", $producerCommand
Start-Process powershell -ArgumentList "-NoExit", "-Command", $websocketCommand
Start-Process powershell -ArgumentList "-NoExit", "-Command", $httpCommand
Write-Host "✅ All three services (Producer, WebSocket, HTTP) have been launched."
