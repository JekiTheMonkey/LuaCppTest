# Define minimal CMake version
cmake_minimum_required(VERSION 3.16)

# Define project name
project(Template)



# --- Packages finding ---

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

# Find Lua
find_package(Lua REQUIRED)



# --- Source files listing ---

# List source files
set(SOURCES 
	"src/main.cpp"
	"src/Engine.cpp"
)

# Tell CMake to build an executable
add_executable("${PROJECT_NAME}" ${SOURCES})



# --- Compilation related ---

# Define used C++ standards
set_target_properties(
	"${PROJECT_NAME}"
	PROPERTIES
	CXX_STANDARD 20
)

# Define flags to use on compilation
target_compile_options(
	"${PROJECT_NAME}" PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-conversion
    -Wsign-promo
)



# --- Including ---

# Include Lua
target_include_directories("${PROJECT_NAME}" PRIVATE "${LUA_INCLUDE_DIR}")

# Include own headers
target_include_directories("${PROJECT_NAME}" PRIVATE include)



# --- Linking ---

# Link SFML
target_link_libraries("${PROJECT_NAME}" PRIVATE sfml-graphics sfml-audio)

# Link Lua
target_link_libraries("${PROJECT_NAME}" PRIVATE "${LUA_LIBRARIES}")