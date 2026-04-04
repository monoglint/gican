# Install script for directory: C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/gican")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/clang64/bin/llvm-objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/AudioDevice.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/AudioStream.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/AutomationEventList.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/BoundingBox.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Camera2D.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Camera3D.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Color.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/FileData.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/FileText.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Font.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Functions.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Gamepad.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Image.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Keyboard.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Material.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Matrix.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Mesh.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/MeshUnmanaged.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Model.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/ModelAnimation.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Mouse.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Music.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Ray.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/RayCollision.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/RaylibException.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/raylib-cpp-utils.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/raylib-cpp.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/raylib.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/raymath.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Rectangle.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/RenderTexture.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/ShaderUnmanaged.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Shader.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Sound.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Text.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Texture.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/TextureUnmanaged.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Touch.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Vector2.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Vector3.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Vector4.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/VrStereoConfig.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Wave.hpp"
    "C:/Users/jghig/projects/cxx/gican/lib/raylib-cpp/include/Window.hpp"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/jghig/projects/cxx/gican/out/debug/lib/raylib-cpp/include/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
