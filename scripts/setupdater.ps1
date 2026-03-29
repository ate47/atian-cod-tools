param(
    [string]
    [ValidateNotNullOrEmpty]
    $versionEndpoint,
    [string]
    [ValidateNotNullOrEmpty]
    $zipEndpoint
)

# default:
# versionEndpoint=https://github.com/ate47/atian-cod-tools/releases/latest/download/version
# zipEndpoint=https://github.com/ate47/atian-cod-tools/releases/latest/download/acts.zip

$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)

    Write-Host "Write to release\updater"
    "$versionEndpoint`n$zipEndpoint" | Out-File .\release\updater
}
finally {
    $prevPwd | Set-Location
}