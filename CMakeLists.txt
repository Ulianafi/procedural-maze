cmake_minimum_required(VERSION 3.15)
project(procedural_maze C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to PGR-framework (https://cent.felk.cvut.cz/courses/PGR/). By default it is
# expected next to this repository: ../pgr-framework
set(PGR_FRAMEWORK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../pgr-framework"
    CACHE PATH "Path to the PGR-framework root directory")

if(NOT EXISTS "${PGR_FRAMEWORK_ROOT}/include/pgr.h")
    message(FATAL_ERROR
        "PGR-framework not found in '${PGR_FRAMEWORK_ROOT}'. "
        "Download it and pass -DPGR_FRAMEWORK_ROOT=<path>.")
endif()

set(SOURCES
    main.cpp
    Camera.cpp   Camera.h
    Chunk.cpp    Chunk.h
    Config.cpp   Config.h
    Maze.cpp     Maze.h
    Object.cpp   ObjectAnomaly.cpp  ObjectSkyBox.cpp  Objects.h
    Player.cpp   Player.h
    Structures.cpp Structures.h
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PGR_FRAMEWORK_ROOT}/include)

if(WIN32)
    # Prebuilt 32-bit libraries shipped with the framework; configure with -A Win32.
    target_link_directories(${PROJECT_NAME} PRIVATE ${PGR_FRAMEWORK_ROOT}/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<CONFIG:Debug>,pgrd,pgr>)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        VS_DEBUGGER_ENVIRONMENT "PATH=${PGR_FRAMEWORK_ROOT}/bin;%PATH%")
else()
    # Linux / macOS: libpgr.a has to be built from ${PGR_FRAMEWORK_ROOT}/src first.
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(assimp REQUIRED)
    find_package(DevIL REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} ${IL_INCLUDE_DIR}/..)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${PGR_FRAMEWORK_ROOT}/libpgr.a
        assimp::assimp
        ${IL_LIBRARIES}
        ${GLUT_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS})
endif()

# Shaders (*.vert/*.frag) and data/ are loaded by relative paths, so the binary
# must be started from the repository root.
