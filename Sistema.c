#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

/* =========================
   ESTRUTURAS
========================= */

typedef struct {
    int id;
    char nome[50];
    float preco;
    int estoque;
} Produto;

typedef struct {
    char usuario[50];
    char senha[20];
    char cargo[20];
} Usuario;

/* =========================
   MEMÓRIA
========================= */

Produto produtos[MAX];
Usuario usuarios[MAX];

int totalProdutos = 0;
int totalUsuarios = 0;

int logado = -1;

/* =========================
   SALVAR / CARREGAR
========================= */

void salvarArquivo() {
    FILE *f = fopen("dados.txt", "w");
    if (!f) return;

    fprintf(f, "PRODUTOS\n");
    for (int i = 0; i < totalProdutos; i++) {
        fprintf(f, "%d;%s;%.2f;%d\n",
            produtos[i].id,
            produtos[i].nome,
            produtos[i].preco,
            produtos[i].estoque);
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

                if (fscanf(f, "%d;%49[^;];%f;%d\n",
                    &p.id,
                    p.nome,
                    &p.preco,
                    &p.estoque) != 4) break;

                produtos[totalProdutos++] = p;
            }
        }

        else if (strcmp(secao, "USUARIOS") == 0) {

            while (totalUsuarios < MAX) {
                Usuario u;

                if (fscanf(f, " %49[^;];%19[^;];%19[^\n]\n",
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
   UI
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
        printf("%d - %s | R$ %.2f | estoque: %d\n",
            produtos[i].id,
            produtos[i].nome,
            produtos[i].preco,
            produtos[i].estoque);

        if (produtos[i].estoque < 5)
            printf("ALERTA: estoque baixo!\n");
    }

    system("pause");
}

void cadastrarProduto() {
    if (totalProdutos >= MAX) return;

    titulo("CADASTRAR PRODUTO");

    Produto p;
    p.id = totalProdutos + 1;

    printf("Nome: ");
    scanf(" %49s", p.nome);

    printf("Preco: ");
    scanf("%f", &p.preco);

    printf("Estoque: ");
    scanf("%d", &p.estoque);

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
   VENDA
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

            printf("\nTotal: R$ %.2f\n",
                qtd * produtos[i].preco);

            salvarArquivo();
            system("pause");
            return;
        }
    }

    printf("Produto nao encontrado!\n");
    system("pause");
}

/* =========================
   MENUS
========================= */

void menuAdmin() {
    int op;

    do {
        titulo("MENU ADMIN");

        printf("1 - Cadastrar produto\n");
        printf("2 - Cadastrar usuario\n");
        printf("3 - Listar produtos\n");
        printf("4 - Venda\n");
        printf("0 - Sair\n");

        scanf("%d", &op);

        switch (op) {
            case 1: cadastrarProduto(); break;
            case 2: cadastrarUsuario(); break;
            case 3: listarProdutos(); break;
            case 4: venda(); break;
        }

    } while (op != 0);
}

void menuFunc() {
    int op;

    do {
        titulo("MENU FUNCIONARIO");

        printf("1 - Venda\n");
        printf("2 - Listar produtos\n");
        printf("0 - Sair\n");

        scanf("%d", &op);

        switch (op) {
            case 1: venda(); break;
            case 2: listarProdutos(); break;
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