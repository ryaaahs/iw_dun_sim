@echo off
set SIMULATION=".\json\simulation_values.json"
set MARKET_PARSE=1
set DISPLAY_TYPE=100
powershell -command .\iw_dun_sim.exe %SIMULATION% %MARKET_PARSE% %DISPLAY_TYPE%
pause
