param(
    [switch]
    $gsc,
    [switch]
    $ddl,
    [switch]
    $adl
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)

    if (!(Test-Path scripts/antlr4.jar)) {
        Invoke-WebRequest "https://www.antlr.org/download/antlr-4.13.0-complete.jar" -OutFile scripts/antlr4.jar
    }
    
    Remove-Item -Recurse -Force .antlr4\grammar -ErrorAction Ignore > $null

    function AddPrecompHeader () {
        foreach ($fileItem in (Get-ChildItem .antlr4\grammar\*.cpp)) {
            "// precompiled header`n#include <includes.hpp>`n`n$((Get-Content $fileItem -Encoding UTF8) -join "`n")" | Out-File -Encoding utf8 $fileItem
        }
    }

    if ($gsc) {
        java -jar scripts/antlr4.jar -o .antlr4 -Dlanguage=Cpp .\grammar\gsc.g4 -no-listener -visitor -Werror
        AddPrecompHeader
    
        New-Item -ItemType Directory src\acts\compiler -ErrorAction Ignore > $null
        Move-Item .antlr4\grammar\*.cpp src\acts\compiler -Force
        Move-Item .antlr4\grammar\*.h src\acts\compiler -Force
    }
    
    if ($ddl) {
        java -jar scripts/antlr4.jar -o .antlr4 -Dlanguage=Cpp .\grammar\ddl.g4 -no-listener -visitor -Werror
        AddPrecompHeader
    
        New-Item -ItemType Directory src\acts\ddl -ErrorAction Ignore > $null

        Move-Item .antlr4\grammar\*.cpp src\acts\ddl -Force
        Move-Item .antlr4\grammar\*.h src\acts\ddl -Force
    }
    
    if ($adl) {
        java -jar scripts/antlr4.jar -o .antlr4 -Dlanguage=Cpp .\grammar\adl.g4 -no-listener -visitor -Werror
        AddPrecompHeader
    
        New-Item -ItemType Directory src\acts\adl -ErrorAction Ignore > $null

        Move-Item .antlr4\grammar\*.cpp src\acts\adl -Force
        Move-Item .antlr4\grammar\*.h src\acts\adl -Force
    }

}
finally {
    $prevPwd | Set-Location
}