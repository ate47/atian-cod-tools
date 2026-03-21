param()

$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    Write-Host "-- Copy data"
    Copy-Item -Recurse "config\data" "build\bin\Release" -ErrorAction Ignore > $null
}
finally {
    $prevPwd | Set-Location
}