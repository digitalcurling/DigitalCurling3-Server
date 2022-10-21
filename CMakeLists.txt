cmake_minimum_required(VERSION 3.19)

project(digitalcurling3__server
    VERSION 0.1.0
    LANGUAGES CXX
)

# use C++ 17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# symbol visiblity
set(CMAKE_CXX_VISIBLIRY_PRESET hidden)
set(CMAKE_VISIBLITY_INLINES_HIDDEN TRUE)


add_subdirectory(extern/DigitalCurling3)


add_executable(digitalcurling3__server
    src/config.cpp
    src/config.hpp
    src/game.cpp
    src/game.hpp
    src/log.cpp
    src/log.hpp
    src/main.cpp
    src/server.cpp
    src/server.hpp
    src/tcp_session.cpp
    src/tcp_session.hpp
    src/trajectory_compressor.cpp
    src/trajectory_compressor.hpp
    src/util.cpp
    src/util.hpp
)

# boost.asio, boost.program_options
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.75 REQUIRED COMPONENTS date_time regex program_options)

find_package(Threads REQUIRED)

target_link_libraries(digitalcurling3__server
  PRIVATE
    digitalcurling3::digitalcurling3
    nlohmann_json::nlohmann_json
    Boost::headers
    Boost::date_time
    Boost::regex
    Boost::program_options
    Threads::Threads
)
