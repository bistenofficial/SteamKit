param(
    [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
)

$ErrorActionPreference = 'Stop'

$probeDir = Join-Path $RepoRoot '.secret-ignore-probe'
$probeFiles = @(
    'test-account.maFile',
    'test-account.mafile',
    'steam-credentials.secret.json',
    'steam-credentials.local.json',
    'steam-credentials.env'
)

try {
    New-Item -ItemType Directory -Force -Path $probeDir | Out-Null

    foreach ($name in $probeFiles) {
        $path = Join-Path $probeDir $name
        Set-Content -LiteralPath $path -Value 'probe only; no real secrets' -NoNewline
    }

    foreach ($name in $probeFiles) {
        $relativePath = ".secret-ignore-probe/$name"
        & git -C $RepoRoot check-ignore -q -- $relativePath
        if ($LASTEXITCODE -ne 0) {
            throw "Expected $relativePath to be ignored by git"
        }
    }

    Write-Host 'Secret ignore probe passed.'
}
finally {
    if (Test-Path -LiteralPath $probeDir) {
        Remove-Item -LiteralPath $probeDir -Recurse -Force
    }
}
