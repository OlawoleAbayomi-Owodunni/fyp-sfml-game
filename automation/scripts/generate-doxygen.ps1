[CmdletBinding()]
param(
	[string] $RepoRoot = (Resolve-Path "$PSScriptRoot\..\..").Path
)

Set-Location $RepoRoot

if (-not (Get-Command doxygen -ErrorAction SilentlyContinue))
{
	throw "Doxygen not found on PATH. Install Doxygen and ensure doxygen.exe is on PATH."
}

Write-Host "Generating Doxygen docs..."
doxygen "$RepoRoot\Doxyfile"

Write-Host "Done. Output: $RepoRoot\docs\doxygen\html\index.html"