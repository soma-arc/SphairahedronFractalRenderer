sources = {
   "src/main.cpp",
}

-- premake5.lua
workspace "SphairahedronRendererWorkspace"
   configurations { "Release", "Debug" }

   platforms { "native", "x64", "x32" }

   -- Use c++11
   flags { cppdialect "C++11" }

   includedirs { "./include" }

   includedirs { "./src" }

   -- A project defines one build target
   project "SphairahedronRenderer"
      kind "ConsoleApp"
      language "C++"
      files { sources }

      if os.istarget("Linux") then
         links { "GL", "GLEW", "glfw" }
      end

      configuration "Debug"
         defines { "DEBUG" } -- -DDEBUG
         symbols "On"
         targetname "SphairahedronRenderer_debug"

      configuration "Release"
         -- defines { "NDEBUG" } -- -NDEBUG
         symbols "On"
         flags { optimize "On" }
         targetname "SphairahedronRenderer"
