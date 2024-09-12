add_rules("mode.debug", "mode.release")
if is_mode("debug") then
    add_defines("DEBUG")
end 
add_ldflags("/subsystem:windows")
add_defines("UNICODE", "_UNICODE")

add_requires("linalg")
add_requires("imgui docking", {config = {win32=true}, debug = true})
add_requires("xxhash")
add_requires("glm")
add_requires("simpleini")
add_requires("directx-headers", {external = false})
add_requires("d3d12-memory-allocator")
add_requires("sokol")
add_requires("tinygltf")

target("sandbox")
    set_languages("c++17")
    add_files("main.cpp")
    add_files("RHI/*.cpp")
    add_files("RHI/d3d12/*.cpp")
    add_files("core/*.cpp")
    add_files("renderer/*.cpp")
    add_includedirs("utils")
    add_includedirs(".")
    add_packages("linalg")
    add_packages("glm")
    add_packages("simpleini")
    add_packages("directx-headers")
    add_packages("imgui")
    add_packages("xxhash")
    add_packages("sokol")
    add_packages("tinygltf")
    add_packages("d3d12-memory-allocator")
    if is_plat("windows") then
        add_syslinks("user32", "kernel32", "gdi32", "shell32", "dxgi", "d3d12", "d3dcompiler")
    end
    if is_mode("debug") then
    set_targetdir("/bin/debug")
    else
    set_targetdir("/bin/release")
    end