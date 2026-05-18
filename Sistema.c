#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100
//add


typedef struct {
    int id;
    char nome[50];
    float preco;
    int estoque;
    char validade[11];
} Produto;

typedef struct {
    char usuario[50];
    char senha[20];
    char cargo[20];
} Usuario;

typedef struct {
    char tipo[10];
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

/* =========================
   MEMÓRIA
========================= */

Produto produtos[MAX];
Usuario usuarios[MAX];
Caixa caixa[MAX];

Comissao comissoes[MAX];
Meta metas[MAX];
HistoricoVenda historico[MAX];

int totalProdutos = 0;
int totalUsuarios = 0;
int totalCaixa = 0;

int totalComissoes = 0;
int totalMetas = 0;
int totalHistorico = 0;

float percentualComissao = 0.05;

int logado = -1;

/* =========================
   ARQUIVO 
========================= */

void salvarArquivo() {
    FILE *f = fopen("dados.txt", "w");
    if (!f) return;

    fprintf(f, "PRODUTOS\n");
    for (int i = 0; i < totalProdutos; i++) {
        fprintf(f, "%d;%s;%.2f;%d;%s\n",
            produtos[i].id,
            produtos[i].nome,
            produtos[i].preco,
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

    char secao[20];

    while (fscanf(f, "%s", secao) == 1) {

        if (strcmp(secao, "PRODUTOS") == 0) {
            while (totalProdutos < MAX) {
                Produto p;
                if (fscanf(f, "%d;%49[^;];%f;%d;%10s\n",
                    &p.id,
                    p.nome,
                    &p.preco,
                    &p.estoque,
                    p.validade) != 5) break;

                produtos[totalProdutos++] = p;
            }
        }

        else if (strcmp(secao, "USUARIOS") == 0) {
            while (totalUsuarios < MAX) {
                Usuario u;
                if (fscanf(f, "%49[^;];%19[^;];%19[^\n]\n",
                    u.usuario,
                    u.senha,
                    u.cargo) != 3) break;

                usuarios[totalUsuarios++] = u;
            }
        }
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
    printf("  %s\n", t);//eu posso passar o que vai tar escrito ai ele coloca nesse printf fica mais organizado e tbm eu economizo umas linhas
    printf("========================================\n\n");
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
            printf(" Estoque baixo!\n");
    }

    system("pause");
}

void cadastrarProduto() {
    if (totalProdutos >= MAX) return;

    titulo("CADASTRAR PRODUTO");

    Produto p;

    int maiorId = 0;
    for (int i = 0; i < totalProdutos; i++)
        if (produtos[i].id > maiorId) maiorId = produtos[i].id;

    p.id = maiorId + 1;

    printf("Nome: ");
    scanf(" %49s", p.nome);

    printf("Preco: ");
    scanf("%f", &p.preco);

    printf("Estoque: ");
    scanf("%d", &p.estoque);

    if (p.estoque < 0) p.estoque = 0;

    printf("Validade (dd/mm/aaaa): ");
    scanf(" %10s", p.validade);

    produtos[totalProdutos++] = p;

    salvarArquivo();

    printf("\nProduto cadastrado!\n");
    system("pause");
}

/* =========================
   USUARIOS
========================= */

void cadastrarUsuario() {
    if (totalUsuarios >= MAX) return;

    titulo("CADASTRAR USUARIO");

    Usuario u;

    printf("Usuario: ");
    scanf(" %49s", u.usuario);

    printf("Senha: ");
    scanf(" %19s", u.senha);

    printf("Cargo (ADMIN/FUNC): ");
    scanf(" %19s", u.cargo);

    usuarios[totalUsuarios++] = u;

    salvarArquivo();

    printf("\nUsuario criado!\n");
    system("pause");
}

/* =========================
   VENDA ORIGINAL
========================= */

void venda() {
    titulo("VENDA");

    if (logado == -1) {
        printf("Faça login!\n");
        system("pause");
        return;
    }

    listarProdutos();

    int id, qtd;

    printf("\nID produto: ");
    scanf("%d", &id);

    printf("Quantidade: ");
    scanf("%d", &qtd);

    for (int i = 0; i < totalProdutos; i++) {

        if (produtos[i].id == id) {

            if (qtd > produtos[i].estoque) {
                printf("Estoque insuficiente!\n");
                system("pause");
                return;
            }

            produtos[i].estoque -= qtd;

            float total = qtd * produtos[i].preco;

            caixa[totalCaixa].valor = total;
            strcpy(caixa[totalCaixa].tipo, "VENDA");
            totalCaixa++;

            historico[totalHistorico++] = (HistoricoVenda){
                totalHistorico + 1,
                id,
                logado,
                qtd,
                total,
                "18/05/2026"
            };

            comissoes[logado].totalVendido += total;
            comissoes[logado].comissao =
                comissoes[logado].totalVendido * percentualComissao;

            salvarArquivo();

            printf("\nVenda realizada! Total: R$ %.2f\n", total);
            system("pause");
            return;
        }
    }

    printf("Produto nao encontrado!\n");
    system("pause");
}

/* =========================
   CAIXA
========================= */

void verCaixa() {
    titulo("CAIXA");

    float total = 0;

    for (int i = 0; i < totalCaixa; i++) {
        printf("%s - R$ %.2f\n", caixa[i].tipo, caixa[i].valor);
        total += caixa[i].valor;
    }

    printf("\nTOTAL: R$ %.2f\n", total);

    system("pause");
}

/* =========================
   NOVAS FUNCIONALIDADES
========================= */

void buscarProduto() {
    titulo("BUSCA");

    char nome[50];
    printf("Nome: ");
    scanf(" %49s", nome);

    for (int i = 0; i < totalProdutos; i++) {
        if (strstr(produtos[i].nome, nome)) {
            printf("%d - %s | R$ %.2f\n",
                produtos[i].id,
                produtos[i].nome,
                produtos[i].preco);
        }
    }

    system("pause");
}

void removerProduto() {
    titulo("REMOVER");

    int id;
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

    system("pause");
}

void relatorioVendas() {
    titulo("RELATORIO");

    float total = 0;

    for (int i = 0; i < totalHistorico; i++) {
        printf("P:%d F:%d Q:%d R$%.2f\n",
            historico[i].produtoId,
            historico[i].funcionarioId,
            historico[i].quantidade,
            historico[i].total);

        total += historico[i].total;
    }

    printf("\nTOTAL: R$ %.2f\n", total);
    system("pause");
}

void definirMeta() {
    titulo("META");

    float m;
    scanf("%f", &m);

    metas[logado].meta = m;

    system("pause");
}

void desempenho() {//aqui e pra ver o desempenho do func, ai da pra ver quanto vendeu, e outras coisas
    titulo("DESEMPENHO");

    printf("Vendido: %.2f\n", comissoes[logado].totalVendido);
    printf("Comissao: %.2f\n", comissoes[logado].comissao);
    printf("Meta: %.2f\n", metas[logado].meta);

    system("pause");
}

void vendaMultipla() {
    titulo("VENDA MULTIPLA");

    int id, qtd;
    float totalCompra = 0;

    while (1) {
        printf("ID (0 sai): ");
        scanf("%d", &id);
        if (id == 0) break;

        printf("Qtd: ");
        scanf("%d", &qtd);

        for (int i = 0; i < totalProdutos; i++) {
            if (produtos[i].id == id) {
                produtos[i].estoque -= qtd;
                totalCompra += qtd * produtos[i].preco;
            }
        }
    }

    caixa[totalCaixa++] = (Caixa){"VENDA", totalCompra};

    printf("Total: %.2f\n", totalCompra);
    system("pause");
}

/* =========================
   MENUS
========================= */

void menuAdmin() {
    int op;

    do {
        titulo("MENU ADMIN");

        printf("1 - Produtos\n");
        printf("2 - Usuarios\n");
        printf("3 - Venda\n");
        printf("4 - Caixa\n");
        printf("5 - Buscar\n");
        printf("6 - Remover\n");
        printf("7 - Relatorio\n");
        printf("8 - Meta\n");
        printf("9 - Desempenho\n");
        printf("10 - Venda Multipla\n");
        printf("0 - Sair\n");

        scanf("%d", &op);

        switch (op) {
            case 1: cadastrarProduto(); break;
            case 2: cadastrarUsuario(); break;
            case 3: venda(); break;
            case 4: verCaixa(); break;
            case 5: buscarProduto(); break;
            case 6: removerProduto(); break;
            case 7: relatorioVendas(); break;
            case 8: definirMeta(); break;
            case 9: desempenho(); break;
            case 10: vendaMultipla(); break;
        }

    } while (op != 0);
}

void menuFunc() {
    int op;

    do {
        titulo("MENU FUNC");

        printf("1 - Venda\n");
        printf("2 - Caixa\n");
        printf("0 - Sair\n");

        scanf("%d", &op);

        switch (op) {
            case 1: venda(); break;
            case 2: verCaixa(); break;
        }

    } while (op != 0);
}

/* =========================
   MAIN
========================= */

int main() {

    carregarArquivo();

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
                if (strcmp(usuarios[logado].cargo, "ADMIN") == 0)
                    menuAdmin();
                else
                    menuFunc();
            }
        }

    } while (op != 0);

    return 0;
}