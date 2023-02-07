cmake_minimum_required(VERSION 3.16)

# set the project name
project(libzeep-webapp-starter VERSION 2.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# Create a revision file, containing the current git version info, if any
include(VersionString)
write_version_header(${PROJECT_SOURCE_DIR}/src)

# use mrc to create resources
find_package(Mrc QUIET)

if(NOT MRC_FOUND)
	message(WARNING "Cannot use resources since mrc was not found")
else()
	add_compile_definitions(WEBAPP_USES_RESOURCES)
	mrc_write_header(${PROJECT_SOURCE_DIR}/src/mrsrc.hpp)
endif()

# Libraries
find_package(zeep 6 REQUIRED)
find_package(libmcfp REQUIRED)

add_executable(libzeep-webapp-starter ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(libzeep-webapp-starter libmcfp::libmcfp zeep::zeep)

# yarn rules for javascripts

find_program(YARN yarn REQUIRED)

set(webpack_input
	${CMAKE_CURRENT_SOURCE_DIR}/webapp/index.js
	${CMAKE_CURRENT_SOURCE_DIR}/webapp/style.scss
	${CMAKE_CURRENT_SOURCE_DIR}/webpack.config.js
)

set(webpack_output ${CMAKE_CURRENT_SOURCE_DIR}/docroot/scripts/index.js)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/docroot/scripts/index.js
	BYPRODUCTS 
	DEPENDS ${webpack_input}
	COMMAND ${YARN} run $<IF:$<CONFIG:Debug>,build,build-production>
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	VERBATIM)

add_custom_target(webpack DEPENDS ${webpack_output})

add_dependencies(libzeep-webapp-starter webpack)
