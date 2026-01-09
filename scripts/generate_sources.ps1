# generate_sources.ps1
# Writes a sources.txt file listing every .cpp and .c under ./src with each path quoted.
# Writes UTF-8 without BOM.

Set-Location -LiteralPath (Get-Location)

# Support optional exclusion of main.cpp when invoked with --no-main or -NoMain.
$excludeMain = $false
if ($args -contains '--no-main' -or $args -contains '-NoMain') {
    $excludeMain = $true
}

$srcRoot = Join-Path (Get-Location) 'src'
$outPath = Join-Path (Get-Location) 'sources.txt'

# Collect .cpp and .c files
$files = Get-ChildItem -Recurse -Path $srcRoot -File |
    Where-Object {
        $_.Extension -in @('.cpp', '.c') -and
        (-not $excludeMain -or $_.Name -ne 'main.cpp')
    } |
    ForEach-Object {
        $p = $_.FullName -replace '\\','/'
        '"' + $p + '"'
    }

[int]$count = $files.Count
[Console]::WriteLine("Wrote $count entries to sources.txt")

# UTF-8 without BOM
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllLines($outPath, $files, $utf8NoBom)
