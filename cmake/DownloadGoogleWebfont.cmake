# SPDX-License-Identifier: BSD-2-Clause
# 
# Copyright (c) 2025 Maarten L. Hekkelman
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

cmake_minimum_required(VERSION 3.15)

function(check_download_status stat)
    list(POP_FRONT _stat _status_code)

    if(_status_code)
        message(FATAL_ERROR "google font downloader: Error trying to download Google font file: ${_stat}")
    endif()
endfunction()

function(download_google_webfont)
    set(flags VERBOSE)
    set(options CSS_FILE)
    set(fonts FONTS)
    cmake_parse_arguments(GWF_OPTION "${flags}" "${options}" "${fonts}" ${ARGN})

    cmake_path(GET GWF_OPTION_CSS_FILE PARENT_PATH _css_file_dir)

    set(_temp_file ${CMAKE_CURRENT_BINARY_DIR}/gwf.file.1)
    set(_temp_css_file ${CMAKE_CURRENT_BINARY_DIR}/gwf.file.2)

    file(WRITE ${_temp_css_file} "")
    set(_file_empty ON)

    foreach(_font IN LISTS GWF_OPTION_FONTS)
        string(REPLACE " " "+" _font_name ${_font})
        set(_url "https://fonts.googleapis.com/css?family=${_font_name}")

        message(STATUS "downloading ${_url}")
        file(DOWNLOAD ${_url} ${_temp_file} STATUS _stat)
        check_download_status(${_stat})

        file(READ ${_temp_file} _font_file_text)
        file(REMOVE ${_temp_file})

        string(REGEX MATCHALL "\(https://[^)]+\)" _font_file_urls ${_font_file_text})

        foreach(_font_file_url IN LISTS _font_file_urls)
            string(REPLACE "https://fonts.gstatic.com/s" "fonts" _font_file_relative_path ${_font_file_url})
            cmake_path(APPEND _css_file_dir ${_font_file_relative_path} OUTPUT_VARIABLE _font_file_path)
            cmake_path(GET _font_file_path RELATIVE_PART _font_file_path_relative)

            if(NOT EXISTS ${_font_file_path})
                message(STATUS "downloading ${_font_file_url}")
                file(DOWNLOAD ${_font_file_url} "${_font_file_path}" STATUS _stat)
                check_download_status(${_stat})
            endif()

            string(REPLACE "${_font_file_url}" "${_font_file_relative_path}" _font_file_text "${_font_file_text}")
        endforeach()

        file(APPEND ${_temp_css_file} "${_font_file_text}")
        set(_file_empty OFF)
    endforeach()

    if(NOT _file_empty)
        file(COPY_FILE ${_temp_css_file} ${GWF_OPTION_CSS_FILE} ONLY_IF_DIFFERENT)
    endif()

    file(REMOVE ${_temp_css_file})
endfunction(download_google_webfont)
