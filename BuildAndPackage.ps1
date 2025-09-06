<#!
.SYNOPSIS
 Builds the solution (Release|x64) and creates distribution zip files.
.DESCRIPTION
 1. Builds LargePageInjectorMods.sln (Release|x64) unless -SkipBuild specified.
 2. Produces three zip archives in x64\Zips:
      FactorioLargePageInjector.zip
      StellarisLargePageInjectorGOG.zip
      StellarisLargePageInjectorSteam.zip
    Each archive contains the compiled binaries from x64\Release excluding *.lib and *.exp
    plus the appropriate configuration preset (renamed to LargePageInjectorMods.toml) and
    optional launcher settings (renamed to launcher_settings.json).
 3. Easily extensible by editing the $distributions array.
#>
[CmdletBinding()]
param(
  [string]$Solution = 'LargePageInjectorMods.sln',
  [string]$Configuration = 'Release',
  [string]$Platform = 'x64',
  [switch]$SkipBuild,
  [switch]$VerboseCopy
)

$ErrorActionPreference = 'Stop'

function Get-MSBuildPath {
  # Try environment first
  $candidates = @()
  if ($env:MSBUILD_EXE_PATH) { $candidates += $env:MSBUILD_EXE_PATH }

  # vswhere (installed with recent VS)
  $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\\Installer\\vswhere.exe'
  if (Test-Path $vswhere) {
    $msbuilds = & $vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\\**\\Bin\\MSBuild.exe 2>$null
    $candidates += $msbuilds
  }

  # Fallback to PATH
  $candidates += 'msbuild.exe'

  foreach ($c in $candidates | Get-Unique) {
    if (Get-Command $c -ErrorAction SilentlyContinue) { return (Get-Command $c).Source }
  }
  throw 'MSBuild not found. Install Visual Studio Build Tools or specify MSBUILD_EXE_PATH.'
}

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $root
try {
  if (-not (Test-Path $Solution)) { throw "Solution '$Solution' not found in $root" }

  if (-not $SkipBuild) {
    Write-Host '==> Building solution...'
    $msbuild = Get-MSBuildPath
    & $msbuild $Solution /t:Build /p:Configuration=$Configuration /p:Platform=$Platform /m /nologo /v:m
  } else {
    Write-Host '==> Skipping build (per parameter)'
  }

  $releaseDir = Join-Path $root "$Platform/$Configuration"
  if (-not (Test-Path $releaseDir)) { throw "Release directory not found: $releaseDir" }

  $zipOutDir = Join-Path $root "$Platform/Zips"
  if (-not (Test-Path $zipOutDir)) { New-Item -ItemType Directory -Path $zipOutDir | Out-Null }

  # Distribution definitions (extendable)
  $distributions = @(
    @{ Name='Factorio';          Zip='FactorioLargePageInjector.zip';       Config='configs/Factorio_LargePageInjectorMods.toml';       Launcher=$null },
    @{ Name='StellarisGOG';      Zip='StellarisLargePageInjectorGOG.zip';   Config='configs/Stellaris_LargePageInjectorMods.toml';      Launcher='configs/StellarisGOG_launcher-settings.json' },
    @{ Name='StellarisSteam';    Zip='StellarisLargePageInjectorSteam.zip'; Config='configs/Stellaris_LargePageInjectorMods.toml';      Launcher='configs/StellarisSteam_launcher-settings.json' }
  )

  foreach ($d in $distributions) {
    Write-Host "==> Packaging $($d.Name)"
    $staging = Join-Path $env:TEMP ("LPIM_" + [System.IO.Path]::GetRandomFileName())
    New-Item -ItemType Directory -Path $staging | Out-Null

    try {
      # Copy all release content except *.lib / *.exp
      Get-ChildItem -Path $releaseDir -Recurse | ForEach-Object {
        if ($_.PSIsContainer) {
          $rel = $_.FullName.Substring($releaseDir.Length).TrimStart('\\/')
          $destDir = Join-Path $staging $rel
          if (-not (Test-Path $destDir)) { New-Item -ItemType Directory -Path $destDir | Out-Null }
        } else {
          if ($_.Extension -in '.lib', '.exp') { return }
          $rel = $_.FullName.Substring($releaseDir.Length).TrimStart('\\/')
          $dest = Join-Path $staging $rel
          $destDir = Split-Path -Parent $dest
          if (-not (Test-Path $destDir)) { New-Item -ItemType Directory -Path $destDir | Out-Null }
          Copy-Item $_.FullName -Destination $dest -Force
          if ($VerboseCopy) { Write-Host "Copied: $rel" }
        }
      }

      # Config preset
      $configSource = Join-Path $root $d.Config
      if (-not (Test-Path $configSource)) { throw "Config file missing: $configSource" }
      Copy-Item $configSource (Join-Path $staging 'LargePageInjectorMods.toml') -Force

      # Optional launcher settings
      if ($d.Launcher) {
        $launcherSource = Join-Path $root $d.Launcher
        if (-not (Test-Path $launcherSource)) { throw "Launcher settings missing: $launcherSource" }
        Copy-Item $launcherSource (Join-Path $staging 'launcher_settings.json') -Force
      }

      $zipPath = Join-Path $zipOutDir $d.Zip
      if (Test-Path $zipPath) { Remove-Item $zipPath -Force }

      # Compress contents (not the staging folder itself)
      Compress-Archive -Path (Join-Path $staging '*') -DestinationPath $zipPath -Force
      Write-Host "Created: $zipPath"
    }
    finally {
      if (Test-Path $staging) { Remove-Item $staging -Recurse -Force }
    }
  }

  Write-Host '==> All distributions packaged successfully.'
}
finally {
  Pop-Location
}
