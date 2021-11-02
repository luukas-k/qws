-- premake5.lua
workspace "qws"
   configurations { "Debug", "Release" }

project "qws"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"

   links { "ws2_32" }

   files { "src/**.h", "src/**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "Off"
