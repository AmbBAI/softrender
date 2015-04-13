solution "rasterizer"
  configurations {"Debug", "Release"}
  language "C++"

  configuration "Debug"
      defines { "DEBUG" }
      flags { "Symbols"}
      targetsuffix "_d"

  configuration "Release"
      defines { "NDEBUG" }
      flags { "Optimize"}

  project "rasterizer"
    kind "ConsoleApp"
    includedirs { "rasterizer/", "thirdpart/"}
    targetdir ("bin/")
    libdirs {"lib/", "thirdpart/freeimage/"}
    files {
      "rasterizer/**.h",
      "rasterizer/**.cpp",
      "rasterizer/**.hpp",
      "rasterizer/**.inl",
    }
    defines { "_MATH_SIMD_INTRINSIC_" }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols"}
        targetsuffix "_d"
        links {"glfw_d", "tinyobjloader_d", "nanovg_d", "freeimage"}

    configuration "Release"
        defines { "NDEBUG"}
        flags { "Optimize"}
        links {"glfw", "tinyobjloader", "nanovg", "freeimage"}

    configuration "windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        links {"opengl32.lib"}

    configuration "macosx"
      buildoptions {"-std=c++11", "-msse4.1", "-Wno-deprecated-declarations"}
      links {"Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework", "Carbon.framework"}

  project "tinyobjloader"
    kind "StaticLib"
    targetdir ("lib/")
    includedirs {"thirdpart/tinyobjloader/"}
    files {
      "thirdpart/tinyobjloader/tiny_obj_loader.h",
      "thirdpart/tinyobjloader/tiny_obj_loader.cc",
    }
    
  project "glfw"
    kind "StaticLib"
    targetdir ("lib/")
    includedirs {"thirdpart/glfw/"}

    files {
      "thirdpart/glfw/include/GLFW/*.h",
      "thirdpart/glfw/src/context.c",
      "thirdpart/glfw/src/init.c",
      "thirdpart/glfw/src/input.c",
      "thirdpart/glfw/src/monitor.c",
      "thirdpart/glfw/src/window.c",
    }

    defines {"_GLFW_USE_OPENGL"}

    configuration "windows"
      defines { "_GLFW_WIN32", "_GLFW_WGL" }
      files {
          "thirdpart/glfw/src/win32*.c",
          "thirdpart/glfw/src/wgl_context.c",
          "thirdpart/glfw/src/winmm_joystick.c",
      }

    configuration "macosx"
      defines { "_GLFW_COCOA", "_GLFW_NSGL" }
      files {
          "thirdpart/glfw/src/cocoa*.m",
          "thirdpart/glfw/src/mach_time.c",
          "thirdpart/glfw/src/posix_tls.c",
          "thirdpart/glfw/src/nsgl_context.m",
          "thirdpart/glfw/src/iokit_joystick.m",
      }

  project "nanovg"
    kind "StaticLib"
    targetdir ("lib/")
    includedirs { "thirdpart/nanovg/src/" }
    files {
        "thirdpart/nanovg/src/**.h",
        "thirdpart/nanovg/src/**.c",
    }

    configuration "Debug"
      defines { "DEBUG" }
      flags { "Symbols", "ExtraWarnings"}

    configuration "Release"
      defines { "NDEBUG" }
      flags { "Optimize", "ExtraWarnings"}

