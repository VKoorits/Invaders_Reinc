cmake_minimum_required(VERSION 2.8)

add_definitions(--std=c++11)

# Set up your project name.
project(Invaders_Reinc)

# For QT and OpenGL
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Threads REQUIRED)

FILE(GLOB ProjectHeaders "src/*.hpp")
FILE(GLOB ProjectGameHeaders "src/gameclasses/*.hpp")
FILE(GLOB ProjectGameSources "src/gameclasses/*.cpp")
FILE(GLOB ProjectGeometryHeaders "src/gameclasses/geometry/*.hpp")
FILE(GLOB ProjectGeometrySources "src/gameclasses/geometry/*.cpp")
FILE(GLOB ProjectDataStyles "data/styles/*")
FILE(GLOB ProjectDataTranslations "data/translations/*.ts")

# Set up the main source folder.
set(SOURCE_ROOT src)
# To display subfolder and its content in QT Creator.
set(S_SOURCE_ROOT src/gameclasses)
aux_source_directory(${S_SOURCE_ROOT} SRC_LIST)
# Scan source folder to find all sources and put the result to SRC_LIST.
aux_source_directory(${SOURCE_ROOT} SRC_LIST)
qt5_wrap_cpp(QT_WRAPPED_SRC ${SRC_LIST})

# add json and xml
add_subdirectory(3party/jsoncpp)
include_directories(3party/jsoncpp/include)
aux_source_directory(3party/jsoncpp/src/lib_json JSONCPP_SRC)
add_subdirectory(3party/pugixml)
include_directories(3party/pugixml/src)
aux_source_directory(3party/pugixml/src PUGIXML_SRC)

# Create executable by SRC_LIST.
add_executable(
    ${PROJECT_NAME}
    ${SRC_LIST}
    ${QT_WRAPPED_SRC}
    ${JSONCPP_SRC}
    ${PUGIXML_SRC}
    ${ProjectHeaders}
    ${ProjectGameHeaders}
    ${ProjectGameSources}
    ${ProjectGeometryHeaders}
    ${ProjectGeometrySources}
    ${ProjectDataStyles}
    ${ProjectDataTranslations}
)

# add threading
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

# Qt modules
qt5_use_modules(${PROJECT_NAME} Widgets OpenGL Multimedia)

# Add subdirectory with Google Test Library.
add_subdirectory(3party/googletest)

# Enable unit testing.
enable_testing()
# Set up testing project name.
set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)
# Add include directories for testing project.
include_directories(3party/googletest/googletest/include ${SOURCE_ROOT})
# Set up testing project.
set(TESTS_SOURCE_ROOT tests)
# To display tests subfolder and its content in QT Creator.
set(T_S_SOURCE_ROOT tests/gameclasses_tests)
aux_source_directory(${T_S_SOURCE_ROOT} TEST_SRC_FILES)
# Scan folder to find all test sources and put the result to TEST_SRC_FILES.
aux_source_directory(${TESTS_SOURCE_ROOT} TEST_SRC_FILES)
set(TEST_SRC_FILES ${SRC_LIST} ${TEST_SRC_FILES})
list(REMOVE_ITEM TEST_SRC_FILES src/main.cpp)
qt5_wrap_cpp(QT_WRAPPED_TEST_SRC ${TEST_SRC_FILES})
add_executable(
  ${PROJECT_TEST_NAME}
  ${TEST_SRC_FILES}
  ${QT_WRAPPED_TEST_SRC}
  ${JSONCPP_SRC}
  ${PUGIXML_SRC}
)
# Link gtest and gtest_main libraries.
target_link_libraries(${PROJECT_TEST_NAME} gtest gtest_main)
qt5_use_modules(${PROJECT_TEST_NAME} Widgets OpenGL Multimedia)
# Finish tests setting up.
add_test(test ${PROJECT_TEST_NAME})

macro(copy_files GLOBPAT DESTINATION)
  file(GLOB COPY_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${GLOBPAT})
  foreach(FILENAME ${COPY_FILES})
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
    set(DST "${DESTINATION}/${FILENAME}")
    add_custom_command(TARGET ${PROJECT_TEST_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST})
  endforeach(FILENAME)
endmacro(copy_files)

set(DATA_FILES_LIST data/images/*)
copy_files("${DATA_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})
set(DATA_STARS_FILES_LIST data/images/stars/*)
copy_files("${DATA_STARS_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})
set(DATA_ALIENS_FILES_LIST data/images/aliens/*)
copy_files("${DATA_ALIENS_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})
set(DATA_BONUSES_FILES_LIST data/images/bonuses/*)
copy_files("${DATA_BONUSES_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})
set(DATA_ICONS_FILES_LIST data/images/icons/*)
copy_files("${DATA_ICONS_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})

set(DATA_AUDIO_FILES_LIST data/audio/*)
copy_files("${DATA_AUDIO_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})

set(DATA_FONT_FILES_LIST data/fonts/*)
copy_files("${DATA_FONT_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})

set(TRANSLATION_FILES_LIST data/translations/*)
copy_files("${TRANSLATION_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})

set(STYLE_FILES_LIST data/styles/*)
copy_files("${STYLE_FILES_LIST}" ${CMAKE_CURRENT_BINARY_DIR})
