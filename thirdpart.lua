
group "thirdpart"
project "tinyobjloader"
  kind "StaticLib"
  targetdir ("lib/")
  includedirs {"thirdpart/tinyobjloader/"}
  files {
    "thirdpart/tinyobjloader/tiny_obj_loader.h",
    "thirdpart/tinyobjloader/tiny_obj_loader.cc",
  }

  configuration "windows"
      defines { "_CRT_SECURE_NO_WARNINGS" }
  
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
    defines { "_GLFW_WIN32", "_GLFW_WGL", "_CRT_SECURE_NO_WARNINGS" }
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
