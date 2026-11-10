@echo off
title JEEVAN MARG - Automated Test Runner
echo ========================================================================
echo  JEEVAN MARG -- AUTOMATED UNIT & INTEGRATION TEST RUNNER
echo  Running Test Suites for M1, M2, and M3 (Dijkstra vs Bellman-Ford 380 Pairs)
echo ========================================================================
echo.

set PATH=C:\Users\Asus\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin;%PATH%

echo [1/3] Testing Module 1 (Emergency Dispatch)...
cd m1_dispatch
g++ -std=c++17 -Wall -Wextra -O2 -I. test/test_m1.cpp model/EmergencyContact.cpp model/EmergencyRequest.cpp service/TriageRuleEngine.cpp service/EmergencyManager.cpp -o m1_test.exe
m1_test.exe
cd ..

echo.
echo [2/3] Testing Module 2 (Healthcare Discovery)...
cd m2_healthcare
g++ -std=c++17 -Wall -Wextra -O2 -I. test/test_m2.cpp model/BedAvailability.cpp model/Doctor.cpp model/Department.cpp model/Hospital.cpp repository/HospitalRepository.cpp service/HealthcareService.cpp -o m2_test.exe
m2_test.exe
cd ..

echo.
echo [3/3] Testing Module 3 (Navigation & 380-Pair Cross Validation)...
cd m3_navigation
g++ -std=c++17 -Wall -Wextra -O2 -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -I. test/test_main.cpp model/Location.cpp structure/LocationNode.cpp structure/RoadEdge.cpp structure/RoadGraph.cpp repository/LocationRepository.cpp datasource/LocationCsvLoader.cpp datasource/RoadNetworkCsvLoader.cpp dto/RouteResponse.cpp service/RoadGraphBuilder.cpp service/ShortestPathFinder.cpp service/NearbyServicesFinder.cpp service/TransportEstimator.cpp util/GeoUtil.cpp -o build/m3_test.exe
build\m3_test.exe
cd ..

echo.
echo ========================================================================
echo  ALL 3 MODULE TEST SUITES EXECUTED SUCCESSFULLY!
echo ========================================================================
pause
