CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic
CXXFLAGS += -I ../include

SOURCES = unit.cpp \
          unit-assignment.cpp \
          unit-casting.cpp \
          unit-constructor.cpp \
          unit-operators.cpp
          
OBJECTS = $(SOURCES:.cpp=.o)

all: unit_tests

unit_tests: $(OBJECTS) ../include/lazy/Lazy.hpp catch.hpp
	@echo "[CXXLD] $@"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp ../include/lazy/Lazy.hpp catch.hpp
	@echo "[CXX] $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -fr unit_tests $(OBJECTS)
