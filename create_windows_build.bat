:: Run bat file within docker windows or windows vm

make clean
make

set "FOLDER=iw_dun_sim_windows_build"

mkdir "%FOLDER%"
copy "%CD%\build\iw_dun_sim.exe" "%CD%\iw_dun_sim_windows_build"
copy "%CD%\json\simulation_values_template.json" "%CD%\iw_dun_sim_windows_build"

powershell -command "Compress-Archive -Path iw_dun_sim_windows_build\* -DestinationPath iw_dun_sim_windows_build.zip -Force"

if exist "%FOLDER%" (
    echo Deleting folder %FOLDER%...
    rmdir /s /q "%FOLDER%"
)
