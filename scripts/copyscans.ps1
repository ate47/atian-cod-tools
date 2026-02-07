param()

$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    Write-Host "-- Copy scans"
    Copy-Item -Recurse "config\scans" "build\bin\scan_paths"
}
finally {
    $prevPwd | Set-Location
}