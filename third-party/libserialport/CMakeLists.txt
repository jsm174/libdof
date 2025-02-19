include(ExternalProject)

set(LIBSERIALPORT_VERSION fd20b0fc5a34cd7f776e4af6c763f59041de223b)
set(LIBSERIALPORT_URL https://github.com/sigrokproject/libserialport/archive/${LIBSERIALPORT_VERSION}.zip)
set(LIBSERIALPORT_HASH bb8d371429849328e6083e61042a85c0d6bdcfd5)


if(PLATFORM STREQUAL "linux")
  ExternalProject_Add(serialport_build
    URL ${LIBSERIALPORT_URL}
    URL_HASH SHA1=${LIBSERIALPORT_HASH}
    CONFIGURE_COMMAND <SOURCE_DIR>/autogen.sh
      COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
  )
elseif(PLATFORM STREQUAL "win")
  find_program(MSBUILD_COMMAND msbuild.exe)

  ExternalProject_Add(serialport_build
    URL ${LIBSERIALPORT_URL}
    URL_HASH SHA1=${LIBSERIALPORT_HASH}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${MSBUILD_COMMAND} <SOURCE_DIR>/libserialport.sln -p:Configuration=Release -p:Platform=${ARCH}
  )
else()
  message(FATAL_ERROR "libserialport build for '${PLATFORM}' platform is not implemented.")
endif()

ExternalProject_Get_Property(serialport_build INSTALL_DIR)

add_library(serialport INTERFACE)
add_dependencies(serialport serialport_build)

target_include_directories(serialport INTERFACE ${INSTALL_DIR}/include)

if (PLATFORM STREQUAL "linux")
  target_link_libraries(serialport INTERFACE
    ${INSTALL_DIR}/lib/libserialport.so
    ${INSTALL_DIR}/lib/libserialport.so.0
    ${INSTALL_DIR}/lib/libserialport.so.0.1.0
  )
endif()
