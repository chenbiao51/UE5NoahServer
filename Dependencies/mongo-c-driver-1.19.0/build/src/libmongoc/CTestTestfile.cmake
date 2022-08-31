# CMake generated Testfile for 
# Source directory: /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libmongoc
# Build directory: /home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test-libmongoc "/home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/build/src/libmongoc/test-libmongoc")
set_tests_properties(test-libmongoc PROPERTIES  WORKING_DIRECTORY "/home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libmongoc/../.." _BACKTRACE_TRIPLES "/home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libmongoc/CMakeLists.txt;1040;add_test;/home/gouzi/Desktop/NoahGameFrame/Dependencies/mongo-c-driver-1.19.0/src/libmongoc/CMakeLists.txt;0;")
subdirs("build")
subdirs("examples")
subdirs("src")
subdirs("tests")
