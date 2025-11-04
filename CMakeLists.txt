cmake_minimum_required(VERSION 3.10)
project(PhysicsDemo)

# Use C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories (headers for glad, GLFW, etc.)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/glad.c
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries (OpenGL + GLFW)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        opengl32
        ${CMAKE_SOURCE_DIR}/lib/glfw3.lib
)

# Copy glfw3.dll next to the built executable after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/glfw3.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Optional: Set output directory for clarity
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
