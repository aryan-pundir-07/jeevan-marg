CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC = main.cpp \
      model/EmergencyContact.cpp \
      model/EmergencyRequest.cpp \
      service/TriageRuleEngine.cpp \
      service/EmergencyManager.cpp

TEST_SRC = test/test_m1.cpp \
           model/EmergencyContact.cpp \
           model/EmergencyRequest.cpp \
           service/TriageRuleEngine.cpp \
           service/EmergencyManager.cpp

all: m1_dispatch.exe

m1_dispatch.exe: $(SRC)
	$(CXX) $(CXXFLAGS) -I. $(SRC) -o m1_dispatch.exe

test: m1_test.exe
	./m1_test.exe

m1_test.exe: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -I. $(TEST_SRC) -o m1_test.exe

clean:
	rm -f *.exe *.o

.PHONY: all test clean
