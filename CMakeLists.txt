cmake_minimum_required(VERSION 3.19)

project(digitalcurling3_server
    VERSION 0.1.1  # server version
    LANGUAGES CXX
)

# protocol version
set(DIGITALCURLING3_SERVER_PROTOCOL_VERSION_MAJOR 1)
set(DIGITALCURLING3_SERVER_PROTOCOL_VERSION_MINOR 0)

# config version
set(DIGITALCURLING3_SERVER_CONFIG_VERSION_MAJOR 1)
set(DIGITALCURLING3_SERVER_CONFIG_VERSION_MINOR 0)

# log version
set(DIGITALCURLING3_SERVER_LOG_VERSION_MAJOR 1)
set(DIGITALCURLING3_SERVER_LOG_VERSION_MINOR 0)


# use C++ 17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# symbol visiblity
set(CMAKE_CXX_VISIBLIRY_PRESET hidden)
set(CMAKE_VISIBLITY_INLINES_HIDDEN TRUE)

# MSVCでutf-8を使用する
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_subdirectory(extern/DigitalCurling3 EXCLUDE_FROM_ALL)

configure_file(src/version.cpp.in version.cpp @ONLY)

add_executable(digitalcurling3_server
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
    src/version.hpp
    "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"  # generated from version.cpp.in
)

# boost.asio, boost.program_options, boost.nowide, boost::filesystem
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.75 REQUIRED
  COMPONENTS
    date_time
    regex
    program_options
    nowide
    filesystem
)

find_package(Threads REQUIRED)

target_include_directories(digitalcurling3_server
  PRIVATE
    src
)

target_link_libraries(digitalcurling3_server
  PRIVATE
    digitalcurling3::digitalcurling3
    nlohmann_json::nlohmann_json
    Boost::headers
    Boost::date_time
    Boost::regex
    Boost::program_options
    Boost::nowide
    Boost::filesystem
    Threads::Threads
)

install(TARGETS digitalcurling3_server
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)