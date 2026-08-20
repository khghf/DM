$ErrorActionPreference = 'SilentlyContinue'

Write-Output "===== Top items inside Temp ====="
Get-ChildItem 'C:\Users\26847\AppData\Local\Temp' -Force | ForEach-Object {
    if ($_.PSIsContainer) {
        $s = (Get-ChildItem $_.FullName -Recurse -Force -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
    } else {
        $s = $_.Length
    }
    [PSCustomObject]@{ Name = $_.Name; MB = [math]::Round($s/1MB,1); Type = if($_.PSIsContainer){'DIR'}else{'FILE'} }
} | Sort-Object MB -Descending | Select-Object -First 20 | Format-Table -AutoSize

Write-Output ""
Write-Output "===== UnrealEngine subdirs ====="
Get-ChildItem 'C:\Users\26847\AppData\Local\UnrealEngine' -Force -Directory | ForEach-Object {
    $s = (Get-ChildItem $_.FullName -Recurse -Force -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
    [PSCustomObject]@{ Dir = $_.Name; MB = [math]::Round($s/1MB,1) }
} | Sort-Object MB -Descending | Select-Object -First 10 | Format-Table -AutoSize

Write-Output ""
Write-Output "===== Unity subdirs ====="
Get-ChildItem 'C:\Users\26847\AppData\Local\Unity' -Force -Directory | ForEach-Object {
    $s = (Get-ChildItem $_.FullName -Recurse -Force -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
    [PSCustomObject]@{ Dir = $_.Name; MB = [math]::Round($s/1MB,1) }
} | Sort-Object MB -Descending | Select-Object -First 10 | Format-Table -AutoSize

Write-Output ""
Write-Output "===== AMD subdirs ====="
Get-ChildItem 'C:\Users\26847\AppData\Local\AMD' -Force -Directory | ForEach-Object {
    $s = (Get-ChildItem $_.FullName -Recurse -Force -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
    [PSCustomObject]@{ Dir = $_.Name; MB = [math]::Round($s/1MB,1) }
} | Sort-Object MB -Descending | Select-Object -First 8 | Format-Table -AutoSize

Write-Output ""
Write-Output "===== .nuget top ====="
Get-ChildItem 'C:\Users\26847\.nuget' -Force -Directory | ForEach-Object {
    $s = (Get-ChildItem $_.FullName -Recurse -Force -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
    [PSCustomObject]@{ Dir = $_.Name; MB = [math]::Round($s/1MB,1) }
} | Sort-Object MB -Descending | Select-Object -First 8 | Format-Table -AutoSize
