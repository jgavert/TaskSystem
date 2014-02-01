DIRS := src
PROGRAM_DIRS := $(DIRS)

WARNINGS := -pedantic -Wall -Wextra

CXX      := g++
CXXFLAGS := $(WARNINGS) -std=c++11 -Ofast

LIBS := -pthread -Wl,--no-as-needed

PROGRAM = bin/main

SRC := src
OBJ := obj
BIN := bin

all: dirs $(PROGRAM)

debug: CXXFLAGS += -O0 -g3
debug: LIBS += -g
debug: all

prof: CXXFLAGS += -pg
prof: LIBS += -pg
prof: all

SRC_PROGRAM := $(foreach dir, $(PROGRAM_DIRS), $(wildcard $(dir)/*.cpp))

OBJ_PROGRAM := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRC_PROGRAM))

DEPFILES := $(OBJ_PROGRAM:.o=.d)
DEPFILES := $(sort $(DEPFILES))

.PHONY: all clean dirs echo

all: dirs $(PROGRAM)

$(PROGRAM): $(OBJ_PROGRAM)

$(BIN)/%:
	@echo LINK $@
	@$(CXX) $^ $(LIBS) -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	@echo CXX $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@$(CXX) $(CXXFLAGS) -MM -MT "$(@:.d=.o) $@" $< > $(@:.o=.d)

clean:
	@$(RM) -rf $(PROGRAM) $(OBJ)

dirs:
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)
	@mkdir -p $(patsubst $(SRC)/%, $(OBJ)/%, $(shell find $(PROGRAM_DIRS) -type d))

ifneq ($(MAKECMDGOALS),clean)
  -include $(DEPFILES)
endif

