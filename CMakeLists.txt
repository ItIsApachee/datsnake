cmake_minimum_required(VERSION 3.20)
project(datsnake CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use clang if available
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME cpr
  GITHUB_REPOSITORY libcpr/cpr
  GIT_TAG 1.14.1
  OPTIONS
    "CPR_BUILD_TESTS OFF"
    "CPR_BUILD_TESTS_SSL OFF"
    "CPR_USE_SYSTEM_CURL ON"
)

CPMAddPackage(
  NAME nlohmann_json
  GITHUB_REPOSITORY nlohmann/json
  GIT_TAG v3.11.3
  OPTIONS
    "JSON_BuildTests OFF"
)

add_executable(datsnake
  src/main.cpp
  src/api/client.cpp
  src/game/game.cpp
  src/strategy/greedy.cpp
)

target_include_directories(datsnake PRIVATE
  src
)

target_link_libraries(datsnake PRIVATE
  cpr::cpr
  nlohmann_json::nlohmann_json
)

# cpplint target
find_program(CPPLINT cpplint
  HINTS ${CMAKE_SOURCE_DIR}/.venv/bin
)
if(CPPLINT)
  file(GLOB_RECURSE ALL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.hpp
  )

  add_custom_target(cpplint
    COMMAND ${CPPLINT} ${ALL_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running cpplint..."
  )

  add_dependencies(datsnake cpplint)
else()
  message(WARNING "cpplint not found, skipping code style checks")
endif()
