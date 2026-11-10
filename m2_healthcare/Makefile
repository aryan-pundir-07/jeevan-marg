CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC = main.cpp \
      model/BedAvailability.cpp \
      model/Doctor.cpp \
      model/Department.cpp \
      model/Hospital.cpp \
      repository/HospitalRepository.cpp \
      service/HealthcareService.cpp

TEST_SRC = test/test_m2.cpp \
           model/BedAvailability.cpp \
           model/Doctor.cpp \
           model/Department.cpp \
           model/Hospital.cpp \
           repository/HospitalRepository.cpp \
           service/HealthcareService.cpp

all: m2_healthcare.exe

m2_healthcare.exe: $(SRC)
	$(CXX) $(CXXFLAGS) -I. $(SRC) -o m2_healthcare.exe

test: m2_test.exe
	./m2_test.exe

m2_test.exe: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -I. $(TEST_SRC) -o m2_test.exe

clean:
	rm -f *.exe *.o

.PHONY: all test clean
