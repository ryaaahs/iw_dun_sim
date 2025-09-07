:: Run bat file within docker windows or windows vm

make clean
make

set FOLDER="iw_dun_sim_windows_build"
set ZIP_FILE="iw_dun_sim_windows_build.zip"
set JSON_FOLDER="json"

mkdir "%FOLDER%"
mkdir "%FOLDER%\%JSON_FOLDER%"
copy "%CD%\build\iw_dun_sim.exe" "%CD%\iw_dun_sim_windows_build"
copy "%CD%\ironwood_dungeon_simulation.bat" "%CD%\iw_dun_sim_windows_build"
copy "%CD%\json\simulation_values_template.json" "%CD%\iw_dun_sim_windows_build\json"

if exist "%ZIP_FILE%" (
    echo Deleting file %ZIP_FILE%...
    del /s /q "%ZIP_FILE%"
)

powershell -command "Compress-Archive -Path iw_dun_sim_windows_build\* -DestinationPath %ZIP_FILE% -Force"

if exist "%FOLDER%" (
    echo Deleting folder %FOLDER%...
    rmdir /s /q "%FOLDER%"
)
