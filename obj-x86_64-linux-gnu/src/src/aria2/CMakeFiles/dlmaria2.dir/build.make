# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /data/home/bulw/downloadmanager

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu

# Include any dependencies generated for this target.
include src/src/aria2/CMakeFiles/dlmaria2.dir/depend.make

# Include the progress variables for this target.
include src/src/aria2/CMakeFiles/dlmaria2.dir/progress.make

# Include the compile flags for this target's objects.
include src/src/aria2/CMakeFiles/dlmaria2.dir/flags.make

src/src/aria2/__/__/include/aria2/moc_aria2rpcinterface.cpp: ../src/include/aria2/aria2rpcinterface.h
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating __/__/include/aria2/moc_aria2rpcinterface.cpp"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/__/__/include/aria2 && /usr/lib/qt5/bin/moc @/data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/__/__/include/aria2/moc_aria2rpcinterface.cpp_parameters

src/src/aria2/CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.o: src/src/aria2/CMakeFiles/dlmaria2.dir/flags.make
src/src/aria2/CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.o: ../src/src/aria2/aria2rpcinterface.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/src/aria2/CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.o"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.o -c /data/home/bulw/downloadmanager/src/src/aria2/aria2rpcinterface.cpp

src/src/aria2/CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.i"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/home/bulw/downloadmanager/src/src/aria2/aria2rpcinterface.cpp > CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.i

src/src/aria2/CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.s"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/home/bulw/downloadmanager/src/src/aria2/aria2rpcinterface.cpp -o CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.s

src/src/aria2/CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.o: src/src/aria2/CMakeFiles/dlmaria2.dir/flags.make
src/src/aria2/CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.o: ../src/src/aria2/aria2cbtinfo.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/src/aria2/CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.o"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.o -c /data/home/bulw/downloadmanager/src/src/aria2/aria2cbtinfo.cpp

src/src/aria2/CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.i"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/home/bulw/downloadmanager/src/src/aria2/aria2cbtinfo.cpp > CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.i

src/src/aria2/CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.s"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/home/bulw/downloadmanager/src/src/aria2/aria2cbtinfo.cpp -o CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.s

src/src/aria2/CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.o: src/src/aria2/CMakeFiles/dlmaria2.dir/flags.make
src/src/aria2/CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.o: src/src/aria2/__/__/include/aria2/moc_aria2rpcinterface.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/src/aria2/CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.o"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.o -c /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/__/__/include/aria2/moc_aria2rpcinterface.cpp

src/src/aria2/CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.i"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/__/__/include/aria2/moc_aria2rpcinterface.cpp > CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.i

src/src/aria2/CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.s"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/__/__/include/aria2/moc_aria2rpcinterface.cpp -o CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.s

src/src/aria2/CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.o: src/src/aria2/CMakeFiles/dlmaria2.dir/flags.make
src/src/aria2/CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.o: src/src/aria2/dlmaria2_autogen/mocs_compilation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/src/aria2/CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.o"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.o -c /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/dlmaria2_autogen/mocs_compilation.cpp

src/src/aria2/CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.i"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/dlmaria2_autogen/mocs_compilation.cpp > CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.i

src/src/aria2/CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.s"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/dlmaria2_autogen/mocs_compilation.cpp -o CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.s

# Object files for target dlmaria2
dlmaria2_OBJECTS = \
"CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.o" \
"CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.o" \
"CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.o" \
"CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.o"

# External object files for target dlmaria2
dlmaria2_EXTERNAL_OBJECTS =

lib/libdlmaria2.so: src/src/aria2/CMakeFiles/dlmaria2.dir/aria2rpcinterface.cpp.o
lib/libdlmaria2.so: src/src/aria2/CMakeFiles/dlmaria2.dir/aria2cbtinfo.cpp.o
lib/libdlmaria2.so: src/src/aria2/CMakeFiles/dlmaria2.dir/__/__/include/aria2/moc_aria2rpcinterface.cpp.o
lib/libdlmaria2.so: src/src/aria2/CMakeFiles/dlmaria2.dir/dlmaria2_autogen/mocs_compilation.cpp.o
lib/libdlmaria2.so: src/src/aria2/CMakeFiles/dlmaria2.dir/build.make
lib/libdlmaria2.so: lib/libdlmlog.so
lib/libdlmaria2.so: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.11.3
lib/libdlmaria2.so: src/src/aria2/CMakeFiles/dlmaria2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX shared library ../../../lib/libdlmaria2.so"
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dlmaria2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/src/aria2/CMakeFiles/dlmaria2.dir/build: lib/libdlmaria2.so

.PHONY : src/src/aria2/CMakeFiles/dlmaria2.dir/build

src/src/aria2/CMakeFiles/dlmaria2.dir/clean:
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 && $(CMAKE_COMMAND) -P CMakeFiles/dlmaria2.dir/cmake_clean.cmake
.PHONY : src/src/aria2/CMakeFiles/dlmaria2.dir/clean

src/src/aria2/CMakeFiles/dlmaria2.dir/depend: src/src/aria2/__/__/include/aria2/moc_aria2rpcinterface.cpp
	cd /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/home/bulw/downloadmanager /data/home/bulw/downloadmanager/src/src/aria2 /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2 /data/home/bulw/downloadmanager/obj-x86_64-linux-gnu/src/src/aria2/CMakeFiles/dlmaria2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/src/aria2/CMakeFiles/dlmaria2.dir/depend

