@echo off
title JEEVAN MARG - REST API Server & Frontend
echo ========================================================================
echo  JEEVAN MARG -- SMART EMERGENCY RESPONSE & ASSISTANCE SYSTEM
echo  Starting C++ REST API Server on http://localhost:8080
echo  Serving Frontend Dashboard (M4) with Live Map & Real-Time Tracking
echo ========================================================================
echo.

set PATH=C:\Users\Asus\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin;%PATH%

if not exist "m3_navigation\build\m3_nav.exe" (
    echo [INFO] Compiling Navigation Engine & REST API Server...
    cd m3_navigation
    g++ -std=c++17 -Wall -Wextra -O2 -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -I. main.cpp model/Location.cpp structure/LocationNode.cpp structure/RoadEdge.cpp structure/RoadGraph.cpp repository/LocationRepository.cpp datasource/LocationCsvLoader.cpp datasource/RoadNetworkCsvLoader.cpp dto/RouteResponse.cpp service/RoadGraphBuilder.cpp service/ShortestPathFinder.cpp service/NearbyServicesFinder.cpp service/TransportEstimator.cpp service/RestApiServer.cpp util/GeoUtil.cpp -o build/m3_nav.exe -lws2_32 -lwsock32
    cd ..
)

echo [INFO] Launching Browser Dashboard...
start "" "http://localhost:8080/index.html"

echo [INFO] Starting Backend HTTP Server...
m3_navigation\build\m3_nav.exe --serve
pause
