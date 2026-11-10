CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00
LDFLAGS = -lws2_32 -lwsock32

BUILD_DIR = build
TARGET = $(BUILD_DIR)/m3_nav.exe
TEST_TARGET = $(BUILD_DIR)/m3_test.exe

SRC = main.cpp \
      model/Location.cpp \
      structure/LocationNode.cpp \
      structure/RoadEdge.cpp \
      structure/RoadGraph.cpp \
      repository/LocationRepository.cpp \
      datasource/LocationCsvLoader.cpp \
      datasource/RoadNetworkCsvLoader.cpp \
      dto/RouteResponse.cpp \
      service/RoadGraphBuilder.cpp \
      service/ShortestPathFinder.cpp \
      service/NearbyServicesFinder.cpp \
      service/TransportEstimator.cpp \
      service/RestApiServer.cpp \
      util/GeoUtil.cpp

TEST_SRC = test/test_main.cpp \
           model/Location.cpp \
           structure/LocationNode.cpp \
           structure/RoadEdge.cpp \
           structure/RoadGraph.cpp \
           repository/LocationRepository.cpp \
           datasource/LocationCsvLoader.cpp \
           datasource/RoadNetworkCsvLoader.cpp \
           dto/RouteResponse.cpp \
           service/RoadGraphBuilder.cpp \
           service/ShortestPathFinder.cpp \
           service/NearbyServicesFinder.cpp \
           service/TransportEstimator.cpp \
           util/GeoUtil.cpp

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I. $(SRC) -o $(TARGET) $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I. $(TEST_SRC) -o $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

serve: $(TARGET)
	./$(TARGET) --serve

clean:
	rm -rf $(BUILD_DIR) *.o

.PHONY: all test run serve clean
