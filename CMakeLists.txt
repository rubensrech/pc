CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT(compilador)
FIND_PACKAGE(FLEX REQUIRED)
FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(PkgConfig REQUIRED)

# Glib stuff
# how to based on
# https://stackoverflow.com/questions/2730135/how-do-i-link-gtk-library-more-easily-with-cmake-in-windows/5105523#5105523
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

FLEX_TARGET(scanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.c)
BISON_TARGET(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c VERBOSE REPORT_FILE parser.output)

# Glib stuff
include_directories((${GLIB_INCLUDE_DIRS}))
link_directories(${GLIB_LIBRARY_DIRS})
add_definitions (${GLIB_CFLAGS_OTHER})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${compilador_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${compilador_SOURCE_DIR}/src/)
ADD_EXECUTABLE(main
  ${FLEX_scanner_OUTPUTS}
  ${BISON_parser_OUTPUTS}
  src/main.c
  src/cc_misc.c
  src/cc_sem.c
  src/cc_gv.c
  src/cc_dict.c
  src/cc_list.c
  src/cc_table.c
  src/cc_tree.c
  src/cc_ast.c
  src/cc_cod.c
)
TARGET_LINK_LIBRARIES(main ${FLEX_LIBRARIES} ${GLIB_LDFLAGS} ${GLIB_LIBRARIES})

ENABLE_TESTING()
SET(i 0)
FILE(COPY tests DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
FILE(GLOB tests "tests/e*/*tesh")
LIST(SORT tests)
FOREACH(file ${tests})
  MATH(EXPR i ${i}+1)
  STRING(REGEX REPLACE "^.*/tests/" "" etapa ${file})
  STRING(REGEX REPLACE "/.*$" "" etapa ${etapa})
  STRING(REGEX REPLACE ".*/" "" testname ${file})
  STRING(REGEX REPLACE ".tesh$" "" testname ${testname})
  ADD_TEST(${etapa}_${testname} perl -I ./tests/scripts ./tests/scripts/tesh ${file})
ENDFOREACH()

OPTION(E1 "The Etapa 1 compilation" ON)
OPTION(E2 "The Etapa 2 compilation" OFF)
OPTION(E3 "The Etapa 3 compilation" OFF)
OPTION(E4 "The Etapa 4 compilation" OFF)
OPTION(E5 "The Etapa 5 compilation" OFF)
OPTION(E6 "The Etapa 6 compilation" OFF)
OPTION(E7 "The Etapa 7 compilation" OFF)

IF(E1)
  add_definitions(-DAVALIACAO_ETAPA_1=1)
ELSEIF(E2)
  add_definitions(-DAVALIACAO_ETAPA_2=1)
ELSEIF(E3)
  add_definitions(-DAVALIACAO_ETAPA_3=1)
ELSEIF(E4)
  add_definitions(-DAVALIACAO_ETAPA_4=1)
ELSEIF(E5)
  add_definitions(-DAVALIACAO_ETAPA_5=1)
ELSEIF(E6)
  add_definitions(-DAVALIACAO_ETAPA_6=1)
ELSEIF(E7)
  add_definitions(-DAVALIACAO_ETAPA_7=1)
ENDIF(E1)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
   message(STATUS "Setting build type to 'Debug' as none was specified.")
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()
