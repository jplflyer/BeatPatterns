#======================================================================
# GNU MAKE REQUIRED!
#
# This Makefile covers Vitac-authored programs used to talk to the
# dialogic modem bank. It requires GNU Make.
#
# Possible targets:
#
#		make clean
#		make
#
# With luck, to add programs, you only need to do two things:
#
# 1. Add them to the PROGRAMS variable here at the very very top.
# 2. Define a rule near the bottom.
#
# Notice VPATH. This is a GNU Make thing that lets us put our source
# files where we want. I've broken them based on where they're running.
# I may later break them up even more for one directory for each program.
# Anyway, we search for .cpp files in the list of directories, and it
# just magically works.
#======================================================================

UNAME := $(shell uname)
MACAPPEND=
ifeq ($(UNAME), Darwin)
    MACAPPEND=-mac
	CXX=clang++
endif

SRCDIR := src
OBJDIR := obj${MACAPPEND}
DEPDIR := .d
TEST_SRC=test

SP_SRC_DIR=${SRCDIR}/showpage
SE_SRC_DIR=${SRCDIR}/song_editor

VPATH := ${SRCDIR}:${SP_SRC_DIR}:${SE_SRC_DIR}:${TEST_SRC}
CXXFLAGS := -I/usr/local/include -I./include -Isrc -std=c++14 -g -Wno-unused-local-typedefs -Wno-deprecated-declarations
LDFLAGS_MIN := -lpthread -lstdc++ -lm -ldl
LDFLAGS := -L. -L./lib -lsfml-audio -lsfml-system -lz -llog4cplus -lcrossguid -lcppunit -lboost_filesystem -lboost_system -lboost_thread -luuid -lpthread -lstdc++ -lm -ldl

SRCS = $(shell find . -name "*.cpp" -print)
SRCS_NODIR = $(notdir ${SRCS})

# $(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -Isrc $(TARGET_ARCH) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -Isrc $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SHOWPAGE_SRC := $(wildcard ${SP_SRC_DIR}/*.cpp)
SONGEDITOR_SRC := $(wildcard ${SE_SRC_DIR}/*.cpp)
LIB_OBJ := $(patsubst ${SP_SRC_DIR}/%.cpp,${OBJDIR}/%.o,${SHOWPAGE_SRC}) $(patsubst ${SE_SRC_DIR}/%.cpp,${OBJDIR}/%.o,${SONGEDITOR_SRC})

LIBNAME=songeditor${MACAPPEND}
LIB=lib/libsongeditor${MACAPPEND}.a
LIB_ARGS= -cvrU
ifeq ($(UNAME), Darwin)
	LIB_ARGS= -cvr
endif

INSTALL_BASE=/usr/local

PROGRAMS=SongEditor

#======================================================================
# Top-level targets.
#======================================================================
.PHONY: all
all: directories ${LIB} ${PROGRAMS}

Makefile: ;

# Clean the contents of the subdirs.
.PHONY: clean
clean:
	rm -f ${DEPDIR}/* ${OBJDIR}/* ${LIB}

# Clean out the subdirs entirely.
.PHONY: reallyclean
reallyclean:
	rm -rf ${DEPDIR} ${OBJDIR}

.PHONY: echo
echo:
	@echo CXXFLAGS is ${CXXFLAGS}
	@echo LIB_OBJ is ${LIB_OBJ}
	@echo VPATH = ${VPATH}

#======================================================================
# Build the directories needed.
# You shouldn't need to invoke these directly.
#======================================================================
.PHONY: directories
directories: lib ${DEPDIR} ${OBJDIR}

lib:
	mkdir -p $@

${DEPDIR}:
	mkdir -p $@

${OBJDIR}:
	mkdir -p $@

#======================================================================
# How to make the .o files as. This is a little complicated but is
# derived from:
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
#======================================================================

${OBJDIR}/%.o : %.cpp
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

INCLUDELIST = $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS_NODIR))))
include ${INCLUDELIST}

#======================================================================
# Making the library.
#======================================================================
${LIB}: ${LIB_OBJ}
	ar ${LIB_ARGS} ${LIB} ${LIB_OBJ}
	ranlib ${LIB}

#======================================================================
# The CLI
#======================================================================
SongEditor: ${OBJDIR}/CLI-Main.o ${LIB}
	$(CXX) ${OBJDIR}/CLI-Main.o -L. -l${LIBNAME} ${LDFLAGS} $(OUTPUT_OPTION)

#======================================================================
# Installation.
#======================================================================
.PHONY: install
install: ${LIB}
	echo to do
