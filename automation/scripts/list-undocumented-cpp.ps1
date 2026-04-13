param(
	[string]$Root = "project-rival",
	[int]$HeadLines = 80,
	[string]$Pattern = "@file"
)

$ErrorActionPreference = "Stop"

$files = Get-ChildItem -Path $Root -Recurse -Filter *.cpp | Sort-Object FullName

foreach ($f in $files)
{
	$head = Get-Content -Path $f.FullName -TotalCount $HeadLines -ErrorAction Stop
	$text = $head -join "`n"
	if ($text -notmatch [regex]::Escape($Pattern))
	{
		Write-Output $f.FullName
	}
}
