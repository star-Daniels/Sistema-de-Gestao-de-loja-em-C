#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100

/* =========================
   ESTRUTURAS
========================= */

typedef struct {
    int id;
    char nome[50];
    float preco;
    float custo;
    int estoque;
    char validade[11];
} Produto;

typedef struct {
    char usuario[50];
    char senha[20];
    char cargo[20];
} Usuario;

typedef struct {
    char tipo[20];
    float valor;
} Caixa;

typedef struct {
    int funcionarioId;
    float totalVendido;
    float comissao;
} Comissao;

typedef struct {
    int funcionarioId;
    float meta;
    float atingido;
} Meta;

typedef struct {
    int vendaId;
    int produtoId;
    int funcionarioId;
    int quantidade;
    float total;
    char data[11];
} HistoricoVenda;

typedef struct {
    int produtoId;
    int quantidadeVendida;
} ProdutoVendido;

/* =========================
   MEMÓRIA
========================= */

Produto produtos[MAX];
Usuario usuarios[MAX];
Caixa caixa[MAX];

Comissao comissoes[MAX];
Meta metas[MAX];
HistoricoVenda historico[MAX];
ProdutoVendido vendidos[MAX];

int totalProdutos = 0;
int totalUsuarios = 0;
int totalCaixa = 0;
int totalHistorico = 0;

float percentualComissao = 0.05;

int logado = -1;

/* =========================
   ARQUIVOS
========================= */

void salvarArquivo() {

    FILE *f = fopen("dados.txt", "w");
    if (!f) return;

    fprintf(f, "PRODUTOS\n");

    for (int i = 0; i < totalProdutos; i++) {

        fprintf(f, "%d;%s;%.2f;%.2f;%d;%s\n",
            produtos[i].id,
            produtos[i].nome,
            produtos[i].preco,
            produtos[i].custo,
            produtos[i].estoque,
            produtos[i].validade);
    }

    fprintf(f, "USUARIOS\n");

    for (int i = 0; i < totalUsuarios; i++) {

        fprintf(f, "%s;%s;%s\n",
            usuarios[i].usuario,
            usuarios[i].senha,
            usuarios[i].cargo);
    }

    fclose(f);
}

void carregarArquivo() {

    FILE *f = fopen("dados.txt", "r");
    if (!f) return;

    char linha[200];
    char secao[20] = "";

    while (fgets(linha, sizeof(linha), f)) {

        linha[strcspn(linha, "\n")] = 0;

        if (strcmp(linha, "PRODUTOS") == 0) {
            strcpy(secao, "PRODUTOS");
            continue;
        }

        if (strcmp(linha, "USUARIOS") == 0) {
            strcpy(secao, "USUARIOS");
            continue;
        }

        if (strcmp(secao, "PRODUTOS") == 0) {

            Produto p;

            if (sscanf(linha,
                "%d;%49[^;];%f;%f;%d;%10s",
                &p.id,
                p.nome,
                &p.preco,
                &p.custo,
                &p.estoque,
                p.validade) == 6) {

                produtos[totalProdutos++] = p;
            }
        }

        else if (strcmp(secao, "USUARIOS") == 0) {

            Usuario u;

            if (sscanf(linha,
                "%49[^;];%19[^;];%19[^\n]",
                u.usuario,
                u.senha,
                u.cargo) == 3) {

                usuarios[totalUsuarios++] = u;
            }
        }
    }

    fclose(f);
}

/* =========================
   HISTÓRICO REAL
========================= */

void salvarHistoricoVenda(HistoricoVenda h) {

    FILE *f = fopen("vendas.txt", "a");
    if (!f) return;

    fprintf(f,
        "%d;%d;%d;%d;%.2f;%s\n",
        h.vendaId,
        h.produtoId,
        h.funcionarioId,
        h.quantidade,
        h.total,
        h.data);

    fclose(f);
}

void carregarHistorico() {

    FILE *f = fopen("vendas.txt", "r");
    if (!f) return;

    HistoricoVenda h;

    while (fscanf(f,
        "%d;%d;%d;%d;%f;%10s\n",
        &h.vendaId,
        &h.produtoId,
        &h.funcionarioId,
        &h.quantidade,
        &h.total,
        h.data) == 6) {

        historico[totalHistorico++] = h;
    }

    fclose(f);
}

/* =========================
   INTERFACE
========================= */

void titulo(char *t) {

    system("cls");

    printf("========================================\n");
    printf("        SISTEMA DE LOJA\n");
    printf("========================================\n");
    printf("  %s\n", t);
    printf("========================================\n\n");
}

/* =========================
   ALERTA VALIDADE
========================= */

int produtoVencendo(char validade[]) {

    int dia, mes, ano;

    sscanf(validade, "%d/%d/%d", &dia, &mes, &ano);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int anoAtual = tm.tm_year + 1900;
    int mesAtual = tm.tm_mon + 1;
    int diaAtual = tm.tm_mday;

    if (ano < anoAtual) return 1;

    if (ano == anoAtual && mes < mesAtual)
        return 1;

    if (ano == anoAtual &&
        mes == mesAtual &&
        dia <= diaAtual + 7)
        return 1;

    return 0;
}

/* =========================
   LOGIN
========================= */

int login() {

    char u[50], s[20];

    titulo("LOGIN");

    printf("Usuario: ");
    scanf("%49s", u);

    printf("Senha: ");
    scanf("%19s", s);

    for (int i = 0; i < totalUsuarios; i++) {

        if (strcmp(usuarios[i].usuario, u) == 0 &&
            strcmp(usuarios[i].senha, s) == 0) {

            logado = i;
            return 1;
        }
    }

    logado = -1;

    printf("\nLogin invalido!\n");
    system("pause");

    return 0;
}

/* =========================
   PRODUTOS
========================= */

void listarProdutos() {

    titulo("PRODUTOS");

    for (int i = 0; i < totalProdutos; i++) {

        printf("%d - %s | R$ %.2f | estoque: %d | validade: %s\n",
            produtos[i].id,
            produtos[i].nome,
            produtos[i].preco,
            produtos[i].estoque,
            produtos[i].validade);

        if (produtos[i].estoque < 5)
            printf("⚠ Estoque baixo!\n");

        if (produtoVencendo(produtos[i].validade))
            printf("⚠ Produto vencendo!\n");
    }

    system("pause");
}

void cadastrarProduto() {

    if (totalProdutos >= MAX) return;

    titulo("CADASTRAR PRODUTO");

    Produto p;

    int maiorId = 0;

    for (int i = 0; i < totalProdutos; i++)
        if (produtos[i].id > maiorId)
            maiorId = produtos[i].id;

    p.id = maiorId + 1;

    printf("Nome: ");
    scanf(" %49s", p.nome);

    printf("Preco venda: ");
    scanf("%f", &p.preco);

    printf("Custo: ");
    scanf("%f", &p.custo);

    printf("Estoque: ");
    scanf("%d", &p.estoque);

    if (p.estoque < 0)
        p.estoque = 0;

    printf("Validade: ");
    scanf("%10s", p.validade);

    produtos[totalProdutos++] = p;

    salvarArquivo();

    printf("\nProduto cadastrado!\n");

    system("pause");
}

void buscarProduto() {

    titulo("BUSCAR PRODUTO");

    char nome[50];

    printf("Nome: ");
    scanf("%49s", nome);

    for (int i = 0; i < totalProdutos; i++) {

        if (strstr(produtos[i].nome, nome)) {

            printf("%d - %s | %.2f\n",
                produtos[i].id,
                produtos[i].nome,
                produtos[i].preco);
        }
    }

    system("pause");
}

void removerProduto() {

    titulo("REMOVER PRODUTO");

    int id;

    printf("ID: ");
    scanf("%d", &id);

    for (int i = 0; i < totalProdutos; i++) {

        if (produtos[i].id == id) {

            for (int j = i; j < totalProdutos - 1; j++)
                produtos[j] = produtos[j + 1];

            totalProdutos--;

            salvarArquivo();

            printf("Removido!\n");

            system("pause");
            return;
        }
    }

    printf("Nao encontrado!\n");
    system("pause");
}

void atualizarEstoque() {

    titulo("ATUALIZAR ESTOQUE");

    int id, qtd;

    printf("ID: ");
    scanf("%d", &id);

    printf("Novo estoque: ");
    scanf("%d", &qtd);

    for (int i = 0; i < totalProdutos; i++) {

        if (produtos[i].id == id) {

            produtos[i].estoque = qtd;

            salvarArquivo();

            printf("Atualizado!\n");

            system("pause");
            return;
        }
    }

    printf("Nao encontrado!\n");

    system("pause");
}

/* =========================
   USUÁRIOS
========================= */

void cadastrarUsuario() {

    if (totalUsuarios >= MAX) return;

    titulo("CADASTRAR USUARIO");

    Usuario u;

    printf("Usuario: ");
    scanf("%49s", u.usuario);

    printf("Senha: ");
    scanf("%19s", u.senha);

    printf("Cargo (ADMIN/FUNC): ");
    scanf("%19s", u.cargo);

    usuarios[totalUsuarios++] = u;

    salvarArquivo();

    printf("Usuario criado!\n");

    system("pause");
}

/* =========================
   VENDAS
========================= */

void vendaMultipla() {

    titulo("VENDA MULTIPLA");

    int id, qtd;
    float totalCompra = 0;

    while (1) {

        printf("ID produto (0 sai): ");
        scanf("%d", &id);

        if (id == 0)
            break;

        printf("Quantidade: ");
        scanf("%d", &qtd);

        for (int i = 0; i < totalProdutos; i++) {

            if (produtos[i].id == id) {

                if (qtd > produtos[i].estoque) {

                    printf("Estoque insuficiente!\n");
                    continue;
                }

                produtos[i].estoque -= qtd;

                float subtotal =
                    qtd * produtos[i].preco;

                totalCompra += subtotal;

                vendidos[id].produtoId = id;
                vendidos[id].quantidadeVendida += qtd;

                HistoricoVenda h;

                h.vendaId = totalHistorico + 1;
                h.produtoId = id;
                h.funcionarioId = logado;
                h.quantidade = qtd;
                h.total = subtotal;

                strcpy(h.data, "18/05/2026");

                historico[totalHistorico++] = h;

                salvarHistoricoVenda(h);
            }
        }
    }

    caixa[totalCaixa++] =
        (Caixa){"VENDA", totalCompra};

    comissoes[logado].funcionarioId = logado;

    comissoes[logado].totalVendido += totalCompra;

    comissoes[logado].comissao =
        comissoes[logado].totalVendido *
        percentualComissao;

    metas[logado].atingido =
        comissoes[logado].totalVendido;

    salvarArquivo();

    printf("\nVenda concluida!\n");
    printf("Total: %.2f\n", totalCompra);

    system("pause");
}

/* =========================
   CAIXA
========================= */

void verCaixa() {

    titulo("CAIXA");

    float total = 0;

    for (int i = 0; i < totalCaixa; i++) {

        printf("%s - %.2f\n",
            caixa[i].tipo,
            caixa[i].valor);

        total += caixa[i].valor;
    }

    printf("\nTOTAL: %.2f\n", total);

    system("pause");
}

void fecharCaixa() {

    titulo("FECHAR CAIXA");

    FILE *f = fopen("fechamento.txt", "a");

    if (!f) return;

    float total = 0;

    for (int i = 0; i < totalCaixa; i++)
        total += caixa[i].valor;

    fprintf(f,
        "FECHAMENTO: %.2f\n",
        total);

    fclose(f);

    printf("Caixa fechado!\n");

    system("pause");
}

/* =========================
   RELATÓRIOS
========================= */

void relatorioVendas() {

    titulo("RELATORIO");

    float total = 0;

    for (int i = 0; i < totalHistorico; i++) {

        printf("Venda:%d Produto:%d Func:%d Qtd:%d Total:%.2f\n",
            historico[i].vendaId,
            historico[i].produtoId,
            historico[i].funcionarioId,
            historico[i].quantidade,
            historico[i].total);

        total += historico[i].total;
    }

    printf("\nFATURAMENTO: %.2f\n", total);

    system("pause");
}

void produtosMaisVendidos() {

    titulo("PRODUTOS MAIS VENDIDOS");

    for (int i = 0; i < totalProdutos; i++) {

        if (vendidos[produtos[i].id].quantidadeVendida > 0) {

            printf("%s -> %d vendas\n",
                produtos[i].nome,
                vendidos[produtos[i].id].quantidadeVendida);
        }
    }

    system("pause");
}

void historicoFuncionario() {

    titulo("HISTORICO FUNCIONARIO");

    for (int i = 0; i < totalHistorico; i++) {

        if (historico[i].funcionarioId == logado) {

            printf("Venda:%d Produto:%d Qtd:%d Total:%.2f\n",
                historico[i].vendaId,
                historico[i].produtoId,
                historico[i].quantidade,
                historico[i].total);
        }
    }

    system("pause");
}

void lucro() {

    titulo("LUCRO");

    float lucroTotal = 0;

    for (int i = 0; i < totalHistorico; i++) {

        int id = historico[i].produtoId;

        float lucroVenda =
            historico[i].total -
            (produtos[id - 1].custo *
            historico[i].quantidade);

        lucroTotal += lucroVenda;
    }

    printf("Lucro total: %.2f\n", lucroTotal);

    system("pause");
}

/* =========================
   META / DESEMPENHO
========================= */

void definirMeta() {

    titulo("META");

    printf("Meta: ");
    scanf("%f", &metas[logado].meta);

    system("pause");
}

void desempenho() {

    titulo("DESEMPENHO");

    printf("Total vendido: %.2f\n",
        comissoes[logado].totalVendido);

    printf("Comissao: %.2f\n",
        comissoes[logado].comissao);

    printf("Meta: %.2f\n",
        metas[logado].meta);

    printf("Atingido: %.2f\n",
        metas[logado].atingido);

    if (metas[logado].atingido >=
        metas[logado].meta)

        printf("META ATINGIDA!\n");

    else
        printf("META NAO ATINGIDA!\n");

    system("pause");
}

/* =========================
   MENUS
========================= */

void menuAdmin() {

    int op;

    do {

        titulo("MENU ADMIN");

        printf("1 - Cadastrar Produto\n");
        printf("2 - Cadastrar Usuario\n");
        printf("3 - Venda\n");
        printf("4 - Caixa\n");
        printf("5 - Buscar Produto\n");
        printf("6 - Remover Produto\n");
        printf("7 - Relatorio\n");
        printf("8 - Meta\n");
        printf("9 - Desempenho\n");
        printf("10 - Mais Vendidos\n");
        printf("11 - Historico Funcionario\n");
        printf("12 - Lucro\n");
        printf("13 - Fechar Caixa\n");
        printf("14 - Atualizar Estoque\n");
        printf("15 - Listar Produtos\n");
        printf("0 - Sair\n");

        scanf("%d", &op);

        switch (op) {

            case 1: cadastrarProduto(); break;
            case 2: cadastrarUsuario(); break;
            case 3: vendaMultipla(); break;
            case 4: verCaixa(); break;
            case 5: buscarProduto(); break;
            case 6: removerProduto(); break;
            case 7: relatorioVendas(); break;
            case 8: definirMeta(); break;
            case 9: desempenho(); break;
            case 10: produtosMaisVendidos(); break;
            case 11: historicoFuncionario(); break;
            case 12: lucro(); break;
            case 13: fecharCaixa(); break;
            case 14: atualizarEstoque(); break;
            case 15: listarProdutos(); break;
        }

    } while (op != 0);
}

void menuFunc() {

    int op;

    do {

        titulo("MENU FUNC");

        printf("1 - Venda\n");
        printf("2 - Caixa\n");
        printf("3 - Historico\n");
        printf("4 - Desempenho\n");
        printf("0 - Sair\n");

        scanf("%d", &op);

        switch (op) {

            case 1: vendaMultipla(); break;
            case 2: verCaixa(); break;
            case 3: historicoFuncionario(); break;
            case 4: desempenho(); break;
        }

    } while (op != 0);
}

/* =========================
   MAIN
========================= */

int main() {

    carregarArquivo();
    carregarHistorico();

    if (totalUsuarios == 0) {

        strcpy(usuarios[0].usuario, "admin");
        strcpy(usuarios[0].senha, "123");
        strcpy(usuarios[0].cargo, "ADMIN");

        totalUsuarios = 1;

        salvarArquivo();
    }

    int op;

    do {

        system("cls");

        printf("===== SISTEMA LOJA =====\n");
        printf("1 - Login\n");
        printf("0 - Sair\n");

        scanf("%d", &op);

        if (op == 1) {

            if (login()) {

                if (strcmp(
                    usuarios[logado].cargo,
                    "ADMIN") == 0)

                    menuAdmin();

                else
                    menuFunc();
            }
        }

    } while (op != 0);

    return 0;
}