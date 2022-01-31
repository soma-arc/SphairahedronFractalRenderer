sources = {
   "src/main.cpp",
   "src/sphairahedron.cpp",
   "src/cube/cube.cpp",
   "src/cake/cake.cpp",
   "src/pentahedralPrism/pentahedralPrism.cpp",
   "src/sphere.cpp",
   "src/plane.cpp"
}

-- premake5.lua
workspace "SphairahedronRendererWorkspace"
   configurations { "Release", "Debug" }

   platforms { "native", "x64", "x32" }

   -- Use c++14
   flags { cppdialect "C++14" }

   includedirs { "./include" }

   includedirs { "./src" }

   -- A project defines one build target
   project "SphairahedronRenderer"
      kind "ConsoleApp"
      language "C++"
      files { sources }
      libdirs { "/usr/local/lib/static" }
      if os.istarget("Linux") then
         links { "GL", "GLEW", "glfw", "pthread", "jinja2cpp" }
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
