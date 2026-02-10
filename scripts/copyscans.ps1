param()

$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    Write-Host "-- Copy scans"
    Copy-Item -Recurse "config\games" "build\bin" -ErrorAction Ignore > $null
}
finally {
    $prevPwd | Set-Location
}