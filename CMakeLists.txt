cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-parentheses-equality -Wno-unused-variable -Wno-unused-function")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(FastBoi)

file(GLOB SRC CONFIGURE_DEPENDS src/*.cpp src/*.hpp src/UI/*.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

file(GLOB SOLOUD_CORE_SRC deps/soloud/src/core/*.cpp deps/soloud/src/audiosource/wav/*.cpp deps/soloud/src/audiosource/wav/*.c)
set(THIRDPARTY_SRC ${SOLOUD_CORE_SRC} deps/soloud/src/backend/sdl2_static/soloud_sdl2_static.cpp)

file(GLOB APP_SRC src/adventure/*.cpp)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(adventure ${SRC} ${THIRDPARTY_SRC} ${APP_SRC})
set_target_properties(adventure PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_compile_options(adventure PRIVATE -flto=thin)
target_link_options(adventure PRIVATE -fuse-ld=lld -flto=thin)
target_include_directories(adventure PRIVATE inc src inc/soloud inc/SDL)

target_link_libraries(adventure PRIVATE SDL2 SDL2main SDL2_image)

target_compile_definitions(adventure PRIVATE WITH_SDL2_STATIC __cpp_lib_ranges)


file(GLOB MC_SRC src/minecraft/*.cpp)
add_executable(minecraft ${SRC} ${THIRDPARTY_SRC} ${MC_SRC})

target_compile_options(minecraft PRIVATE -flto=thin)
target_link_options(minecraft PRIVATE -fuse-ld=lld -flto=thin)
target_include_directories(minecraft PRIVATE inc src inc/soloud inc/SDL)

target_link_libraries(minecraft PRIVATE SDL2 SDL2main SDL2_image)

target_compile_definitions(minecraft PRIVATE WITH_SDL2_STATIC __cpp_lib_ranges)





