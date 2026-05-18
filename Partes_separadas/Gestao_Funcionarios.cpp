#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX 100

typedef struct {
    int id;
    char nome[50];
    char cargo[20];
    char senha[20];
    
} Funcionario;

Funcionario funcionarios[MAX];

int total = 0;

void cadastrarFuncionario() {
	int id,existe,i;
    if (total >= MAX) {
        printf("\nLimite de funcionários atingido!\n");
        return;
    }

    printf("\n===== CADASTRO DE FUNCIONÁRIO =====\n");

    do {

        existe = 0;

        printf("ID: ");
        scanf("%d", &id);
        getchar();

        for(i = 0; i < total; i++) {

            if(funcionarios[i].id == id) {
                printf("ID ja usado! Digite outro.\n");
                existe = 1;
                break;
            }
        }

    } while(existe);

    funcionarios[total].id = id;

    printf("Nome: ");
    fgets(funcionarios[total].nome, 50, stdin);
    funcionarios[total].nome[strcspn(funcionarios[total].nome, "\n")] = '\0';

    printf("Cargo (ADMIN/FUNCIONARIO): ");
    fgets(funcionarios[total].cargo, 20, stdin);
    funcionarios[total].cargo[strcspn(funcionarios[total].cargo, "\n")] = '\0';

    printf("Senha: ");
    fgets(funcionarios[total].senha, 20, stdin);
    funcionarios[total].senha[strcspn(funcionarios[total].senha, "\n")] = '\0';

    total++;

    printf("\nFuncionário cadastrado com sucesso!\n");
}

void listarFuncionarios() {

    if (total == 0) {
        printf("\nNenhum funcionário cadastrado.\n");
        return;
    }

    printf("\n===== LISTA DE FUNCIONÁRIOS =====\n");

    for (int i = 0; i < total; i++) {

        printf("\nFuncionário %d\n", i + 1);
        printf("ID: %d\n", funcionarios[i].id);
        printf("Nome: %s\n", funcionarios[i].nome);
        printf("Cargo: %s\n", funcionarios[i].cargo);
    }
}

int main() {

    setlocale(LC_ALL, "Portuguese");

    int opcao;

    do {

        printf("\n========== MENU ==========\n");
        printf("1 - Cadastrar funcionário\n");
        printf("2 - Listar funcionários\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch(opcao) {

            case 1:
                cadastrarFuncionario();
                break;

            case 2:
                listarFuncionarios();
                break;

            case 0:
                printf("\nEncerrando sistema...\n");
                break;

            default:
                printf("\nOpção inválida!\n");
        }

    } while(opcao != 0);

    return 0;
}
