# Tencent is pleased to support the open source community by making xLua available.
# Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
# Licensed under the MIT License (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
# http://opensource.org/licenses/MIT
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

set(ProjectName "NFTsScriptPlugin")
file(GLOB NFTsScriptPlugin_ROOT_Cpp *.cpp)
file(GLOB NFTsScriptPlugin_ROOT_Hpp *.h)

add_library(NFTsScriptPlugin STATIC
	${NFTsScriptPlugin_ROOT_Cpp}
	${NFTsScriptPlugin_ROOT_Hpp})

target_link_libraries(NFTsScriptPlugin libnode.so.93)
target_link_libraries(NFTsScriptPlugin liblua.a)
add_dependencies(NFTsScriptPlugin NFCore)

if(UNIX)

	target_link_libraries(NFTsScriptPlugin -static-libstdc++)
else()


endif()

set_target_properties( NFTsScriptPlugin PROPERTIES OUTPUT_NAME_DEBUG "NFTsScriptPlugin")
set_target_properties( NFTsScriptPlugin PROPERTIES PREFIX "")
set_target_properties( NFTsScriptPlugin PROPERTIES
	FOLDER "NFComm/NFTsScriptPlugin"
	ARCHIVE_OUTPUT_DIRECTORY ${NFOutPutDir}
	RUNTIME_OUTPUT_DIRECTORY ${NFOutPutDir}
	LIBRARY_OUTPUT_DIRECTORY ${NFOutPutDir} )


cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project(embedtest)

set(CMAKE_CXX_FLAGS "-std=c++14 -mfma")
include_directories(
    #nodejs/include
    ${CMAKE_SOURCE_DIR}/nodejs/src
    ${CMAKE_SOURCE_DIR}/nodejs/include
    ${CMAKE_SOURCE_DIR}/nodejs/deps/v8/include
    ${CMAKE_SOURCE_DIR}/nodejs/deps/uv/include
    puerts/include
)

add_executable(embedtest embedtest.cc
          puerts/src/CppObjectMapper.cpp
          puerts/src/DataTransfer.cpp
          puerts/src/JSClassRegister.cpp)
          
#target_compile_definitions(embedtest PRIVATE MAPPER_ISOLATE_DATA_POS=2)

target_link_libraries(embedtest ${CMAKE_SOURCE_DIR}/nodejs/lib/Linux/libnode.so.93)

#cmake -DCMAKE_BUILD_TYPE=Release