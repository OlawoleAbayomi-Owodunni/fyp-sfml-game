$BASE = "origin/main"
$out = "automation/scripts/output/enemies-merge-changes.txt"

"===== LOCAL COMMITS (origin/main..HEAD) =====`n" | Out-File $out -Encoding utf8
git log --reverse --date=short --pretty=format:"%ad %h %s%n%b%n" "$BASE..HEAD" -- >> $out

"`n===== PATCH (per commit; origin/main..HEAD) =====`n" >> $out
git log -p --reverse "$BASE..HEAD" -- . ":!*.docx" >> $out

"`n===== STAGED DIFF SUMMARY (git diff --cached --name-status) =====`n" >> $out
git diff --cached --name-status -M -C >> $out

"`n===== STAGED PATCH (git diff --cached) =====`n" >> $out
git diff --cached >> $out

# If you've done `git reset --soft origin/main`, this section captures the full squashed content.
"`n===== STAGED FILE CONTENTS (index blobs; skips deletions + binaries) =====`n" >> $out

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
} >> $out

Write-Host "Wrote: $out"