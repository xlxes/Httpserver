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
include CMakeFiles/httpthread.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/httpthread.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/httpthread.dir/flags.make

CMakeFiles/httpthread.dir/base/httpthread.cpp.o: CMakeFiles/httpthread.dir/flags.make
CMakeFiles/httpthread.dir/base/httpthread.cpp.o: ../base/httpthread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/x/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/httpthread.dir/base/httpthread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/httpthread.dir/base/httpthread.cpp.o -c /home/x/httpserver/base/httpthread.cpp

CMakeFiles/httpthread.dir/base/httpthread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/httpthread.dir/base/httpthread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/x/httpserver/base/httpthread.cpp > CMakeFiles/httpthread.dir/base/httpthread.cpp.i

CMakeFiles/httpthread.dir/base/httpthread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/httpthread.dir/base/httpthread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/x/httpserver/base/httpthread.cpp -o CMakeFiles/httpthread.dir/base/httpthread.cpp.s

CMakeFiles/httpthread.dir/base/httpthread.cpp.o.requires:

.PHONY : CMakeFiles/httpthread.dir/base/httpthread.cpp.o.requires

CMakeFiles/httpthread.dir/base/httpthread.cpp.o.provides: CMakeFiles/httpthread.dir/base/httpthread.cpp.o.requires
	$(MAKE) -f CMakeFiles/httpthread.dir/build.make CMakeFiles/httpthread.dir/base/httpthread.cpp.o.provides.build
.PHONY : CMakeFiles/httpthread.dir/base/httpthread.cpp.o.provides

CMakeFiles/httpthread.dir/base/httpthread.cpp.o.provides.build: CMakeFiles/httpthread.dir/base/httpthread.cpp.o


# Object files for target httpthread
httpthread_OBJECTS = \
"CMakeFiles/httpthread.dir/base/httpthread.cpp.o"

# External object files for target httpthread
httpthread_EXTERNAL_OBJECTS =

libhttpthread.a: CMakeFiles/httpthread.dir/base/httpthread.cpp.o
libhttpthread.a: CMakeFiles/httpthread.dir/build.make
libhttpthread.a: CMakeFiles/httpthread.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/x/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libhttpthread.a"
	$(CMAKE_COMMAND) -P CMakeFiles/httpthread.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/httpthread.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/httpthread.dir/build: libhttpthread.a

.PHONY : CMakeFiles/httpthread.dir/build

CMakeFiles/httpthread.dir/requires: CMakeFiles/httpthread.dir/base/httpthread.cpp.o.requires

.PHONY : CMakeFiles/httpthread.dir/requires

CMakeFiles/httpthread.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/httpthread.dir/cmake_clean.cmake
.PHONY : CMakeFiles/httpthread.dir/clean

CMakeFiles/httpthread.dir/depend:
	cd /home/x/httpserver/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/x/httpserver /home/x/httpserver /home/x/httpserver/build /home/x/httpserver/build /home/x/httpserver/build/CMakeFiles/httpthread.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/httpthread.dir/depend
