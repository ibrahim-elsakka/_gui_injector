# Execute Script
# 1. Run Powershell as Admin
# 2. set-executionpolicy remotesigned

copy .\x64\Static\_gui_injector.exe .\QT_GH_Injector
Compress-Archive .\QT_GH_Injector QT_GH_Injector.zip