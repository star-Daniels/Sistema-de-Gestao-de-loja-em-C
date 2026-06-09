<#
  pack-standalone.ps1 - transforma a pasta dist\ (exe + DLLs + plugins do Qt) em
  UM unico arquivo .exe auto-extraivel, usando o 7-Zip.

  O .exe resultante (gestao-de-loja-portatil.exe) e um auto-extrator: ao abrir,
  descompacta tudo numa pasta temporaria e ja inicia o programa. Para o usuario
  final e um clique so, sem pasta de DLLs solta para se perder.

  Uso:
      .\pack-standalone.ps1
      .\pack-standalone.ps1 -DistDir .\dist -Out .\gestao-de-loja-portatil.exe

  Requer o 7-Zip instalado (7z.exe). Veja o README.
#>
param(
    [string]$DistDir = (Join-Path $PSScriptRoot "dist"),
    [string]$Exe     = "gestao-de-loja.exe",
    [string]$Out     = (Join-Path $PSScriptRoot "gestao-de-loja-portatil.exe"),
    [string]$SevenZip = "C:\Program Files\7-Zip\7z.exe",
    [string]$SfxModule = "C:\Program Files\7-Zip\7z.sfx"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $SevenZip))   { throw "7-Zip nao encontrado em '$SevenZip'. Instale-o (ver README) ou passe -SevenZip." }
if (-not (Test-Path $SfxModule))  { throw "Modulo SFX nao encontrado em '$SfxModule'. Reinstale o 7-Zip." }
if (-not (Test-Path $DistDir))    { throw "Pasta '$DistDir' nao existe. Rode o build.ps1 antes." }
if (-not (Test-Path (Join-Path $DistDir $Exe))) { throw "Nao achei '$Exe' em '$DistDir'. Rode o build.ps1 antes." }

$arc = Join-Path $PSScriptRoot "app.7z"
$cfg = Join-Path $PSScriptRoot "sfxconfig.txt"

# 1) Compacta o conteudo de dist\ (caminhos relativos: o exe fica na raiz e os
#    plugins do Qt em platforms\, styles\, ... exatamente como o Qt espera).
Write-Host "==> Compactando dist\ ..." -ForegroundColor Cyan
if (Test-Path $arc) { [System.IO.File]::Delete($arc) }
& $SevenZip a -t7z -mx=7 $arc "$DistDir\*" | Out-Null
if ($LASTEXITCODE -ne 0) { throw "7z a falhou (codigo $LASTEXITCODE)" }

# 2) Config do auto-extrator: extrai para %TEMP% e abre o programa.
$config = @"
;!@Install@!UTF-8!
Title="Sistema de Gestao de Loja"
Progress="no"
RunProgram="$Exe"
;!@InstallEnd@!
"@
[System.IO.File]::WriteAllText($cfg, $config, (New-Object System.Text.UTF8Encoding($false)))

# 3) .exe final = modulo SFX + config + arquivo .7z (concatenados em binario).
Write-Host "==> Gerando arquivo unico ..." -ForegroundColor Cyan
$fs = [System.IO.File]::Create($Out)
try {
    foreach ($part in $SfxModule, $cfg, $arc) {
        $bytes = [System.IO.File]::ReadAllBytes($part)
        $fs.Write($bytes, 0, $bytes.Length)
    }
} finally { $fs.Close() }

# Limpeza dos intermediarios.
Remove-Item $arc, $cfg -Force -ErrorAction SilentlyContinue

$mb = [math]::Round((Get-Item $Out).Length / 1MB, 1)
Write-Host "OK -> $Out  ($mb MB) -- arquivo unico, pronto para apresentar." -ForegroundColor Green
