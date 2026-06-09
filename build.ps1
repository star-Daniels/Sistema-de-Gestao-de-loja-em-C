<#
  build.ps1 - compila o Sistema de Gestao de Loja e gera a pasta portavel dist\.

  Uso:
      .\build.ps1           # compila e empacota em dist\
      .\build.ps1 -Test     # compila, roda o teste do nucleo e empacota
      .\build.ps1 -Run      # compila, empacota e abre o aplicativo

  Requer Qt 6 (MinGW), CMake e Ninja. Por padrao usa a instalacao em C:\Qt
  feita via aqtinstall. Ajuste -QtDir / -ToolsDir se instalou em outro lugar.
#>
param(
    [string]$QtDir     = "C:\Qt\6.8.3\mingw_64",
    [string]$ToolsDir  = "C:\Qt\Tools",
    [string]$MingwName = "mingw1310_64",
    [switch]$Test,
    [switch]$Run
)

$ErrorActionPreference = "Stop"
$proj = $PSScriptRoot

$mingw = Join-Path $ToolsDir "$MingwName\bin"
$ninja = Join-Path $ToolsDir "Ninja"
$cmkB  = Join-Path $ToolsDir "CMake_64\bin"
$cmake = Join-Path $cmkB "cmake.exe"
$env:Path = "$mingw;$ninja;$cmkB;$QtDir\bin;" + $env:Path

Write-Host "==> Configurando (CMake + Ninja)..." -ForegroundColor Cyan
& $cmake -S $proj -B "$proj\build" -G Ninja `
    -DCMAKE_PREFIX_PATH="$QtDir" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
if ($LASTEXITCODE -ne 0) { throw "Falha no configure" }

Write-Host "==> Compilando..." -ForegroundColor Cyan
& $cmake --build "$proj\build" --parallel
if ($LASTEXITCODE -ne 0) { throw "Falha na compilacao" }

if ($Test) {
    Write-Host "==> Rodando selftest (nucleo + SQLite)..." -ForegroundColor Cyan
    & "$proj\build\selftest.exe"
    if ($LASTEXITCODE -ne 0) { throw "selftest falhou" }
}

Write-Host "==> Empacotando dist\ (windeployqt)..." -ForegroundColor Cyan
$dist = Join-Path $proj "dist"

# Encerra instancias em execucao: senao o .exe fica travado e a copia/limpeza falha.
Get-Process gestao-de-loja -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue

Remove-Item -Recurse -Force $dist -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $dist | Out-Null
Copy-Item "$proj\build\gestao-de-loja.exe" $dist

# windeployqt copia Qt6*.dll + os plugins (platforms\, styles\, imageformats\, ...).
& "$QtDir\bin\windeployqt.exe" --release --no-translations "$dist\gestao-de-loja.exe"
if ($LASTEXITCODE -ne 0) { throw "windeployqt falhou (codigo $LASTEXITCODE)" }

foreach ($dll in 'libgcc_s_seh-1.dll', 'libstdc++-6.dll', 'libwinpthread-1.dll') {
    $src = Join-Path $mingw $dll
    if (Test-Path $src) { Copy-Item $src $dist -Force }
}

# Verificacao: sem o plugin de plataforma o app nem abre ("could not load the Qt
# platform plugin windows"). Falha alto e claro em vez de gerar um dist quebrado.
if (-not (Test-Path "$dist\platforms\qwindows.dll")) {
    throw "Empacotamento incompleto: faltou dist\platforms\qwindows.dll. Rode o build de novo (sem outro build/app rodando em paralelo)."
}

Write-Host "OK -> $dist\gestao-de-loja.exe" -ForegroundColor Green
Write-Host "    (a pasta dist\ inteira e portavel: copie/zipe ela para rodar em qualquer Windows)"

# Empacota dist\ num unico .exe auto-extraivel (gestao-de-loja-portatil.exe), se o
# 7-Zip estiver instalado. E o arquivo unico, ideal para entregar/apresentar.
$sevenZip = "C:\Program Files\7-Zip\7z.exe"
if (Test-Path $sevenZip) {
    & "$proj\pack-standalone.ps1"
} else {
    Write-Host "==> 7-Zip nao instalado: pulei o arquivo unico (.exe portatil)." -ForegroundColor Yellow
    Write-Host "    Instale o 7-Zip (ver README) e rode .\pack-standalone.ps1 para gera-lo."
}

if ($Run) { & "$dist\gestao-de-loja.exe" }
