param(
	[string]$Root = "project-rival",
	[int]$HeadLines = 120
)

$ErrorActionPreference = "Stop"

$headers = Get-ChildItem -Path $Root -Recurse -Include *.h,*.hpp | Sort-Object FullName

function HasDocAboveLine {
	param(
		[string[]]$Lines,
		[int]$Index
	)

	for ($j = $Index - 1; $j -ge 0 -and $j -ge ($Index - 12); $j--) {
      $line = ($Lines[$j]).TrimStart()
		if ($line -match '^\s*$') { continue }
		if ($line -match '^\s*/\*\*') { return $true }
		if ($line -match '^\s*///') { return $true }
		return $false
	}
	return $false
}

foreach ($h in $headers) {
	$lines = Get-Content -Path $h.FullName -ErrorAction Stop
	$undoc = @()

	for ($i = 0; $i -lt $lines.Count; $i++) {
		$line = $lines[$i]
		if ($line -match '^\s*(enum\s+class|enum)\b') {
			if (-not (HasDocAboveLine -Lines $lines -Index $i)) { $undoc += "line $($i+1): $line" }
		}
		elseif ($line -match '^\s*struct\b') {
			if (-not (HasDocAboveLine -Lines $lines -Index $i)) { $undoc += "line $($i+1): $line" }
		}
	}

	if ($undoc.Count -gt 0) {
		Write-Output $h.FullName
		$undoc | ForEach-Object { Write-Output "  $_" }
		Write-Output ""
	}
}
