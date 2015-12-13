Build {
    Units = function()
        local tests = Program {
            Name = "Tests",
            Sources = { "rjob.cpp" },
        }
        Default(tests)
    end,
    Configs = {
        Config {
            Name = "win64-msvc",
            DefaultOnHost = "windows",
            Tools = { { "msvc-vs2013"; TargetPlatform = "x64" }, },
            SupportedHosts = { "windows" },
        },
    },
}