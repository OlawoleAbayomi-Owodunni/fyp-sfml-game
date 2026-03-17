param(
	[string]$Feature = "aiming",
	[string]$Main = "main",
	[string]$RemoteBase = "origin/main",
	[string]$Out = ""
)

if (-not $Out) {
	$Out = "automation/scripts/output/$Feature-merge-changes.txt"
}

New-Item -ItemType Directory -Force (Split-Path -Parent $Out) | Out-Null
git fetch origin | Out-Null

"===== LOCAL COMMITS ($RemoteBase..HEAD) =====`n" | Out-File $Out -Encoding utf8
git log --reverse --date=short --pretty=format:"%ad %h %s%n%b%n" "$RemoteBase..HEAD" -- >> $Out

"`n===== FEATURE COMMITS ($Main..$Feature) (what will be squashed) =====`n" >> $Out
git log --reverse --date=short --pretty=format:"%ad %h %s%n%b%n" "$Main..$Feature" -- >> $Out

"`n===== FEATURE PATCH (per commit; $Main..$Feature) =====`n" >> $Out
git log -p --reverse "$Main..$Feature" -- . ":!*.docx" >> $Out

"`n===== STAGED DIFF SUMMARY (git diff --cached --name-status) =====`n" >> $Out
git diff --cached --name-status -M -C >> $Out

"`n===== STAGED PATCH (git diff --cached; what will be committed on $Main) =====`n" >> $Out
git diff --cached -- . ":!*.docx" >> $Out

"`n===== STAGED FILE CONTENTS (index blobs; skips deletions + binaries) =====`n" >> $Out

$skipExt = @(
	".docx", ".pdf",
	".png", ".jpg", ".jpeg", ".gif", ".bmp",
	".ttf", ".otf",
	".wav", ".mp3", ".ogg",
	".exe", ".dll", ".pdb"
)

git diff --cached --name-status -M -C | ForEach-Object {
	$line = $_.Trim()
	if (-not $line) { return }

	$cols = $line -split "`t"
	$status = $cols[0]
	if ($status -like "D*") { return } # skip deletions

	$path =
		if ($status -like "R*" -or $status -like "C*") { $cols[2] } # new path
		else { $cols[1] }

	if (-not $path) { return }

	$ext = [System.IO.Path]::GetExtension($path).ToLowerInvariant()
	if ($skipExt -contains $ext) {
		"===== FILE: $path (skipped: $ext) ====="
		""
		return
	}

	"===== FILE: $path ====="
	git show ":$path" 2>&1
	""
} >> $Out

Write-Host "Wrote: $Out"