param(
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)

    acts fastfilelinker .\test\gsc-obfuscator\fastfile0\fastfile0.zone -o output_ff
    if (!$?) {
        Write-Error "Error during linking"
        return -1
    }
    acts -s ignored -w .\output_ff\package_index\fastfile0.wni -l p --seed 42 gsco .\output_ff\zone\fastfile0.ff --private .\test\gsc-obfuscator\fastfile0_privates.csv -o output_ff/obf
    if (!$?) {
        Write-Error "Error during obfuscation"
        return -1
    }
    acts -s ignored -w .\output_ff\package_index\fastfile0.wni fastfile -r bo3 '.\output_ff\obf\fastfile0.ff' -o .\output_ff\obf\decompff\ --disableScriptsDecomp
    if (!$?) {
        Write-Error "Error during fastfile dumping"
        return -1
    }
    acts -s ignored -w .\output_ff\package_index\fastfile0.wni gscd .\output_ff\obf\decompff\bo3\spt\ -o .\output_ff\obf\decompff\bo3\source\ -H -a -g
    if (!$?) {
        Write-Error "Error during gsc decompilation"
        return -1
    }
}
finally {
    $prevPwd | Set-Location
}