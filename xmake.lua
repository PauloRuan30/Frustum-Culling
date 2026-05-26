add_rules("mode.debug", "mode.release")
set_languages("c++17")
-- Add raylib as a required package
add_requires("raylib")

target("FrustumCulling")
    set_kind("binary")
    
    -- Source files will live in a src/ folder
    add_files("src/*.cpp")
    
    -- Link the raylib package
    add_packages("raylib")

    if is_mode("debug") then
        set_symbols("debug")
        set_optimize("none")
    end