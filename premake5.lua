
function createTestProject(_name)
  project ("test_" .. _name)
    kind "ConsoleApp"
    debugdir "bin/"
    targetdir "bin/"
    includedirs {
      "softrender/",
      "thirdpart/",
      "test/common/",
      path.join("test", "test_" .. _name),
    }

    files {
      path.join("test", "test_" .. _name, "**.cpp"),
      path.join("test", "test_" .. _name, "**.h"),
    }

    libdirs {"lib/", "thirdpart/freeimage/"}

    links {"common", "softrender", "glfw", "tinyobjloader", "freeimage"}
    configuration "windows"
      defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }
      links { "opengl32.lib" }

    configuration "macosx"
      buildoptions {"-std=c++11", "-msse4.1", "-Wno-deprecated-declarations"}
      links {"Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework", "Carbon.framework"}
end

location "build"
solution "softrender"
  configurations {"Debug", "Debug_SIMD", "Release", "Release_SIMD"}
  language "C++"
  startproject "test_hello"

  configuration "Debug or Debug_SIMD"
    defines { "DEBUG" }
    flags { "Symbols"}
    targetsuffix "_d"

  configuration "Release or Release_SIMD"
    defines { "NDEBUG" }
    flags { "Optimize"}

  group "test"
  createTestProject("hello")
  createTestProject("image")
  createTestProject("plane")
  createTestProject("light");
  createTestProject("shadow")
  createTestProject("cubemap")
  createTestProject("sponza")
  createTestProject("pbr")
  createTestProject("pbr2")
  
  project "common"
    kind "StaticLib"
    targetdir "lib/"
    includedirs {
      "softrender/",
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
      defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }
      links {"opengl32.lib", "glu32.lib"}

    configuration "macosx"
      buildoptions {"-std=c++11", "-msse4.1", "-Wno-deprecated-declarations"}
      links {"Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework", "Carbon.framework"}


  dofile "thirdpart.lua"

  group ""
  project "softrender"
    kind "StaticLib"
    targetdir "lib/"
    includedirs { "softrender/", "thirdpart/"}
    libdirs {"lib/", "thirdpart/freeimage/"}
    files {
      "softrender/**.h",
      "softrender/**.cpp",
      "softrender/**.hpp",
      "softrender/**.inl",
    }

    configuration "Debug_SIMD or Release_SIMD"
        defines { "_MATH_SIMD_INTRINSIC_" }

    configuration "windows"
      defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }
      links {"opengl32.lib", "glu32.lib" }

    configuration "macosx"
      buildoptions {"-std=c++11", "-msse4.1", "-Wno-deprecated-declarations"}
      links {"Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework", "Carbon.framework"}


