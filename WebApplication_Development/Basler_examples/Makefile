PYLON_ROOT ?= /opt/pylon
CPPFLAGS   += $(shell $(PYLON_ROOT)/bin/pylon-config --cflags)
LDFLAGS    += $(shell $(PYLON_ROOT)/bin/pylon-config --libs-rpath)
LDLIBS     += $(shell $(PYLON_ROOT)/bin/pylon-config --libs)

# Makefile for Basler pylon sample program
.PHONY: all clean

# The program to build
NAME       := baslerTest

# Installation directories for pylon
PYLON_ROOT ?= /opt/pylon

# Build tools and flags
LD         := $(CXX)
CPPFLAGS   := $(shell $(PYLON_ROOT)/bin/pylon-config --cflags) #-DUSE_GIGE
CXXFLAGS   := #e.g., CXXFLAGS=-g -O0 for debugging
LDFLAGS    := $(shell $(PYLON_ROOT)/bin/pylon-config --libs-rpath)
LDLIBS     := $(shell $(PYLON_ROOT)/bin/pylon-config --libs)

# Rules for building
all: $(NAME)

$(NAME): $(NAME).o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(NAME).o: $(NAME).cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)-I/usr/include/opencv4 -g -c  -o $@ $< -lm -lpthread -lrt

clean:
	$(RM) $(NAME).o $(NAME)
