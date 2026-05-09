cmake_minimum_required(VERSION 3.16)

project(gozcu-child VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(gozcu-child
  src/main.cpp
  src/controller.cpp
  src/gatherer/eventWatcher.cpp
  src/utility/configHandler.cpp
)

find_package(Boost REQUIRED)
target_include_directories(gozcu-child PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(gozcu-child PRIVATE Boost::headers)
find_package(Threads REQUIRED)
target_link_libraries(gozcu-child PRIVATE Threads::Threads)
target_compile_definitions(gozcu-child
PRIVATE BOOST_ASIO_NO_DEPRECATED
)

find_package(nlohmann_json REQUIRED)
target_link_libraries(gozcu-child PRIVATE nlohmann_json::nlohmann_json)

find_package(OpenSSL REQUIRED)
target_link_libraries(gozcu-child PRIVATE OpenSSL::SSL)
