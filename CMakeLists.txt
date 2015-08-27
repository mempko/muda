cmake_minimum_required(VERSION 2.6)
project(muda)

#setup boost
set(Boost_USE_STATIC_LIBS on)
find_package(Boost 1.42 COMPONENTS filesystem serialization regex signals)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

#setup wt
link_directories(${muda_SOURCE_DIR}/external/wt/lib)
include_directories(${muda_SOURCE_DIR}/external/wt/include)

add_subdirectory(src)
