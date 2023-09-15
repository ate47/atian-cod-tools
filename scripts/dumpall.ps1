param(
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)

    # Script decompilation
    .\build\bin\acts.exe gscinfo -g -o output_dump -m output_dump\scripts_hashes.csv -C "Atian COD Tools GSC decompiler test"
    .\build\bin\acts.exe dp 43 -o output_dump -m output_dump\scripts_stringtable.csv
}
finally {
    $prevPwd | Set-Location
}