CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic -Weffc++ -Wcast-align -Wcast-qual -Wno-ctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch -Wundef -Wno-unused -Wnon-virtual-dtor -Wreorder -Wdeprecated -Wno-keyword-macro -Wno-float-equal
CXXFLAGS += -I ../include

SOURCES = unit.cpp \
          unit-assignment.cpp \
          unit-casting.cpp \
          unit-constructor.cpp \
          unit-operators.cpp
          
OBJECTS = $(SOURCES:.cpp=.o)

all: unit_tests

unit_tests: $(OBJECTS) ../include/Lazy.hpp catch.hpp
	@echo "[CXXLD] $@"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp ../include/Lazy.hpp catch.hpp
	@echo "[CXX] $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -fr unit_tests $(OBJECTS)
