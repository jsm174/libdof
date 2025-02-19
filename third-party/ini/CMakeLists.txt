# Exports an 'ini' target for ini.h header lib

add_library(ini INTERFACE)
target_include_directories(ini INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
