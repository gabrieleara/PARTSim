# Compilation variables
# To compile debug version use make BUILD=debug
# To compile using another C++ version use CXX_VERSION=c++14
# Should compile with c++14 just fine (or even better)
BUILD              := release
CXX_VERSION        := c++11

# To enable verbose compilation output, use make V=1
V                  :=

################################################################################

# Directories
DIR_RELEASE         = release
DIR_DEBUG           = debug
DIR_DEPENDENCIES    = deps

DIRS = $(DIR_RELEASE) $(DIR_DEBUG) $(DIR_DEPENDENCIES)

# Files
CPP_FILES           = $(wildcard *.cpp)
DEP_FILES           = $(wildcard ${DIR_DEPENDENCIES}/*.d)
# HPP_FILES           = $(wildcard *.hpp)

# Automatically places object files in correct directory
OBJ_FILES           = $(patsubst %.cpp,${BUILD}/%.o,$(CPP_FILES))

PROGRAMS.release    = $(shell find examples/ -name '*.cpp' | sed -e 's/\.cpp//' | grep -v 'feedback\|schedp')
PROGRAMS.debug      = $(patsubst %,%_debug,$(PROGRAMS.release))

LIBRARIES.release   = librtsim.so
LIBRARIES.debug     = $(patsubst %.so,%_debug.so,$(LIBRARIES.release))

PROGRAMS            = ${PROGRAMS.${BUILD}}
LIBRARIES           = ${LIBRARIES.${BUILD}}

# Compilation flags
CPPFLAGS           += -I.

# TODO: Typically, I build using these three arguments, but for this projects there were so many errors that I had to remove them.
CXXFLAGS.common     = # -Wall -Wextra -pedantic
CXXFLAGS.common    += -Wno-deprecated -Wno-deprecated-declarations
CXXFLAGS.common    += -fPIC --std=$(strip ${CXX_VERSION})
CXXFLAGS.release    = -Werror -O3
CXXFLAGS.debug      = -g -O0 -D__DEBUG__
LDFLAGS_LIB        += -shared

# These flags generate dependency files during compilation (the substitution is
# needed for when compiled files are in subdirectories, to avoid problems for
# mising paths in the $DIR_DEPENDENCIES folder)
DEPFLAGS            = -MT $@ -MMD -MP -MF $(DIR_DEPENDENCIES)/$(subst /,--,$@).d

# Uses either debug or release flags depending on the value of BUILD
CXXFLAGS           += ${CXXFLAGS.common} ${CXXFLAGS.${BUILD}} ${DEPFLAGS}

# Linking options
PROGRAMS_LDLIBS.common  = -L. -lboost_program_options
PROGRAMS_LDLIBS.release = -lrtsim
PROGRAMS_LDLIBS.debug   = -lrtsim_debug

PROGRAMS_LDLIBS     = ${PROGRAMS_LDLIBS.common} ${PROGRAMS_LDLIBS.${BUILD}}

# Compilation and linking program
CC                  = g++

# Compilation rules
COMPILE_RULE        = $(COMPILE.cc) $(OUTPUT_OPTION)
COMPILE_RULE_LIB    = $(COMPILE_RULE) $(CXXFLAGS_LIB)

# Linking rules
LINK_RULE_PROGRAM   = $(LINK.o) $(OUTPUT_OPTION)
LINK_RULE_LIBRARY   = $(LINK.o) $(LDFLAGS_LIB) $(OUTPUT_OPTION)

################################################################################

# Define macros to handle verbosity level

# Check if V is greater than 0
ifeq ($(shell !(test -z $(V)) && test $(V) -gt 0; echo $$?),0)
	# It is, use default behavior and print every command
	ECHO_COMPILE_CMD=
	ECHO_LINK_CMD   =
	V_OPT           =
else
	# It is not, use a compact version
	ECHO_COMPILE_CMD=@echo "   CXX $@"
	ECHO_LINK_CMD   =@echo "   LNK $@"
	V_OPT           =@
endif


################################################################################

# Targets

.PHONY: all dirs run clean

all: $(LIBRARIES) $(PROGRAMS)

dirs: $(DIRS)

$(DIRS):
	mkdir -p $@

release/%.o: %.cpp | dirs
	$(ECHO_COMPILE_CMD)
	$(V_OPT) $(COMPILE_RULE) $<

debug/%.o: %.cpp | dirs
	$(ECHO_COMPILE_CMD)
	$(V_OPT) $(COMPILE_RULE) $<

librtsim.so: $(OBJ_FILES)
	$(ECHO_LINK_CMD)
	$(V_OPT) $(LINK_RULE_LIBRARY) $^

librtsim_debug.so: $(OBJ_FILES)
	$(ECHO_LINK_CMD)
	$(V_OPT) $(LINK_RULE_LIBRARY) $^

# Files after the pipe in dependencies lists are "order-only dependencies" and indicate that the current rule should be run AFTER the other files have been built, even if those files are not used by the rule itself. In this Makefile, they are used to postpone any build action for examples AFTER successfully bulding the shared library.

examples/%.o: examples/%.cpp | dirs $(LIBRARIES)
	$(ECHO_COMPILE_CMD)
	$(V_OPT) $(COMPILE_RULE) $<

examples/%_debug.o: examples/%.cpp | dirs $(LIBRARIES)
	$(ECHO_COMPILE_CMD)
	$(V_OPT) $(COMPILE_RULE) $<

# REMEMBER: order in linking rules is important!

examples/%: examples/%.o $(LIBRARIES)
	$(ECHO_LINK_CMD)
	$(V_OPT) $(LINK_RULE_PROGRAM) $< $(PROGRAMS_LDLIBS)

examples/%_debug: examples/%_debug.o $(LIBRARIES)
	$(ECHO_LINK_CMD)
	$(V_OPT) $(LINK_RULE_PROGRAM) $< $(PROGRAMS_LDLIBS)

clean:
	rm -f $(PROGRAMS) $(LIBRARIES) $(OBJ_FILES)
	rm -f $(shell find . -name '*.o')
	rm -f capacities.txt debug.txt *~ *.bak

# run:
# 	./example > t.txt

# Dependency files are automatically generated during compilation and included
# if found in the appropriate directory
-include $(DEP_FILES)
