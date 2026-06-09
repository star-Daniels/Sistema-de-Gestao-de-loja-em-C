# Sistema de Gestao de Loja

Aplicativo **desktop** de gestao de loja, escrito em **C++17** com interface
grafica **Qt 6 (Widgets)** e banco de dados **SQLite**. Controla produtos,
vendas, caixa, funcionarios, comissoes, metas e relatorios, com login e niveis
de acesso (administrador e funcionario).

## Arquitetura

Separacao em camadas — a interface nao conhece o banco, e a regra de negocio
nao conhece a tela:

- **Modelos** (`Produto`, `Usuario`, `Venda`, `LancamentoCaixa`): dados e
  pequenas regras de cada entidade.
- **`Loja`**: o nucleo. Guarda os dados em memoria e concentra as regras de
  negocio. Nao faz I/O de tela.
- **`Storage`**: a persistencia em SQLite (substitui os antigos `.txt`).
- **`MainWindow` / `LoginDialog`**: a interface Qt (toda a tela fica aqui).

```
.
├── include/                # cabecalhos (.hpp)
│   ├── Produto.hpp  Usuario.hpp  Venda.hpp  LancamentoCaixa.hpp
│   ├── Loja.hpp     Storage.hpp  Util.hpp
│   └── LoginDialog.hpp  MainWindow.hpp
├── src/                    # implementacoes (.cpp)
│   ├── main.cpp     Produto.cpp  Util.cpp
│   ├── Loja.cpp     Storage.cpp
│   └── LoginDialog.cpp  MainWindow.cpp
├── third_party/sqlite/     # SQLite amalgamation (sqlite3.c/.h) — compila junto
├── tests/selftest.cpp      # teste headless do nucleo (opcional; nao versionado)
├── build.ps1               # compila, gera dist\ e o .exe unico (um comando)
├── pack-standalone.ps1     # transforma dist\ no arquivo unico (chamado pelo build)
├── CMakeLists.txt
└── README.md
```

## Requisitos (so para compilar)

- **Qt 6** (build MinGW) + **MinGW 13.1** + **CMake** + **Ninja**.
- **7-Zip** — usado no fim do build para gerar o **arquivo unico** (.exe portatil).

Forma usada neste projeto, sem precisar de conta Qt (usa Python):

```powershell
pip install aqtinstall
aqt install-qt   windows desktop 6.8.3 win64_mingw -O C:\Qt
aqt install-tool windows desktop tools_mingw1310 qt.tools.win64_mingw1310 -O C:\Qt
aqt install-tool windows desktop tools_cmake -O C:\Qt
aqt install-tool windows desktop tools_ninja -O C:\Qt
```

7-Zip (instalador oficial, instalacao silenciosa):

```powershell
# baixa e instala em C:\Program Files\7-Zip
Invoke-WebRequest https://www.7-zip.org/a/7z2409-x64.exe -OutFile $env:TEMP\7zsetup.exe
Start-Process $env:TEMP\7zsetup.exe -ArgumentList '/S' -Wait
```

> O 7-Zip e **opcional**: sem ele o build ainda gera a pasta `dist\` normalmente,
> apenas pula o `.exe` unico. Para **executar** o programa ja compilado, nao e
> preciso instalar nada (ver "Distribuir" abaixo).

## Como compilar

Com o script (recomendado) — compila, gera a pasta `dist\` **e** o arquivo unico
`gestao-de-loja-portatil.exe`:

```powershell
.\build.ps1            # ou  .\build.ps1 -Test   (roda o teste antes)
```

Saidas do build:

- **`gestao-de-loja-portatil.exe`** (raiz do projeto) — **o arquivo unico**, ideal
  para entregar/apresentar (ver "Arquivo unico" abaixo).
- **`dist\`** — a mesma coisa em pasta (exe + DLLs), util como alternativa.

Manualmente:

```powershell
$env:Path = "C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\CMake_64\bin;C:\Qt\6.8.3\mingw_64\bin;$env:Path"
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/mingw_64" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build build --parallel
```

## Arquivo unico (portatil) — para apresentar

O build gera **`gestao-de-loja-portatil.exe`** na raiz: um **unico arquivo** que
ja contem o programa, todas as DLLs do Qt e o runtime. E so **copiar/abrir esse
arquivo** — ele roda em qualquer Windows 64-bit, sem instalar nada e sem pasta de
DLLs para se perder.

```powershell
.\gestao-de-loja-portatil.exe
```

Como funciona: ele e um **auto-extrator** (7-Zip). Ao abrir, descompacta tudo numa
pasta temporaria e ja inicia o programa.

> **Aviso:** por ser um auto-extrator sem assinatura digital, o Windows pode
> mostrar um alerta do SmartScreen ("Windows protegeu o seu PC") ou um pedido de
> permissao na primeira vez. E so clicar em **Mais informacoes -> Executar assim
> mesmo**. Em um PC sem direitos de administrador (ex.: laboratorio da escola) que
> bloqueie esse aviso, use a pasta `dist\` (abaixo), que nao pede nada.

## Como executar (pasta dist\)

Alternativa a pasta, equivalente ao arquivo unico:

```powershell
.\dist\gestao-de-loja.exe
```

### Primeiro acesso (multi-organizacao)

O sistema e **multi-organizacao**: cada conta admin tem a sua organizacao — um
espaco isolado com produtos, vendas, caixa e equipe proprios. Nao ha conta
semeada. O **login e so usuario + senha** (o usuario e unico no sistema e leva
direto a organizacao a que pertence).

1. Na tela de login, clique em **Criar conta**.
2. Informe um **usuario** e uma **senha**.
3. Voce vira o **administrador dono** de uma organizacao nova e ja entra logado.

O **nome da organizacao** comeca igual ao seu usuario e so muda depois, em
**Configuracoes**.

**Contas e cargos:** o admin cadastra funcionarios na secao **Usuarios** — eles
entram sempre como **funcionario (FUNC)** e fazem login normalmente (usuario +
senha, caindo direto na organizacao). Um funcionario vira admin **apenas por
elevacao** (*Promover a admin*); o **dono** nao pode ser rebaixado. Nomes de
usuario sao unicos no sistema inteiro.

Os dados de todas as organizacoes ficam em **`loja.db`** (SQLite), criado ao
lado do executavel.

## Distribuir para outro PC (sem instalar nada)

A pasta **`dist\`** e gerada pelo `windeployqt` e ja contem o `.exe` mais todas
as DLLs do Qt e do runtime. **Basta copiar/zipar a pasta `dist\` inteira** — ela
roda em qualquer Windows 64-bit, mesmo sem Qt instalado. (O SQLite ja esta
compilado dentro do `.exe`.)

> Quer um **unico arquivo .exe** em vez de uma pasta? Da para empacotar a pasta
> `dist\` com uma ferramenta como o **Enigma Virtual Box** (gratis), ou compilar
> o Qt em modo estatico. Para uma apresentacao, a pasta `dist\` ja resolve.

## Funcionalidades

Interface com um **rail de navegacao** a esquerda e o conteudo a direita. As
secoes aparecem conforme o cargo:

- **ADMIN:** Produtos, Vendas, Caixa, Usuarios, Relatorios, Desempenho, Perfil, Configuracoes.
- **FUNC:** Vendas, Caixa, Desempenho, Perfil, Configuracoes.

- **Produtos** — cadastro, busca, remocao, ajuste de estoque; alerta de estoque
  baixo e de validade proxima.
- **Vendas** — carrinho com varios itens; baixa de estoque automatica; a venda e
  gravada de forma **atomica** (ou tudo, ou nada).
- **Caixa** — lancamentos, saldo e fechamento do periodo (so ADMIN fecha).
- **Usuarios** — cadastro (sempre como FUNC), listagem e elevacao de cargo
  (promover/rebaixar); nao permite nome duplicado nem rebaixar o dono.
- **Perfil** — avatar, cargo, organizacao e indicadores (total vendido, comissao, meta).
- **Configuracoes** — trocar a propria senha, dados da organizacao e sobre o app.
- **Relatorios** — faturamento, lucro estimado, produtos mais vendidos e
  historico geral.
- **Desempenho** — total vendido, comissao, meta e o historico do proprio
  funcionario.

## Teste

O teste headless (`tests/selftest.cpp`) roda um cenario completo do nucleo direto
no SQLite, sem abrir a interface. **Ele nao e versionado** neste repo; o build so
compila o `selftest.exe` quando o arquivo `tests/selftest.cpp` esta presente. Se
existir:

```powershell
.\build\selftest.exe        # ou  .\build.ps1 -Test
```

---

**Resumo para entregar:** rode `.\build.ps1` e use o **`gestao-de-loja-portatil.exe`**
(arquivo unico, ja standalone). Se o PC de destino bloquear o auto-extrator
(sem admin), entregue a pasta `dist\` zipada — as duas formas dispensam instalar
qualquer coisa.
