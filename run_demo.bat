@echo off
title RESQ Dehradun - Standalone Demo Runner
echo ========================================================================
echo  RESQ DEHRADUN -- SMART EMERGENCY RESPONSE & ASSISTANCE SYSTEM
echo  Running Phase-2 Standalone Demonstrations & Interactive Routing Engine
echo ========================================================================
echo.

set PATH=C:\Users\Asus\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin;%PATH%

if not exist "m3_navigation\build\m3_nav.exe" (
    echo [INFO] Compiling Navigation Engine...
    cd m3_navigation
    g++ -std=c++17 -Wall -Wextra -O2 -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -I. main.cpp model/Location.cpp structure/LocationNode.cpp structure/RoadEdge.cpp structure/RoadGraph.cpp repository/LocationRepository.cpp datasource/LocationCsvLoader.cpp datasource/RoadNetworkCsvLoader.cpp dto/RouteResponse.cpp service/RoadGraphBuilder.cpp service/ShortestPathFinder.cpp service/NearbyServicesFinder.cpp service/TransportEstimator.cpp service/RestApiServer.cpp util/GeoUtil.cpp -o build/m3_nav.exe -lws2_32 -lwsock32
    cd ..
)

echo [INFO] Launching Demo Engine...
echo.
m3_navigation\build\m3_nav.exe
pause
