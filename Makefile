CXX=g++
C=gcc
STD=c++2a

CXXFLAGS=-std=$(STD) -DLOCAL_ -I$(INCLUDE_DIR) -m64 -Wunreachable-code
CFLAGS=
LDFLAGS=

INCLUDE_DIR=include
SRCDIR=src
OBJDIR=obj
DEPDIR=dep
DEBUG_OBJDIR=debug/obj
SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*.c)
SOURCES_NO_DIR_ = $(SOURCES:$(SRCDIR)/%=%)
SOURCES_RENAMED_C = $(SOURCES_NO_DIR_:.c=_c)
SOURCES_RENAMED_CPP = $(SOURCES_RENAMED_C:.cpp=_cpp)


OBJECTS = $(SOURCES_RENAMED_CPP:%=$(OBJDIR)/%.o)
DEBUG_OBJECTS = $(OBJECTS:%=debug/%)

SOURCES_DEPS = $(SOURCES_RENAMED_CPP:%=$(DEPDIR)/%.d)
SOURCES_DEBUG_DEPS = $(SOURCES_DEPS:%=debug/%)

BINARY_=exe
MK_DEBUG_OBJ_DIR=mkdir
MK_DEBUG_DEP_DIR=mkdir

ifeq ($(OS),Windows_NT)
	MK_DEBUG_OBJ_DIR += debug\\obj
	MK_DEBUG_DEP_DIR += debug\\dep
	BINARY=$(BINARY_:%=%.exe)
else
	MK_DEBUG_OBJ_DIR +=-p debug/obj
	MK_DEBUG_DEP_DIR +=-p debug/dep
	BINARY=BINARY_
endif

DEBUG_BINARY=$(BINARY:%=debug/%)

all: prepare $(BINARY)

debug: prepare_debug $(DEBUG_BINARY)
	echo $(DEBUG_BINARY)
clean:

$(DEPDIR)/.exists:
	mkdir $(DEPDIR) || echo "exists"
	echo 1 > $(DEPDIR)/.exists

$(OBJDIR)/.exists:
	mkdir $(OBJDIR) || echo "exists"
	echo 1 > $(OBJDIR)/.exists

debug/.exists:
	mkdir debug
	echo 1 > debug/.exists

debug/$(OBJDIR)/.exists: debug/.exists
	$(MK_DEBUG_OBJ_DIR)
	echo 2 > debug/$(OBJDIR)/.exists

debug/$(DEPDIR)/.exists: debug/.exists
	$(MK_DEBUG_DEP_DIR)
	echo 2 > debug/$(DEPDIR)/.exists

prepare: $(OBJDIR)/.exists  $(DEPDIR)/.exists
prepare_debug: debug/$(DEPDIR)/.exists debug/$(OBJDIR)/.exists $(DEPDIR)/.exists

$(BINARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(BINARY)
$(DEBUG_BINARY): $(DEBUG_OBJECTS)
	$(CXX) -g $(DEBUG_OBJECTS) -o $(DEBUG_BINARY)

$(OBJDIR)/%_c.o: $(SRCDIR)/%.c
	$(C) $(CFLAGS) -c -Werror -pedantic -I$(INCLUDE_DIR) $(SRCDIR)/$*.c -o $(OBJDIR)/$*_c.o -MMD -MF $(DEPDIR)/$*_c.d

$(OBJDIR)/%_cpp.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -Werror -pedantic -I$(INCLUDE_DIR) $(SRCDIR)/$*.cpp -o $(OBJDIR)/$*_cpp.o -MMD -MF $(DEPDIR)/$*_cpp.d

debug/$(OBJDIR)/%_c.o: $(SRCDIR)/%.c
	$(C) $(CFLAGS) -g -c -Werror -pedantic -I$(INCLUDE_DIR) $(SRCDIR)/$*.c -o debug/$(OBJDIR)/$*_c.o -MMD -MF debug/$(DEPDIR)/$*_c.d

debug/$(OBJDIR)/%_cpp.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -g -c -Werror -pedantic -I$(INCLUDE_DIR) $(SRCDIR)/$*.cpp -o debug/$(OBJDIR)/$*_cpp.o -MMD -MF debug/$(DEPDIR)/$*_cpp.d

clean:
	rm -r $(OBJDIR) || echo 1
	rm -r $(DEPDIR) || echo 1
	rm -r debug || echo 1
	rm -r preprocessor_inc || echo 1
	rm -r preprocessor_src || echo 1
	rm $(BINARY) || echo 1
	rm out.cpp || echo 1

HEADERS_ = $(wildcard $(INCLUDE_DIR)/*.h)
HEADERS_INC = $(HEADERS_:$(INCLUDE_DIR)%=preprocessor_inc/%)
SRC_INC = $(SOURCES:$(SRCDIR)/%=preprocessor_src/%)
SRC_INC_INCL = $(SRC_INC:preprocessor_src/%=preprocessor_src\\\\\\/%)

preprocessor_inc/%: $(INCLUDE_DIR)/%
	grep -E -v "^#include <[A-z_]+?>" $(INCLUDE_DIR)/$* | grep -E -v "^#include <stdint.h>" | grep -E -v "^#include <stddef.h>" > preprocessor_inc/$*

preprocessor_src/%: $(SRCDIR)/%
	grep -E -v "^#include <[A-z_]+?>" $(SRCDIR)/$* | grep -E -v "^#include <stdint.h>" | grep -E -v "^#include <stddef.h>" > preprocessor_src/$*

preprocessor_inc/.exists:
	mkdir preprocessor_inc
	echo 1 > preprocessor_inc/.exists
preprocessor_src/.exists:
	mkdir preprocessor_src
	echo 1 > preprocessor_src/.exists

preprocessor_src/main_.cpp: preprocessor_inc/.exists preprocessor_src/.exists $(HEADERS_INC) $(SRC_INC)
	sed -E "s/####/$(shell echo $(SRC_INC_INCL) | sed -E "s/ /\n/g" | grep -v main.cpp | xargs) /g" preprocessor_src/main.cpp > preprocessor_src/main_.cpp
	sed -i -E "s/.cpp preprocessor_src/.cpp\npreprocessor_src/g" preprocessor_src/main_.cpp
	sed -i -E "s/preprocessor_src\/(.+?)/#include <\1>/g" preprocessor_src/main_.cpp
	sed -i -E "s/ >/>/g" preprocessor_src/main_.cpp
out.cpp: preprocessor_src/main_.cpp
	cat $(HEADERS_) $(SOURCES) | grep -E "^#include <[A-z_]+>" > out.cpp
	$(CXX) -E -std=$(STD) -Ipreprocessor_inc -Ipreprocessor_src -DPREPROCESSOR_ -nostdinc preprocessor_src/main_.cpp -undef | grep -E -v "^# " | grep -E "[^\s]+" >> out.cpp
source: out.cpp
	

-include $(SOURCES_DEPS) $(SOURCES_DEBUG_DEPS)