cmake_minimum_required(VERSION 3.9)
project(drwalin_util)
cmake_policy(SET CMP0115 OLD)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DRWUTIL_USE_RPMALLOC "Use rpmalloc. Otherwise use object pool" ON)
option(DRWUTIL_MEMORY_COLLECT_STATS "Enable memory statistics collection" OFF)
option(DRWUTIL_LOG_LEVEL_SINGLE_CHARACTER "Write single character for log level" ON)
option(DRWUTIL_LOG_USE_PRETTY_FUNCTION "Use pretty function name for log" ON)
set(DRWUTIL_LOG_DATETIME_SUBSECONDS_DIGITS 5 CACHE STRING "Number of sub-second digits in log date-time")
set(DRWUTIL_IGNORE_COMMON_LOG_PATH ${PROJECT_SOURCE_DIR} CACHE STRING "A common path part to ignore from log")

aux_source_directory(src/ source_files)
add_library(drwalin_util STATIC
	${source_files}
)

if(NOT DEFINED DRWUTIL_IGNORE_COMMON_LOG_PATH)
	target_compile_definitions(drwalin_util PUBLIC DRWUTIL_LOG_IGNORE_COMMON_PATH="${PROJECT_SOURCE_DIR}")
else()
	target_compile_definitions(drwalin_util PUBLIC
		DRWUTIL_LOG_IGNORE_COMMON_PATH="${DRWUTIL_IGNORE_COMMON_LOG_PATH}")
endif()

if(NOT DEFINED DRWUTIL_LOG_DEFAULT_LOG_LEVEL)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_definitions(drwalin_util PUBLIC DRWUTIL_LOG_DEFAULT_LOG_LEVEL=INFO)
	elseif(DRWUTIL_CLANG_FUZZTEST)
		target_compile_definitions(drwalin_util PUBLIC DRWUTIL_LOG_DEFAULT_LOG_LEVEL=LL_IGNORE)
	else()
		target_compile_definitions(drwalin_util PUBLIC DRWUTIL_LOG_DEFAULT_LOG_LEVEL=LL_IGNORE)
	endif()
else()
	target_compile_definitions(drwalin_util PUBLIC DRWUTIL_LOG_DEFAULT_LOG_LEVEL=${DRWUTIL_LOG_DEFAULT_LOG_LEVEL})
endif()

if(DRWUTIL_MEMORY_COLLECT_STATS)
	target_compile_definitions(drwalin_util PUBLIC DRWUTIL_MEMORY_COLLECT_STATS=1)
endif()

if(DRWUTIL_LOG_LEVEL_SINGLE_CHARACTER)
	target_compile_definitions(drwalin_util PUBLIC DRWUTIL_LOG_LEVEL_SINGLE_CHARACTER=1)
endif()

if(DRWUTIL_LOG_USE_PRETTY_FUNCTION)
	target_compile_definitions(drwalin_util PUBLIC DRWUTIL_LOG_USE_PRETTY_FUNCTION=1)
endif()

target_compile_definitions(drwalin_util PUBLIC DRWUTIL_LOG_DATETIME_SUBSECONDS_DIGITS=${DRWUTIL_LOG_DATETIME_SUBSECONDS_DIGITS})

add_executable(test_log
	tests/test_log.cpp
)
target_link_libraries(test_log PRIVATE drwalin_util)

add_executable(test_config
	tests/test_config.cpp
)
target_link_libraries(test_config PRIVATE drwalin_util)
