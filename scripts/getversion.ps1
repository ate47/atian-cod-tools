param(
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    return (Get-Content release/version)[0]
}
finally {
    $prevPwd | Set-Location
}