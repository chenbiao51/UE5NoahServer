set(ProjectName "NFMongoDBPlugin")

set(ProjectName "NFMongoDBPlugin")
file(GLOB NFMongoDBPlugin_ROOT_Cpp
		*.cpp)

file(GLOB NFMongoDBPlugin_ROOT_Hpp
		*.h)

add_library(NFMongoDBPlugin STATIC
		${NFMongoDBPlugin_ROOT_Cpp}
		${NFMongoDBPlugin_ROOT_Hpp})

set_target_properties( NFMongoDBPlugin PROPERTIES OUTPUT_NAME_DEBUG "NFMongoDBPlugin")
set_target_properties( NFMongoDBPlugin PROPERTIES PREFIX "")
set_target_properties( NFMongoDBPlugin PROPERTIES
		FOLDER "NFComm/NFMongoDBPlugin"
		ARCHIVE_OUTPUT_DIRECTORY ${NFOutPutDir}
		RUNTIME_OUTPUT_DIRECTORY ${NFOutPutDir}
		LIBRARY_OUTPUT_DIRECTORY ${NFOutPutDir} )
link_NFSDK("NFMongoDBPlugin")

if(UNIX)
	if (CMAKE_BUILD_TYPE MATCHES "Release")
		target_link_libraries(NFMongoDBPlugin bsoncxx mongocxx)
	else()
		target_link_libraries(NFMongoDBPlugin bsoncxx mongocxx)
	endif()

	target_link_libraries(NFMongoDBPlugin -static-libstdc++)

	add_definitions(
			-DELPP_NO_DEFAULT_LOG_FILE
	)

else()
	if (CMAKE_BUILD_TYPE MATCHES "Release")
		target_link_libraries(NFMongoDBPlugin bsoncxx mongocxx)
	else()
		target_link_libraries(NFMongoDBPlugin bsoncxx mongocxx)
	endif()

	target_link_libraries(NFMongoDBPlugin -static-libstdc++)
	add_definitions(
			-DWIN
			-DELPP_NO_DEFAULT_LOG_FILE
	)
endif()

