# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build

# Include any dependencies generated for this target.
include src/libmongoc/CMakeFiles/hello_mongoc.dir/depend.make

# Include the progress variables for this target.
include src/libmongoc/CMakeFiles/hello_mongoc.dir/progress.make

# Include the compile flags for this target's objects.
include src/libmongoc/CMakeFiles/hello_mongoc.dir/flags.make

src/libmongoc/CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.o: src/libmongoc/CMakeFiles/hello_mongoc.dir/flags.make
src/libmongoc/CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.o: ../src/libmongoc/examples/hello_mongoc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/libmongoc/CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.o"
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.o   -c /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libmongoc/examples/hello_mongoc.c

src/libmongoc/CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.i"
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libmongoc/examples/hello_mongoc.c > CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.i

src/libmongoc/CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.s"
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libmongoc/examples/hello_mongoc.c -o CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.s

# Object files for target hello_mongoc
hello_mongoc_OBJECTS = \
"CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.o"

# External object files for target hello_mongoc
hello_mongoc_EXTERNAL_OBJECTS =

src/libmongoc/hello_mongoc: src/libmongoc/CMakeFiles/hello_mongoc.dir/examples/hello_mongoc.c.o
src/libmongoc/hello_mongoc: src/libmongoc/CMakeFiles/hello_mongoc.dir/build.make
src/libmongoc/hello_mongoc: src/libmongoc/libmongoc-1.0.so.0.0.0
src/libmongoc/hello_mongoc: /usr/lib/x86_64-linux-gnu/libssl.so
src/libmongoc/hello_mongoc: /usr/lib/x86_64-linux-gnu/libcrypto.so
src/libmongoc/hello_mongoc: /usr/lib/x86_64-linux-gnu/libz.so
src/libmongoc/hello_mongoc: src/libbson/libbson-1.0.so.0.0.0
src/libmongoc/hello_mongoc: src/libmongoc/CMakeFiles/hello_mongoc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable hello_mongoc"
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hello_mongoc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/libmongoc/CMakeFiles/hello_mongoc.dir/build: src/libmongoc/hello_mongoc

.PHONY : src/libmongoc/CMakeFiles/hello_mongoc.dir/build

src/libmongoc/CMakeFiles/hello_mongoc.dir/clean:
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc && $(CMAKE_COMMAND) -P CMakeFiles/hello_mongoc.dir/cmake_clean.cmake
.PHONY : src/libmongoc/CMakeFiles/hello_mongoc.dir/clean

src/libmongoc/CMakeFiles/hello_mongoc.dir/depend:
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0 /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libmongoc /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc/CMakeFiles/hello_mongoc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/libmongoc/CMakeFiles/hello_mongoc.dir/depend

