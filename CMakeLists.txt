cmake_minimum_required(VERSION 3.25)

set(PLATFORM "win" CACHE STRING "Platform")
set(ARCH "x64" CACHE STRING "Arch")

option(BUILD_SHARED "Option to build shared library" ON)
option(BUILD_STATIC "Option to build static library" ON)
option(POST_BUILD_COPY_EXT_LIBS "Option to copy external libraries to build directory" ON)

message(STATUS "PLATFORM: ${PLATFORM}")
message(STATUS "ARCH: ${ARCH}")

message(STATUS "BUILD_SHARED: ${BUILD_SHARED}")
message(STATUS "BUILD_STATIC: ${BUILD_STATIC}")
message(STATUS "POST_BUILD_COPY_EXT_LIBS: ${POST_BUILD_COPY_EXT_LIBS}")

if(PLATFORM STREQUAL "macos")
   set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0)
elseif(PLATFORM STREQUAL "ios" OR PLATFORM STREQUAL "ios-simulator")
   set(CMAKE_SYSTEM_NAME iOS)
   if (PLATFORM STREQUAL "ios-simulator")
      set(CMAKE_OSX_SYSROOT iphonesimulator)
   endif()
   set(CMAKE_OSX_DEPLOYMENT_TARGET 17.0)
elseif(PLATFORM STREQUAL "tvos")
   set(CMAKE_SYSTEM_NAME tvOS)
   set(CMAKE_OSX_DEPLOYMENT_TARGET 17.0)
elseif(PLATFORM STREQUAL "android")
   set(CMAKE_SYSTEM_NAME Android)
   set(CMAKE_SYSTEM_VERSION 30)
   set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
endif()

file(READ include/DOF/DOF.h version)
string(REGEX MATCH "LIBDOF_VERSION_MAJOR[ ]+([0-9]+)" _tmp ${version})
set(VERSION_MAJOR "${CMAKE_MATCH_1}")
string(REGEX MATCH "LIBDOF_VERSION_MINOR[ ]+([0-9]+)" _tmp ${version})
set(VERSION_MINOR "${CMAKE_MATCH_1}")
string(REGEX MATCH "LIBDOF_VERSION_PATCH[ ]+([0-9]+)" _tmp ${version})
set(VERSION_PATCH "${CMAKE_MATCH_1}")

project(dof VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
   DESCRIPTION "Cross-platform DOF utilities library")

# Dependencies
include(FetchContent)

FetchContent_Declare(cargs
  GIT_REPOSITORY git@github.com:likle/cargs.git
  GIT_TAG v1.2.0
)


if(PLATFORM STREQUAL "win")
   if(ARCH STREQUAL "x86")
      add_compile_definitions(WIN32)
   endif()
elseif(PLATFORM STREQUAL "macos")
   if (ARCH STREQUAL "arm64")
      set(CMAKE_OSX_ARCHITECTURES arm64)
   elseif(ARCH STREQUAL "x64")
      set(CMAKE_OSX_ARCHITECTURES x86_64)
   endif()
   set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
   set(CMAKE_INSTALL_RPATH "@executable_path")
elseif(PLATFORM STREQUAL "ios" OR PLATFORM STREQUAL "ios-simulator" STREQUAL "tvos")
   set(CMAKE_OSX_ARCHITECTURES arm64)
elseif(PLATFORM STREQUAL "linux" OR PLATFORM STREQUAL "android")
   set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
   set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)

set(LIBDOF_SOURCES
   src/Config.cpp
   src/DOF.cpp
   src/Pinball.cpp
   src/Log.cpp
   src/Logger.cpp
   
   src/globalconfiguration/GlobalConfig.cpp

   src/pinballsupport/AlarmHandler.cpp
   src/pinballsupport/InputQueue.cpp

   src/fx/AssignedEffect.cpp
   src/fx/AssignedEffectList.cpp
   src/fx/EffectList.cpp
   
   src/table/Table.cpp
   src/table/TableElement.cpp
   src/table/TableElementData.cpp
   src/table/TableElementList.cpp

   src/cab/ICabinetOwner.cpp
   src/cab/Cabinet.cpp
   src/cab/CabinetOutputList.cpp

   src/cab/out/IOutput.cpp
   src/cab/out/Output.cpp
   src/cab/out/IOutputController.cpp
   src/cab/out/OutputControllerBase.cpp
   src/cab/out/OutputControllerCompleteBase.cpp
   src/cab/out/OutputControllerFlexCompleteBase.cpp
   src/cab/out/OutputControllerList.cpp
   src/cab/out/IAutoConfigOutputController.cpp

   src/cab/toys/IToy.cpp
   src/cab/toys/IToyUpdatable.cpp
   src/cab/toys/ToyList.cpp

   src/ledcontrol/loader/LedControl.cpp
   src/ledcontrol/loader/LedControlConfig.cpp
   src/ledcontrol/loader/LedControlConfigList.cpp
)

if(PLATFORM STREQUAL "win" OR PLATFORM STREQUAL "macos" OR PLATFORM STREQUAL "linux")
   list(APPEND LIBDOF_SOURCES
      src/cab/out/ps/Pinscape.cpp
      src/cab/out/ps/PinscapeDevice.cpp
      src/cab/out/ps/PinscapeAutoConfigurator.cpp
   )
endif()

set(LIBDOF_INCLUDE_DIRS
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   third-party/include
)

if(BUILD_SHARED)
   add_library(dof_shared SHARED ${LIBDOF_SOURCES})

   target_include_directories(dof_shared PUBLIC ${LIBDOF_INCLUDE_DIRS})

   if(PLATFORM STREQUAL "win")
      target_link_directories(dof_shared PUBLIC
         third-party/build-libs/${PLATFORM}/${ARCH}
         third-party/runtime-libs/${PLATFORM}/${ARCH}
      )
      if(ARCH STREQUAL "x64")
         target_link_libraries(dof_shared PUBLIC libserialport64 sockpp64 hidapi64 ws2_32)
      else()
         target_link_libraries(dof_shared PUBLIC libserialport sockpp hidapi ws2_32)
      endif()
   elseif(PLATFORM STREQUAL "macos")
      target_link_directories(dof_shared PUBLIC
         third-party/runtime-libs/${PLATFORM}/${ARCH}
      )
      target_link_libraries(dof_shared PUBLIC serialport sockpp hidapi)
   elseif(PLATFORM STREQUAL "linux")
      target_link_directories(dof_shared PUBLIC
         third-party/runtime-libs/${PLATFORM}/${ARCH}
      )
      target_link_libraries(dof_shared PUBLIC serialport sockpp hidapi-hidraw)
   elseif(PLATFORM STREQUAL "ios" OR PLATFORM STREQUAL "ios-simulator" OR PLATFORM STREQUAL "tvos")
      target_link_directories(dof_shared PUBLIC
         third-party/build-libs/${PLATFORM}/${ARCH}
      )
      target_link_libraries(dof_shared PUBLIC sockpp)
   elseif(PLATFORM STREQUAL "android")
      target_link_directories(dof_shared PUBLIC
         third-party/runtime-libs/${PLATFORM}/${ARCH}
      )
      target_link_libraries(dof_shared PUBLIC sockpp)
   endif()

   if(PLATFORM STREQUAL "win" AND ARCH STREQUAL "x64")
      set(LIBDOF_OUTPUT_NAME "dof64")
   else()
      set(LIBDOF_OUTPUT_NAME "dof")
   endif()

   set_target_properties(dof_shared PROPERTIES
      OUTPUT_NAME ${LIBDOF_OUTPUT_NAME}
      VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
   )

   install(TARGETS dof_shared
      LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   )
   install(DIRECTORY include/DOF DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

   if(PLATFORM STREQUAL "win" OR PLATFORM STREQUAL "macos" OR PLATFORM STREQUAL "linux")
      FetchContent_MakeAvailable(cargs)
      add_executable(dofserver
         src/dofServer.cpp
         src/Logger.cpp
      )
      target_link_libraries(dofserver PUBLIC dof_shared PRIVATE cargs)

      add_executable(dofserver_test
         src/testDofServer.cpp
      )
      target_link_libraries(dofserver_test PUBLIC dof_shared)

      add_executable(dof_test
         src/test.cpp
      )
      target_link_libraries(dof_test PUBLIC dof_shared)

      if(POST_BUILD_COPY_EXT_LIBS)
         if(PLATFORM STREQUAL "win")
            if(ARCH STREQUAL "x64")
               add_custom_command(TARGET dofserver POST_BUILD
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/libserialport64.lib" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/libserialport64.dll" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/sockpp64.lib" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/sockpp64.dll" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/hidapi64.lib" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/hidapi64.dll" "$<TARGET_FILE_DIR:dofserver>"
               )
            else()
               add_custom_command(TARGET dofserver POST_BUILD
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/libserialport.lib" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/libserialport.dll" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/sockpp.lib" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/sockpp.dll" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/hidapi.lib" "$<TARGET_FILE_DIR:dofserver>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/hidapi.dll" "$<TARGET_FILE_DIR:dofserver>"
               )
            endif()
         elseif(PLATFORM STREQUAL "macos")
            add_custom_command(TARGET dofserver POST_BUILD
               COMMAND cp -a "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/*.dylib" "$<TARGET_FILE_DIR:dofserver>"
            )
         elseif(PLATFORM STREQUAL "linux")
            add_custom_command(TARGET dofserver POST_BUILD
               COMMAND cp -a "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/*.so" "$<TARGET_FILE_DIR:dofserver>"
               COMMAND cp -a "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/*.so.*" "$<TARGET_FILE_DIR:dofserver>"
            )
         endif()
      endif()
   endif()
endif()

if(BUILD_STATIC)
   add_library(dof_static STATIC ${LIBDOF_SOURCES})

   target_include_directories(dof_static PUBLIC ${LIBDOF_INCLUDE_DIRS})

   if(PLATFORM STREQUAL "win")
      set_target_properties(dof_static PROPERTIES
         OUTPUT_NAME "dof_static"
      )
   else()
      set_target_properties(dof_static PROPERTIES
         OUTPUT_NAME "dof"
      )
   endif()

   install(TARGETS dof_static
      LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   )
   install(DIRECTORY include/DOF DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

   if(PLATFORM STREQUAL "win" OR PLATFORM STREQUAL "macos" OR PLATFORM STREQUAL "linux")
      add_executable(dof_test_s
         src/test.cpp
      )

      if(PLATFORM STREQUAL "win")
         target_link_directories(dof_test_s PUBLIC
            third-party/build-libs/${PLATFORM}/${ARCH}
            third-party/runtime-libs/${PLATFORM}/${ARCH}
         )
         if(ARCH STREQUAL "x64")
            target_link_libraries(dof_test_s PUBLIC dof_static libserialport64 sockpp64 hidapi64 ws2_32)
         else()
            target_link_libraries(dof_test_s PUBLIC dof_static libserialport sockpp hidapi ws2_32)
         endif()
      elseif(PLATFORM STREQUAL "macos")
         target_link_directories(dof_test_s PUBLIC
            third-party/runtime-libs/${PLATFORM}/${ARCH}
         )
         target_link_libraries(dof_test_s PUBLIC dof_static serialport sockpp hidapi)
      elseif(PLATFORM STREQUAL "linux")
         target_link_directories(dof_test_s PUBLIC
            third-party/runtime-libs/${PLATFORM}/${ARCH}
         )
         target_link_libraries(dof_test_s PUBLIC dof_static serialport sockpp hidapi-hidraw)
      endif()

      if(POST_BUILD_COPY_EXT_LIBS)
         if(PLATFORM STREQUAL "win")
            if(ARCH STREQUAL "x64")
               add_custom_command(TARGET dof_test_s POST_BUILD
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/libserialport64.lib" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/libserialport64.dll" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/sockpp64.lib" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/sockpp64.dll" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/hidapi64.lib" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/hidapi64.dll" "$<TARGET_FILE_DIR:dof_test_s>"
               )
            else()
               add_custom_command(TARGET dof_test_s POST_BUILD
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/libserialport.lib" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/libserialport.dll" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/sockpp.lib" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/sockpp.dll" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/build-libs/${PLATFORM}/${ARCH}/hidapi.lib" "$<TARGET_FILE_DIR:dof_test_s>"
                  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/hidapi.dll" "$<TARGET_FILE_DIR:dof_test_s>"
               )
            endif()
         elseif(PLATFORM STREQUAL "macos")
            add_custom_command(TARGET dof_test_s POST_BUILD
               COMMAND cp -a "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/*.dylib" "$<TARGET_FILE_DIR:dof_test_s>"
            )
         else()
            add_custom_command(TARGET dof_test_s POST_BUILD
               COMMAND cp -a "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/*.so" "$<TARGET_FILE_DIR:dof_test_s>"
               COMMAND cp -a "${CMAKE_SOURCE_DIR}/third-party/runtime-libs/${PLATFORM}/${ARCH}/*.so.*" "$<TARGET_FILE_DIR:dof_test_s>"
            )
         endif()
      endif()
   endif()
endif()
