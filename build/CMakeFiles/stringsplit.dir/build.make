# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/x/httpserver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/x/httpserver/build

# Include any dependencies generated for this target.
include CMakeFiles/stringsplit.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/stringsplit.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/stringsplit.dir/flags.make

CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o: CMakeFiles/stringsplit.dir/flags.make
CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o: ../src/stringsplit.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/x/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o -c /home/x/httpserver/src/stringsplit.cpp

CMakeFiles/stringsplit.dir/src/stringsplit.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/stringsplit.dir/src/stringsplit.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/x/httpserver/src/stringsplit.cpp > CMakeFiles/stringsplit.dir/src/stringsplit.cpp.i

CMakeFiles/stringsplit.dir/src/stringsplit.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/stringsplit.dir/src/stringsplit.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/x/httpserver/src/stringsplit.cpp -o CMakeFiles/stringsplit.dir/src/stringsplit.cpp.s

CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o.requires:

.PHONY : CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o.requires

CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o.provides: CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o.requires
	$(MAKE) -f CMakeFiles/stringsplit.dir/build.make CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o.provides.build
.PHONY : CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o.provides

CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o.provides.build: CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o


# Object files for target stringsplit
stringsplit_OBJECTS = \
"CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o"

# External object files for target stringsplit
stringsplit_EXTERNAL_OBJECTS =

libstringsplit.a: CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o
libstringsplit.a: CMakeFiles/stringsplit.dir/build.make
libstringsplit.a: CMakeFiles/stringsplit.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/x/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libstringsplit.a"
	$(CMAKE_COMMAND) -P CMakeFiles/stringsplit.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/stringsplit.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/stringsplit.dir/build: libstringsplit.a

.PHONY : CMakeFiles/stringsplit.dir/build

CMakeFiles/stringsplit.dir/requires: CMakeFiles/stringsplit.dir/src/stringsplit.cpp.o.requires

.PHONY : CMakeFiles/stringsplit.dir/requires

CMakeFiles/stringsplit.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/stringsplit.dir/cmake_clean.cmake
.PHONY : CMakeFiles/stringsplit.dir/clean

CMakeFiles/stringsplit.dir/depend:
	cd /home/x/httpserver/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/x/httpserver /home/x/httpserver /home/x/httpserver/build /home/x/httpserver/build /home/x/httpserver/build/CMakeFiles/stringsplit.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/stringsplit.dir/depend
