param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,

    [Parameter(Mandatory = $true)]
    [string]$PythonExecutable
)

$ErrorActionPreference = 'Stop'

$generator = Join-Path $RepoRoot 'SteamKitCpp/tools/generate_steamlang_cpp.py'
$eresult = Join-Path $RepoRoot 'Resources/SteamLanguage/eresult.steamd'
$emsg = Join-Path $RepoRoot 'Resources/SteamLanguage/emsg.steamd'
$tempDir = Join-Path ([System.IO.Path]::GetTempPath()) ([System.Guid]::NewGuid().ToString('N'))
$output = Join-Path $tempDir 'steam_enums.hpp'

New-Item -ItemType Directory -Path $tempDir | Out-Null

try {
    & $PythonExecutable $generator --namespace steamkit::protocol --output $output $eresult $emsg
    if ($LASTEXITCODE -ne 0) {
        throw "Generator exited with code $LASTEXITCODE"
    }

    if (-not (Test-Path -LiteralPath $output)) {
        throw 'Generator did not create the output header.'
    }

    $header = Get-Content -LiteralPath $output -Raw
    if ($header -notmatch 'namespace steamkit::protocol') {
        throw 'Generated header does not use the requested namespace.'
    }
    if ($header -notmatch 'enum class EResult : std::uint32_t') {
        throw 'Generated header is missing EResult.'
    }
    if ($header -notmatch 'OK = 1,') {
        throw 'Generated header is missing EResult::OK.'
    }
    if ($header -notmatch 'ClientGamesPlayed = 742,') {
        throw 'Generated header is missing EMsg::ClientGamesPlayed.'
    }

    Write-Host 'SteamLanguage generator probe passed.'
}
finally {
    if (Test-Path -LiteralPath $tempDir) {
        Remove-Item -LiteralPath $tempDir -Recurse -Force
    }
}
