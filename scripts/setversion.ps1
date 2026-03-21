param(
    [string]
    $version
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    $params = $version.Split(".")

    if (3 -ne $params.Count) {
        Write-Error "Invalid error format, excepted XXX.YYY.ZZZ"
        return -1
    }

    $x = [uint]$params[0]
    $y = [uint]$params[1]
    $z = [uint]$params[2]

    $id = ($x -shl 24) -bor ($y -shl 12) -bor $z
    $ids = "{0:x}" -f $id

    Write-Host "Write version $version (0x$ids) to release\version"
    "$version`n$ids" | Out-File .\release\version

    Write-Host "Don't forget to update $( ([System.Uri](Get-Item .\release\RELEASE.MD).FullName).AbsoluteUri )"
}
finally {
    $prevPwd | Set-Location
}