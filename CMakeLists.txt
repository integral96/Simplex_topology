cmake_minimum_required(VERSION 3.5)

project(topology_symplex ${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/x64-windows-static/include)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS filesystem iostreams REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif ()
#ADD_DEFINITIONS(-DQT_NO_KEYWORDS)


#find_package(TBB COMPONENTS tbb)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -lboost_log_setup -pthread -lstdc++fs --pedantic -flifetime-dse=1")
set(CMAKE_CXX_FLAGS "-Wall -fPIC -DBOOST_LOG_DYN_LINK")

file(GLOB SOURCES
    *.hpp
    *.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} )
target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES}

    )
