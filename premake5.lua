
function createTestProject(_name)
  project ("test_" .. _name)
    kind "WindowedApp"
    debugdir "bin/"
    targetdir "bin/"
    includedirs {
      "rasterizer/",
      "thirdpart/",
      "test/common/",
      path.join("test", "test_" .. _name),
    }

    files {
      path.join("test", "test_" .. _name, "**.cpp"),
      path.join("test", "test_" .. _name, "**.h"),
    }

    libdirs {"lib/", "thirdpart/freeimage/"}

    links {"common", "rasterizer", "glfw", "tinyobjloader", "nanovg", "freeimage"}
    configuration "windows"
      defines { "_CRT_SECURE_NO_WARNINGS" }
      links {"opengl32.lib", "glu32.lib", "thirdpart/glew/lib/Release/Win32/glew32.lib"}

    configuration "macosx"
      buildoptions {"-std=c++11", "-msse4.1", "-Wno-deprecated-declarations"}
      links {"Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework", "Carbon.framework"}
end

location "build"
solution "rasterizer"
  configurations {"Debug", "Debug_SIMD", "Release", "Release_SIMD"}
  language "C++"
  startproject "test_hello"

  configuration "Debug or Debug_SIMD"
    defines { "DEBUG" }
    flags { "Symbols"}
    targetsuffix "_d"

  configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize"}

  group "test"
  createTestProject("hello")
  createTestProject("image")
  createTestProject("plane")
  createTestProject("light");
  createTestProject("cubemap")
  createTestProject("sponza")

  project "common"
    kind "StaticLib"
    targetdir "lib/"
    includedirs {
      "rasterizer/",
      "thirdpart/",
      "test/common/",
    }
    libdirs {"lib/", "thirdpart/freeimage/"}
    files {
      "test/common/**.h",
      "test/common/**.cpp",
      "test/common/**.hpp",
    }

    configuration "windows"
      defines { "_CRT_SECURE_NO_WARNINGS" }
      links {"opengl32.lib", "glu32.lib", "thirdpart/glew/lib/Release/Win32/glew32.lib"}

    configuration "macosx"
      buildoptions {"-std=c++11", "-msse4.1", "-Wno-deprecated-declarations"}
      links {"Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework", "Carbon.framework"}


  dofile "thirdpart.lua"

  group ""
  project "rasterizer"
    kind "StaticLib"
    targetdir "lib/"
    includedirs { "rasterizer/", "thirdpart/"}
    libdirs {"lib/", "thirdpart/freeimage/"}
    files {
      "rasterizer/**.h",
      "rasterizer/**.cpp",
      "rasterizer/**.hpp",
      "rasterizer/**.inl",
    }

    configuration "Debug_SIMD or Release_SIMD"
        defines { "_MATH_SIMD_INTRINSIC_" }

    configuration "windows"
      defines { "_CRT_SECURE_NO_WARNINGS", "_USE_GLEW_"}
      links {"opengl32.lib", "glu32.lib", "thirdpart/glew/lib/Release/Win32/glew32.lib"}

    configuration "macosx"
      buildoptions {"-std=c++11", "-msse4.1", "-Wno-deprecated-declarations"}
      links {"Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework", "Carbon.framework"}


