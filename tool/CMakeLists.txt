cmake_minimum_required(VERSION 3.5)

project(coviddatatool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# change accordingly
include_directories("/usr/local/include")
include_directories("../orpp")


add_executable(coviddatatool main.cpp)

#

#target_link_libraries("seirfilter" ${LNLOPT} )


