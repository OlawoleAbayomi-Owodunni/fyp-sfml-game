# Writes, in order:
# 1) git diff --cached --summary
# 2) git diff --cached --name-status -M -C
# 3) staged file contents for non-deleted paths only
@(
  "===== STAGED DIFF SUMMARY (git diff --cached --summary) ====="
  git diff --cached --summary
  ""

  "===== STAGED NAME/STATUS (git diff --cached --name-status -M -C) ====="
  git diff --cached --name-status -M -C
  ""

  "===== STAGED FILE CONTENTS (index blobs; skips deletions) ====="
  ""

  # Parse name-status output and exclude deletions (D)
  (git diff --cached --name-status -M -C | ForEach-Object {
    $line = $_.Trim()
    if (-not $line) { return }

    # Format is tab-separated. Examples:
    #   M<TAB>path
    #   A<TAB>path
    #   D<TAB>path
    #   R100<TAB>old<TAB>new
    $cols = $line -split "`t"

    $status = $cols[0]
    if ($status -like "D*") { return }  # skip deletions

    $path =
      if ($status -like "R*" -or $status -like "C*") { $cols[2] } # new path
      else { $cols[1] }

    if (-not $path) { return }

    "===== FILE: $path ====="
    git show ":$path" 2>&1
    ""
  })
) | Out-File staged-files-dump.txt -Encoding utf8