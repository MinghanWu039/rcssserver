
bison_target(player_command_parser
    player_command_parser.ypp
    ${CMAKE_CURRENT_BINARY_DIR}/player_command_parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/player_command_parser.hpp
)
flex_target(player_command_tokenizer
    player_command_tok.lpp
    ${CMAKE_CURRENT_BINARY_DIR}/raw_player_command_tok.cpp
)
add_flex_bison_dependency(player_command_tokenizer player_command_parser)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/player_command_tok.cpp"
  COMMAND ${CMAKE_COMMAND}
  ARGS
    "-DGENERATED_FILE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/raw_player_command_tok.cpp\""
    "-DCORRECT_HEADER_NAME=player_command_tok.h"
    "-DOUTPUT_FILE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/player_command_tok.cpp\""
    "-P" "${CMAKE_CURRENT_SOURCE_DIR}/fix_lexer_file.cmake"
  MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}/raw_player_command_tok.cpp"
)

option(RCSSSERVER_PYTHON_PACKAGE_ONLY "Build only the embedded Python package targets" OFF)

set(RCSSSERVER_CORE_SOURCES
    audio.cpp
    bodysender.cpp
    coach.cpp
    csvsaver.cpp
    dispsender.cpp
    embedded_simulator/embeddedsimulator.cpp
    field.cpp
    fullstatesender.cpp
    heteroplayer.cpp
    initsender.cpp
    initsendercoach.cpp
    initsenderlogger.cpp
    initsendermonitor.cpp
    initsenderonlinecoach.cpp
    initsenderplayer.cpp
    landmarkreader.cpp
	leg.cpp
    logger.cpp
    monitor.cpp
    pcombuilder.cpp
    pcomparser.cpp
    player.cpp
    playerparam.cpp
    object.cpp
    referee.cpp
    remoteclient.cpp
    resultsaver.cpp
    serializer.cpp
    serializercoachstdv1.cpp
    serializercoachstdv7.cpp
    serializercoachstdv8.cpp
    serializercoachstdv13.cpp
    serializercoachstdv14.cpp
	serializercommonstdv1.cpp
    serializercommonstdv7.cpp
    serializercommonstdv8.cpp
    serializercommonjson.cpp
    serializermonitor.cpp
    serializeronlinecoachstdv1.cpp
    serializeronlinecoachstdv6.cpp
    serializeronlinecoachstdv7.cpp
    serializeronlinecoachstdv8.cpp
    serializeronlinecoachstdv13.cpp
    serializeronlinecoachstdv14.cpp
    serializerplayerstdv1.cpp
    serializerplayerstdv5.cpp
    serializerplayerstdv7.cpp
    serializerplayerstdv8.cpp
    serializerplayerstdv13.cpp
    serializerplayerstdv14.cpp
	serializerplayerstdv18.cpp
    serverparam.cpp
    stadium.cpp
    stdoutsaver.cpp
    stdtimer.cpp
    synctimer.cpp
    team.cpp
    utility.cpp
    visualsendercoach.cpp
    visualsenderplayer.cpp
    weather.cpp
    xmlreader.cpp
    xpmholder.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/player_command_parser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/player_command_tok.cpp
)

set(RCSSSERVER_VCPKG_MATPLOTLIB_HINTS)
if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg[.]cmake$")
  get_filename_component(RCSSSERVER_VCPKG_BUILDSYSTEM_DIR "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
  get_filename_component(RCSSSERVER_VCPKG_SCRIPTS_DIR "${RCSSSERVER_VCPKG_BUILDSYSTEM_DIR}" DIRECTORY)
  get_filename_component(RCSSSERVER_VCPKG_ROOT "${RCSSSERVER_VCPKG_SCRIPTS_DIR}" DIRECTORY)
  file(GLOB RCSSSERVER_VCPKG_MATPLOTLIB_HINTS "${RCSSSERVER_VCPKG_ROOT}/installed/*/include")
endif()

find_path(MATPLOTLIB_CPP_INCLUDE_DIRS
  "matplotlibcpp.h"
  HINTS
    ${RCSSSERVER_VCPKG_MATPLOTLIB_HINTS}
    "$ENV{VCPKG_ROOT}/installed/x64-linux/include"
    "$ENV{VCPKG_ROOT}/installed/x86-linux/include"
    "$ENV{HOME}/vcpkg/installed/x64-linux/include"
    "$ENV{HOME}/vcpkg/installed/x86-linux/include"
)
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

if(NOT pybind11_DIR)
  find_path(RCSSSERVER_PYBIND11_CMAKE_DIR
    "pybind11Config.cmake"
    HINTS
      "$ENV{CONDA_PREFIX}/share/cmake/pybind11"
      "$ENV{HOME}/miniconda3/share/cmake/pybind11"
      "$ENV{HOME}/miniconda3/pkgs/pybind11-global-3.0.1-pyhc7ab6ef_0/share/cmake/pybind11"
      "$ENV{HOME}/miniconda3/pkgs/pybind11-3.0.1-pyh7a1b43c_0/site-packages/pybind11/share/cmake/pybind11"
  )
  if(RCSSSERVER_PYBIND11_CMAKE_DIR)
    set(pybind11_DIR "${RCSSSERVER_PYBIND11_CMAKE_DIR}" CACHE PATH "pybind11 CMake config directory")
  endif()
endif()

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

set(RCSSSERVER_EMBEDDED_SMOKE_RPATH
  "${PROJECT_BINARY_DIR}/rcss/clang"
  "${PROJECT_BINARY_DIR}/rcss/conf"
  "${PROJECT_BINARY_DIR}/rcss/net"
  "${PROJECT_BINARY_DIR}/rcss/gzip"
  "/usr/lib/x86_64-linux-gnu"
  "/lib/x86_64-linux-gnu"
)

if(TARGET Python3::Python)
  get_target_property(RCSSSERVER_PYTHON3_IMPORTED_LOCATION Python3::Python IMPORTED_LOCATION)
  if(RCSSSERVER_PYTHON3_IMPORTED_LOCATION)
    get_filename_component(RCSSSERVER_PYTHON3_LIBRARY_DIR "${RCSSSERVER_PYTHON3_IMPORTED_LOCATION}" DIRECTORY)
    list(APPEND RCSSSERVER_EMBEDDED_SMOKE_RPATH "${RCSSSERVER_PYTHON3_LIBRARY_DIR}")
  endif()
endif()

set(RCSSSERVER_PYTHON_MODULE_RPATH
  "$ORIGIN/rcss/clang"
  "$ORIGIN/rcss/conf"
  "$ORIGIN/rcss/net"
  "$ORIGIN/rcss/gzip"
)

set(RCSSSERVER_EMBEDDED_BUNDLE_DIR "${PROJECT_BINARY_DIR}/embedded_sim_bundle")
set(RCSSSERVER_PYTHON_PACKAGE_DIR "rcssserver_embedded")
set(RCSSSERVER_PYTHON_PACKAGE_BUILD_DIR "${PROJECT_BINARY_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}")

file(MAKE_DIRECTORY "${RCSSSERVER_PYTHON_PACKAGE_BUILD_DIR}")
configure_file(
  "${PROJECT_SOURCE_DIR}/python/rcssserver_embedded/__init__.py"
  "${RCSSSERVER_PYTHON_PACKAGE_BUILD_DIR}/__init__.py"
  COPYONLY
)



if(NOT RCSSSERVER_PYTHON_PACKAGE_ONLY)
  add_executable(RCSSServer
      ${RCSSSERVER_CORE_SOURCES}
      main.cpp
  )

  target_link_libraries(RCSSServer
    PRIVATE
      RCSS::Flex
      RCSS::CLangParser
      RCSS::ConfParser
      RCSS::Net
      RCSS::GZ
      ZLIB::ZLIB
  	)

  target_compile_definitions(RCSSServer
    PUBLIC
      HAVE_CONFIG_H
  )

  target_compile_options(RCSSServer
    PRIVATE
      -W -Wall
  )

  set_target_properties(RCSSServer
    PROPERTIES
      RUNTIME_OUTPUT_NAME "rcssserver"
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
  )

  add_executable(EmbeddedSimulatorSmoke
      ${RCSSSERVER_CORE_SOURCES}
      embedded_simulator/embeddedsimulator_smoke.cpp
  )

  target_link_libraries(EmbeddedSimulatorSmoke
    PRIVATE
      RCSS::Flex
      RCSS::CLangParser
      RCSS::ConfParser
      RCSS::Net
      RCSS::GZ
      ZLIB::ZLIB
  )

  if(MATPLOTLIB_CPP_INCLUDE_DIRS AND Python3_Development_FOUND AND Python3_NumPy_FOUND)
    target_include_directories(EmbeddedSimulatorSmoke
      PRIVATE
        ${MATPLOTLIB_CPP_INCLUDE_DIRS}
        ${Python3_NumPy_INCLUDE_DIRS}
    )
    target_link_libraries(EmbeddedSimulatorSmoke
      PRIVATE
        Python3::Python
    )
    target_compile_definitions(EmbeddedSimulatorSmoke
      PRIVATE
        RCSSSERVER_HAVE_MATPLOTLIB_CPP=1
    )
  else()
    target_compile_definitions(EmbeddedSimulatorSmoke
      PRIVATE
        RCSSSERVER_HAVE_MATPLOTLIB_CPP=0
    )
  endif()

  target_compile_definitions(EmbeddedSimulatorSmoke
    PUBLIC
      HAVE_CONFIG_H
  )

  target_compile_options(EmbeddedSimulatorSmoke
    PRIVATE
      -W -Wall
  )

  set_target_properties(EmbeddedSimulatorSmoke
    PROPERTIES
      RUNTIME_OUTPUT_NAME "embeddedsimulator_smoke"
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
      BUILD_RPATH "${RCSSSERVER_EMBEDDED_SMOKE_RPATH}"
  )
endif()

pybind11_add_module(rcssserver_embedded_core
    ${RCSSSERVER_CORE_SOURCES}
    embedded_simulator/pybind_embeddedsimulator.cpp
)

target_link_libraries(rcssserver_embedded_core
  PRIVATE
    RCSS::Flex
    RCSS::CLangParser
    RCSS::ConfParser
    RCSS::Net
    RCSS::GZ
    ZLIB::ZLIB
)

target_compile_definitions(rcssserver_embedded_core
  PUBLIC
    HAVE_CONFIG_H
)

target_compile_options(rcssserver_embedded_core
  PRIVATE
    -W -Wall
)

set_target_properties(rcssserver_embedded_core
  PROPERTIES
    OUTPUT_NAME "_core"
    LIBRARY_OUTPUT_DIRECTORY ${RCSSSERVER_PYTHON_PACKAGE_BUILD_DIR}
    BUILD_RPATH "${RCSSSERVER_PYTHON_MODULE_RPATH}"
    INSTALL_RPATH "${RCSSSERVER_PYTHON_MODULE_RPATH}"
    BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_target(EmbeddedSimulatorBundle
  DEPENDS
    rcssserver_embedded_core
    RCSSCLangParser
    RCSSConfParser
    RCSSNet
    RCSSGZ
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/clang"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/conf"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/net"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/gzip"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/python/rcssserver_embedded/__init__.py"
    "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:rcssserver_embedded_core>
    "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/demo.py"
    "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/EMBEDDED_SIMULATOR_INTERFACE.md"
    "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:RCSSCLangParser>
    "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/clang/"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:RCSSConfParser>
    "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/conf/"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:RCSSNet>
    "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/net/"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:RCSSGZ>
    "${RCSSSERVER_EMBEDDED_BUNDLE_DIR}/${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/gzip/"
  VERBATIM
)


if(NOT RCSSSERVER_PYTHON_PACKAGE_ONLY)
  add_executable(RCSSClient
      client.cpp
  )

  target_link_libraries(RCSSClient
    PRIVATE
      RCSS::Net
      RCSS::GZ
      ZLIB::ZLIB
  )

  set_target_properties(RCSSClient
    PROPERTIES
      RUNTIME_OUTPUT_NAME "rcssclient"
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
  )

  set(prefix ${CMAKE_INSTALL_PREFIX})
  set(exec_prefix ${CMAKE_INSTALL_PREFIX})
  set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
  configure_file(rcsoccersim.in rcsoccersim @ONLY)

  install(TARGETS RCSSServer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT Libraries
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )

  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/rcsoccersim DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

install(FILES
  "${PROJECT_SOURCE_DIR}/python/rcssserver_embedded/__init__.py"
  DESTINATION "${RCSSSERVER_PYTHON_PACKAGE_DIR}"
)

install(TARGETS rcssserver_embedded_core
  LIBRARY DESTINATION "${RCSSSERVER_PYTHON_PACKAGE_DIR}"
)

install(TARGETS RCSSCLangParser
  LIBRARY DESTINATION "${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/clang"
)

install(TARGETS RCSSConfParser
  LIBRARY DESTINATION "${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/conf"
)

install(TARGETS RCSSNet
  LIBRARY DESTINATION "${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/net"
)

install(TARGETS RCSSGZ
  LIBRARY DESTINATION "${RCSSSERVER_PYTHON_PACKAGE_DIR}/rcss/gzip"
)
