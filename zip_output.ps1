# Execute Script
# 1. Run Powershell as Admin
# 2. set-executionpolicy remotesigned

copy .\x64\Static\_gui_injector.exe .\QT_GH_Injector\GH_Injector_x64.exe
copy .\Win32\Static\_gui_injector.exe .\QT_GH_Injector\GH_Injector_x86.exe
Compress-Archive .\QT_GH_Injector QT_GH_Injector.zip