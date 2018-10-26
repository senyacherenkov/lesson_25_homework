cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{TRAVIS_BUILD_NUMBER})
        set(ENV{TRAVIS_BUILD_NUMBER} "0")
endif()

project(bulk_async_server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Boost COMPONENTS system REQUIRED)

set(CMAKE_BUILD_TYPE DEBUG)

include_directories(${Boost_INCLUDE_DIR})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(async SHARED async.h async.cpp observer.h observer.cpp subject.h subject.cpp utility.h utility.cpp manager.h manager.cpp)
add_executable(bulk_server main.cpp)

set_target_properties(bulk_server async PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

add_compile_options(lboost_system)

target_link_libraries(bulk_server 
	Threads::Threads 
	${Boost_LIBRARIES} 
	async
)

install(TARGETS bulk_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT senyacherenkov@yandex.ru)

include (CPack)
