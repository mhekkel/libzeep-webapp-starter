# SPDX-License-Identifier: BSD-2-Clause
# 
# Copyright (c) 2023 Maarten L. Hekkelman
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.16)

# set the project name
project(libzeep-webapp-starter VERSION 1.0.0 LANGUAGES CXX)

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

if(MRC_FOUND)
	mrc_target_resources(libzeep-webapp-starter ${PROJECT_SOURCE_DIR}/docroot/)
endif()
