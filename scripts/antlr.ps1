param(
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)

    if (!(Test-Path scripts/antlr4.jar)) {
        Invoke-WebRequest "https://www.antlr.org/download/antlr-4.13.0-complete.jar" -o scripts/antlr4.jar
    }
    
    java -jar scripts/antlr4.jar -o .antlr4 -Dlanguage=Cpp .\grammar\gsc.g4 -no-listener -visitor -Werror

    Move-Item .antlr4\grammar\*.c src\compiler -Force
    Move-Item .antlr4\grammar\*.cpp src\compiler -Force
    Move-Item .antlr4\grammar\*.h src\compiler -Force
}
finally {
    $prevPwd | Set-Location
}