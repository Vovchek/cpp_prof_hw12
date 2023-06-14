cmake_minimum_required(VERSION 3.11 FATAL_ERROR) # need for FetchContent

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(join_server VERSION ${PROJECT_VESRION})

find_package(Boost)
include_directories(${Boost_INCLUDE_DIR})

if(NOT WIN32)
  find_package(Threads)
endif()

# see https://www.sqlite.org/howtocompile.html
include(FetchContent)

cmake_host_system_information(RESULT HOST QUERY HOSTNAME)
if("${HOST}" STREQUAL "PCVS012CHEKAL")
    set(SQLITE_URL ${PROJECT_SOURCE_DIR}/supplements/sqlite3src.zip)
else()
    set(SQLITE_URL https://www.sqlite.org/snapshot/sqlite-snapshot-202205041843.tar.gz)
endif()
FetchContent_Declare(
    sqlite3
    URL ${SQLITE_URL}
)

FetchContent_MakeAvailable(sqlite3)
if (NOT sqlite3_POPULATED)
  FetchContent_Populate(sqlite3)
endif()

add_executable(${PROJECT_NAME} main.cpp "${sqlite3_SOURCE_DIR}/sqlite3.c")
target_include_directories(${PROJECT_NAME} PUBLIC "${sqlite3_SOURCE_DIR}")
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)

if(WIN32)
  target_link_libraries(${PROJECT_NAME})
  set(Boost_ARCHITECTURE -x64)
  set(Boost_USE_STATIC_LIBS   ON)
  target_link_libraries(${PROJECT_NAME}
        "C:/msys64/mingw64/lib/libwsock32.a"
        "c:/msys64/mingw64/lib/libws2_32.a"
  )
else()
  target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} dl)
endif()

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT vovchek64@inbox.ru)

include(CPack)
