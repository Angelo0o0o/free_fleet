cmake_minimum_required(VERSION 3.7.0)
project(free_fleet)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

if(NOT CMAKE_BUILD_TYPE)
  # Use the Release build type by default if the user has not specified one
  set(CMAKE_BUILD_TYPE Release)
endif()

include(GNUInstallDirs)

find_package(ament_cmake REQUIRED)
find_package(Eigen3 REQUIRED)

set(dep_pkgs
  Threads
  rmf_utils
  rmf_api_msgs
  nlohmann_json
  nlohmann_json_schema_validator_vendor
  nlohmann_json_schema_validator
)
foreach(pkg ${dep_pkgs})
  find_package(${pkg} REQUIRED)
endforeach()

file(GLOB lib_srcs
  "src/*.cpp"
  "src/client/*.cpp"
  # "src/manager/*.cpp"
  # "src/messages/*.cpp"
  # "src/manager/requests/*.cpp"
  # "src/manager/utilities/*.cpp"
)

add_library(free_fleet SHARED
  ${lib_srcs}
)

target_link_libraries(free_fleet
  PUBLIC
    # rmf_traffic::rmf_traffic
    rmf_utils::rmf_utils
    Threads::Threads
    rmf_api_msgs::rmf_api_msgs
    nlohmann_json::nlohmann_json
    nlohmann_json_schema_validator
)

target_include_directories(free_fleet
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${rmf_api_msgs_INCLUDE_DIRS}
    ${nlohmann_json_schema_validator_INCLUDE_DIRS}
)

# Tests
# if (BUILD_TESTING)
#   find_package(ament_cmake_catch2 REQUIRED)
#   find_package(ament_cmake_uncrustify REQUIRED)

#   file(GLOB_RECURSE unit_test_srcs "test/*.cpp")

#   ament_add_catch2(
#     test_free_fleet test/main.cpp ${unit_test_srcs}
#     TIMEOUT 300)
#   target_link_libraries(test_free_fleet
#     free_fleet
#     rmf_utils::rmf_utils
#     # rmf_traffic::rmf_traffic
#   )
#   target_include_directories(test_free_fleet
#     PRIVATE
#       $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/>
#   )

#   find_file(uncrustify_config_file
#     NAMES "rmf_code_style.cfg"
#     PATHS "${rmf_utils_DIR}/../../../share/rmf_utils/")

#   ament_uncrustify(
#     include src test
#     CONFIG_FILE ${uncrustify_config_file}
#     MAX_LINE_LENGTH 80
#   )
# endif()

ament_export_targets(free_fleet HAS_LIBRARY_TARGET)
ament_export_dependencies(${dep_pkgs})

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  TARGETS
    free_fleet
  EXPORT free_fleet
  RUNTIME DESTINATION lib/free_fleet
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

ament_package()
