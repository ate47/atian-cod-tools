param()

$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    Write-Host "-- Copy scans"
    Copy-Item -Recurse "configs\scans" "build\bin"
}
finally {
    $prevPwd | Set-Location
}