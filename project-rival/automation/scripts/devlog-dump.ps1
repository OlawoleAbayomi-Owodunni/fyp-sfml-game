param(
	[string]$SinceHash = "",
	[string]$Out = "",
	[string]$DevLogPath = "",
	[switch]$IncludeStaged
)

$ErrorActionPreference = "Stop"

function Write-Section([string]$title) {
	"'n===== $title ====='n" | Out-File -FilePath $Out -Append -Encoding utf8
}

# Resolve output path
if (-not $Out) {
	$stamp = Get-Date -Format "yyyyMMdd-HHmmss"
	$Out = "automation/scripts/output/devlog-dump-$stamp.txt"
}

New-Item -ItemType Directory -Force (Split-Path -Parent $Out) | Out-Null

# Resolve devlog path (try common locations)
if (-not $DevLogPath) {
	$candidates = @(
		"docs/DEVELOPMENT_LOG.md",
		"project-rival/docs/DEVELOPMENT_LOG.md"
	)
	foreach ($c in $candidates) {
		if (Test-Path $c) {
			$DevLogPath = $c
			break
		}
	}
}

"===== DEVLOG DUMP (for generating a new entry) ====='n" | Out-File -FilePath $Out -Encoding utf8

Write-Section "REPO CONTEXT"
try {
	"Timestamp: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" | Out-File $Out -Append
	"Repo root: $(git rev-parse --show-toplevel)" | Out-File $Out -Append
	"Branch: $(git branch --show-current)" | Out-File $Out -Append
	"HEAD: $(git rev-parse --short HEAD)" | Out-File $Out -Append
	"" | Out-File $Out -Append
	git status -sb | Out-File $Out -Append
}
catch {
	"(git info failed) $_" | Out-File $Out -Append
}

Write-Section "DEVLOG"
if ($DevLogPath -and (Test-Path $DevLogPath)) {
	"Devlog path: $DevLogPath" | Out-File $Out -Append

	if (-not $SinceHash) {
		# Heuristic: take the FIRST backticked hash in the file (devlog entries are typically newest-first)
		$content = Get-Content -Raw -Path $DevLogPath
		$m = [regex]::Match($content, "'([0-9a-f]{7,40})'")
		if ($m.Success) {
			$SinceHash = $m.Groups[1].Value
		}
	}

	"Detected SinceHash: $SinceHash" | Out-File $Out -Append
}
else {
	"Devlog path not found. Pass -DevLogPath explicitly." | Out-File $Out -Append
	"Detected SinceHash: $SinceHash" | Out-File $Out -Append
}

Write-Section "WHY / INTENT (fill in)"
"Paste 2-5 bullets here when you send this output:" | Out-File $Out -Append
"- " | Out-File $Out -Append

Write-Section "COMMITS SINCE LAST DEVLOG ENTRY"
if (-not $SinceHash) {
	"No SinceHash detected. Re-run with: .\\automation\\scripts\\devlog-dump.ps1 -SinceHash <hash>" | Out-File $Out -Append
}
else {
	git fetch origin | Out-Null
	$range = "$SinceHash..HEAD"
	"Range: $range" | Out-File $Out -Append
	"" | Out-File $Out -Append

	# Commit summary + touched files per commit
	git log --date=short --reverse --name-status --pretty=format:"%ad %h %s%n%b" $range -- ":!*.docx" | Out-File $Out -Append
}

if ($IncludeStaged) {
	Write-Section "STAGED CHANGES (if you are preparing a squash commit)"
	git diff --cached --name-status -M -C | Out-File $Out -Append
	"" | Out-File $Out -Append
	git diff --cached --stat | Out-File $Out -Append
}

Write-Host "Wrote: $Out"