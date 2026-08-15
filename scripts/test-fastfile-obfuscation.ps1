param(
    $Game = "bo3",
    $Seed = 42,
    [switch]
    $NoDebug,
    [switch]
    $NoLocals,
    [switch]
    $NoPrivate,
    [switch]
    $NoTrampoline,
    [switch]
    $NoDevOpReplace,
    [switch]
    $RecompCrc,
    [switch]
    $DecompIgnoreDevBlocks
)


$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)

    $gscoOptions = @( "--dump-scripts" )
    $gscdOptions = @( "-H", "-a", "-g", "--internalblocks" )

    if ($NoDebug) { $gscoOptions += @( "--no-debug" ) }
    if ($NoLocals) { $gscoOptions += @( "--no-locals" ) }
    if ($NoPrivate) { $gscoOptions += @( "--no-private" ) }
    if ($NoTrampoline) { $gscoOptions += @( "--no-trampoline" ) }
    if ($RecompCrc) { $gscoOptions += @( "--export-crc-recomp" ) }
    if ($NoDevOpReplace) { $gscoOptions += @( "--no-devop-replace" ) }
    if ($DecompIgnoreDevBlocks) { $gscdOptions += @( "--ignore-devblocks" ) }

    $actsArgs = @("-t", "-s", "ignored", "-w", ".\output_ff\package_index\fastfile0.wni", "--seed", "$Seed")

    acts -l i fastfilelinker .\test\gsc-obfuscator\fastfile0\fastfile0.zone -o output_ff "-DOBF_GAME=$Game"
    acts @actsArgs -l p gsco .\output_ff\zone\fastfile0.ff --private .\test\gsc-obfuscator\fastfile0_privates.csv -o .\output_ff\obf @gscoOptions
    acts @actsArgs -l i fastfile -r $Game '.\output_ff\obf\fastfile0.ff' -o .\output_ff\obf\decompff\ --disableScriptsDecomp
    acts @actsArgs -l i gscd .\output_ff\obf\decompff\$Game\spt\ -o .\output_ff\obf\decompff\$Game\source\ @gscdOptions
}
finally {
    $prevPwd | Set-Location
}
