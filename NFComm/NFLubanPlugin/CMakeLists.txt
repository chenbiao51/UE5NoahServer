set(ProjectName "NFLubanPlugin")

include_directories(
	bright
	Gen)

file(GLOB NFLubanPlugin_ROOT_Cpp *.cpp)
	
file(GLOB NFLubanPlugin_ROOT_Hpp *.h)


file(GLOB_RECURSE NF_Luban_Bright_Cpp ./bright/*.cpp)
file(GLOB_RECURSE NF_Luban_Bright_Hpp ./bright/*.h  ./bright/*.hpp)
file(GLOB NF_LubanGen_Cpp ./Gen/*.cpp)
file(GLOB NF_LubanGen_Hpp ./Gen/*.h)


add_library(NFLubanPlugin STATIC
	${NFLubanPlugin_ROOT_Cpp}
	${NFLubanPlugin_ROOT_Hpp}
	${NF_Luban_Bright_Cpp}
	${NF_Luban_Bright_Hpp}
	${NF_LubanGen_Cpp}
	${NF_LubanGen_Hpp}
	)
	
set_target_properties( NFLubanPlugin PROPERTIES OUTPUT_NAME_DEBUG "NFLubanPlugin")
set_target_properties( NFLubanPlugin PROPERTIES PREFIX "")
set_target_properties( NFLubanPlugin PROPERTIES 
	FOLDER "NFComm/NFLubanPlugin"
	ARCHIVE_OUTPUT_DIRECTORY ${NFOutPutDir}
	RUNTIME_OUTPUT_DIRECTORY ${NFOutPutDir}
	LIBRARY_OUTPUT_DIRECTORY ${NFOutPutDir} )

link_NFSDK("NFLubanPlugin")

if(UNIX)
	target_link_libraries(NFLubanPlugin -static-libstdc++)

	add_definitions(
		-DELPP_NO_DEFAULT_LOG_FILE
	)
	
else()

	target_link_libraries(NFLubanPlugin -static-libstdc++)
	
	add_definitions(
		-DWIN
		-DELPP_NO_DEFAULT_LOG_FILE
	)
endif()
