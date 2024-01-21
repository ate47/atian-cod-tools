param(
    [switch]
    $gsc,
    [switch]
    $ddl
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)

    if (!(Test-Path scripts/antlr4.jar)) {
        Invoke-WebRequest "https://www.antlr.org/download/antlr-4.13.0-complete.jar" -o scripts/antlr4.jar
    }
    
    Remove-Item -Recurse -Force .antlr4\grammar -ErrorAction Ignore > $null

    if ($gsc) {
        java -jar scripts/antlr4.jar -o .antlr4 -Dlanguage=Cpp .\grammar\gsc.g4 -no-listener -visitor -Werror
    
        New-Item -ItemType Directory src\cli\compiler -ErrorAction Ignore > $null

        Move-Item .antlr4\grammar\*.c src\cli\compiler -Force
        Move-Item .antlr4\grammar\*.cpp src\cli\compiler -Force
        Move-Item .antlr4\grammar\*.h src\cli\compiler -Force
    }
    
    if ($ddl) {
        java -jar scripts/antlr4.jar -o .antlr4 -Dlanguage=Cpp .\grammar\ddl.g4 -no-listener -visitor -Werror
    
        New-Item -ItemType Directory src\cli\ddl -ErrorAction Ignore > $null

        Move-Item .antlr4\grammar\*.c src\cli\ddl -Force
        Move-Item .antlr4\grammar\*.cpp src\cli\ddl -Force
        Move-Item .antlr4\grammar\*.h src\cli\ddl -Force
    }

}
finally {
    $prevPwd | Set-Location
}