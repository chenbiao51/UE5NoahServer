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
include src/libbson/CMakeFiles/bson-validate.dir/depend.make

# Include the progress variables for this target.
include src/libbson/CMakeFiles/bson-validate.dir/progress.make

# Include the compile flags for this target's objects.
include src/libbson/CMakeFiles/bson-validate.dir/flags.make

src/libbson/CMakeFiles/bson-validate.dir/examples/bson-validate.c.o: src/libbson/CMakeFiles/bson-validate.dir/flags.make
src/libbson/CMakeFiles/bson-validate.dir/examples/bson-validate.c.o: ../src/libbson/examples/bson-validate.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/libbson/CMakeFiles/bson-validate.dir/examples/bson-validate.c.o"
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libbson && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/bson-validate.dir/examples/bson-validate.c.o   -c /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libbson/examples/bson-validate.c

src/libbson/CMakeFiles/bson-validate.dir/examples/bson-validate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bson-validate.dir/examples/bson-validate.c.i"
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libbson && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libbson/examples/bson-validate.c > CMakeFiles/bson-validate.dir/examples/bson-validate.c.i

src/libbson/CMakeFiles/bson-validate.dir/examples/bson-validate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bson-validate.dir/examples/bson-validate.c.s"
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libbson && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libbson/examples/bson-validate.c -o CMakeFiles/bson-validate.dir/examples/bson-validate.c.s

# Object files for target bson-validate
bson__validate_OBJECTS = \
"CMakeFiles/bson-validate.dir/examples/bson-validate.c.o"

# External object files for target bson-validate
bson__validate_EXTERNAL_OBJECTS =

src/libbson/bson-validate: src/libbson/CMakeFiles/bson-validate.dir/examples/bson-validate.c.o
src/libbson/bson-validate: src/libbson/CMakeFiles/bson-validate.dir/build.make
src/libbson/bson-validate: src/libbson/libbson-1.0.so.0.0.0
src/libbson/bson-validate: src/libbson/CMakeFiles/bson-validate.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bson-validate"
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libbson && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bson-validate.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/libbson/CMakeFiles/bson-validate.dir/build: src/libbson/bson-validate

.PHONY : src/libbson/CMakeFiles/bson-validate.dir/build

src/libbson/CMakeFiles/bson-validate.dir/clean:
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libbson && $(CMAKE_COMMAND) -P CMakeFiles/bson-validate.dir/cmake_clean.cmake
.PHONY : src/libbson/CMakeFiles/bson-validate.dir/clean

src/libbson/CMakeFiles/bson-validate.dir/depend:
	cd /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0 /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libbson /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libbson /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libbson/CMakeFiles/bson-validate.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/libbson/CMakeFiles/bson-validate.dir/depend
