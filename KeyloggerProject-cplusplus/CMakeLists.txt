cmake_minimum_required(VERSION 3.10)
project(Keylogger)

set(CMAKE_CXX_STANDARD 17)

add_executable(keylogger src/keylogger.cpp)

target_link_libraries(keylogger user32)

if(WIN32)
    set_target_properties(keylogger PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )

endif()
