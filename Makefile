# Copyright 2020 by Julian Huch
# Autor: Julian Huch

# Target Name
TARGET 		:= editorProg

# Target directory
TARGET_DIR 	:= bin

# Compiler
CXX 		:= gcc
# Linker
LD 			:= gcc

# Language Standard
STAND 		:= -std=c11
# debug
DEBUG 		:= -g
# optimisation
OPT 		:= -O0
# Warnings
WARN 		:= -Wall -Wextra -Weffc++ -pedantic 
# Threading
PTHREAD 	:= -pthread
# Include
INCDIRS 	:= -I.

# Often used libaries.
#GTKLIB
GTKLIB 		:= `pkg-config --cflags --libs gtk+-3.0`
# GLIB
GLIB 		:= `pkg-config --cflags --libs glib-2.0`
# Curses
CURSES 		:= -lcurses

# Compiler flags
CCFLAGS 	:= $(OPT) $(WARN) $(PTHREAD) $(STAND) -pipe
# Linker flags
LDFLAGS 	:= $(PTHREAD) -export-dynamic

# Source Code
SRC 		:= $(wildcard *.c) $(wildcard **/*.c)
# Header
HEAD 		:= $(wildcard *.h) $(wildcard **/*.h)
# Object Code
OBJ 		:= $(patsubst %.c,%.o,$(SRC))
# Dependencies
DEP 		:= $(patsubst %.c,%.d,$(SRC))

# Phony Targets
.PHONY: all check checkstyle valgrind clean make_target_directory

# Clean and build the Target
all: make_target_directory $(TARGET)
	@echo $(TARGET) successfully compiled!
# Link object files
$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $(TARGET_DIR)/$@
# Compile object files from source files
%.o: %.c
	$(CXX) $(CCFLAGS) $(INCDIRS) -c $< -o $@
# Build dependencie files
%.d: %.c
	$(CXX) $(INCDIRS) -MM $< > $@
	
# make target directory
make_target_directory:
	mkdir -p $(TARGET_DIR)

# Check for style, Build the Target, Check for memory errors
check: clean checkstyle all valgrind
# Check for style with cpplinter
checkstyle:
	python2 ./cpplint.py $(SRC) $(HEAD) 
# Check for memory errors
valgrind:
	valgrind ./$(TARGET)

# Remove builded binary, object files and dependencie files
clean:
	rm -rf $(OBJ) $(DEP) $(TARGET) $(TARGET_DIR)

# Include depenendcies
-include $(DEP)

