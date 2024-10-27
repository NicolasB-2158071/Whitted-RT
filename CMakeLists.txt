cmake_minimum_required(VERSION 3.8)

# if mvsc doesnt work
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

project("Raytracer")

# Find cmake-config for embree
set(embree_DIR ${CMAKE_SOURCE_DIR}/Lib/embree-4.3.3.x64.windows/lib/cmake/embree-4.3.3)
find_package(embree 4 REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)

add_executable(Raytracer "Src/main.cpp" "Src/Scene/Scene.h" "Src/Scene/Scene.cpp" "src/Scene/Material.h" "src/Scene/Shape.h" "Src/Raytracer.h" "Src/Raytracer.cpp" "Src/Image/Image.h"  "Src/Image/Image.cpp" "Src/Image/toojpeg.h" "Src/Image/toojpeg.cpp" "Src/Camera/Camera.h" "Src/Camera/Camera.cpp" "Src/Scene/Light.h" "Src/Utility/Random.h" "Src/Utility/Math.h" "Src/Exception/ParseException.h" "Src/Utility/SmoothingNormals.h")

target_link_libraries(Raytracer embree)
target_link_libraries(Raytracer tinyobjloader::tinyobjloader)
target_link_libraries(Raytracer spdlog::spdlog)
target_link_libraries(Raytracer glm::glm)

# Add embree dll's to output folder
add_custom_command(TARGET Raytracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Raytracer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/embree-4.3.3.x64.windows/bin/embree4.dll"
        "${CMAKE_SOURCE_DIR}/lib/embree-4.3.3.x64.windows/bin/tbb12.dll"
        "${CMAKE_SOURCE_DIR}/lib/embree-4.3.3.x64.windows/bin/tbbmalloc.dll"
        $<TARGET_FILE_DIR:Raytracer>
)