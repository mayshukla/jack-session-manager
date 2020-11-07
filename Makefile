# Based on the following sources:
#  https://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/
#  http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#traditional

CC = g++
HEADERDIR = include
CFLAGS = -std=c++14 -I$(HEADERDIR)
# std::thread requires libpthread
LDFLAGS = -ljack
SRCDIR = src
BUILDDIR = build
BUILDDIR_D = build_debug
TARGETDIR = bin
TARGET = $(TARGETDIR)/jack-session-manager

SRCEXT = cpp
# Find all regular files in source directory with extenstion SRCEXT
# must use shell since find is not a make function, but a shell program
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
# To get a list of object files, replace SRCDIR with BUILDDIR and replace
# SRCEXT with .o
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
OBJECTS_D = $(patsubst $(SRCDIR)/%,$(BUILDDIR_D)/%,$(SOURCES:.$(SRCEXT)=.o))

# Locations of generated dependency files
DEP = $(OBJECTS:%.o=%.d)
DEP += $(OBJECTS_D:%.o=%.d)

# Takes all .o files as prerequisites and links everything into TARGET
$(TARGET): $(OBJECTS) $(OBJECTS_D)
	mkdir -p $(TARGETDIR)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	$(CC) $(LDFLAGS) $(OBJECTS_D) -o $(TARGET)_debug

# "-" ensures that make won't throw an error if some .d files don't exist.
# On the first compile, the .d files have not been generated yet.
-include $(DEP)

# Use -MMD flag to generate .d file at the same time as compiling.
# This ensures that the next invocation of "make" will see an up-to-date .d
# file.
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	mkdir -p $(BUILDDIR)
	$(CC) -O -MMD $(CFLAGS) -c -o $@ $<

# compile with -g flag
$(BUILDDIR_D)/%.o: $(SRCDIR)/%.$(SRCEXT)
	mkdir -p $(BUILDDIR_D)
	$(CC) -g -MMD $(CFLAGS) -c -o $@ $<

clean:
	-rm -r $(BUILDDIR)
	-rm -r $(BUILDDIR_D)
	-rm -r $(TARGET)
	-rm -r $(TARGET)_debug
