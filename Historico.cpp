#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_Vendas 500
#define MAX_Caixas 500
#define MAX_Funcionarios 100

typedef struct {
    char produto[50];
    int quantidade;
    float valor;
    char funcionario[50];
} Venda;

typedef struct {
    char tipo[30];
    float valor;
    char funcionario[50];
} Caixa;

typedef struct {
    char nome[50];
    int totalVendas;
} Funcionario;


Venda vendas[MAX_Vendas];
Caixa caixas[MAX_Caixas];
Funcionario funcionarios[MAX_Funcionarios];

int qtdVendas = 0;
int qtdCaixa = 0;
int qtdFuncionarios = 0;


void registrarVenda() {
    printf("\n=== REGISTRAR VENDA ===\n");

    printf("Produto: ");
    scanf(" %[^\n]", vendas[qtdVendas].produto);

    printf("Quantidade: ");
    scanf("%d", &vendas[qtdVendas].quantidade);

    printf("Valor: ");
    scanf("%f", &vendas[qtdVendas].valor);

    printf("Funcionario: ");
    scanf(" %[^\n]", vendas[qtdVendas].funcionario);

    int encontrado = 0;

    for(int i = 0; i < qtdFuncionarios; i++) {
        if(strcmp(funcionarios[i].nome,
                  vendas[qtdVendas].funcionario) == 0) {

            funcionarios[i].totalVendas++;
            encontrado = 1;
            break;
        }
    }

    if(!encontrado) {
        strcpy(funcionarios[qtdFuncionarios].nome,
               vendas[qtdVendas].funcionario);

        funcionarios[qtdFuncionarios].totalVendas = 1;
        qtdFuncionarios++;
    }

    qtdVendas++;

    printf("\nVenda registrada com sucesso!\n");
}

void historicoVendas() {
    printf("\n=== HISTORICO DE VENDAS ===\n");

    if(qtdVendas == 0) {
        printf("Nenhuma venda registrada.\n");
        return;
    }

    for(int i = 0; i < qtdVendas; i++) {
        printf("\nVenda %d\n", i + 1);
        printf("Produto: %s\n", vendas[i].produto);
        printf("Quantidade: %d\n", vendas[i].quantidade);
        printf("Valor: R$ %.2f\n", vendas[i].valor);
        printf("Funcionario: %s\n", vendas[i].funcionario);
    }
}

void registrarCaixa() {
    printf("\n=== REGISTRAR MOVIMENTO DE CAIXA ===\n");

    printf("Tipo (Entrada/Saida): ");
    scanf(" %[^\n]", caixas[qtdCaixa].tipo);

    printf("Valor: ");
    scanf("%f", &caixas[qtdCaixa].valor);

    printf("Funcionario responsavel: ");
    scanf(" %[^\n]", caixas[qtdCaixa].funcionario);

    qtdCaixa++;

    printf("\nMovimento registrado!\n");
}

void historicoCaixa() {
    printf("\n=== HISTORICO DE CAIXA ===\n");

    if(qtdCaixa == 0) {
        printf("Nenhum movimento registrado.\n");
        return;
    }

    for(int i = 0; i < qtdCaixa; i++) {
        printf("\nMovimento %d\n", i + 1);
        printf("Tipo: %s\n", caixas[i].tipo);
        printf("Valor: R$ %.2f\n", caixas[i].valor);
        printf("Funcionario: %s\n", caixas[i].funcionario);
    }
}

void historicoFuncionarios() {
    printf("\n=== HISTORICO POR FUNCIONARIOS ===\n");

    if(qtdFuncionarios == 0) {
        printf("Nenhum funcionario registrado.\n");
        return;
    }

    for(int i = 0; i < qtdFuncionarios; i++) {
        printf("\nFuncionario: %s\n", funcionarios[i].nome);
        printf("Total de vendas: %d\n",
               funcionarios[i].totalVendas);
    }
}

int main() {

    int opcao;

    do {

        printf("\n===== SISTEMA =====\n");
        printf("1 - Registrar venda\n");
        printf("2 - Historico de vendas\n");
        printf("3 - Registrar caixa\n");
        printf("4 - Historico de caixa\n");
        printf("5 - Historico por funcionarios\n");
        printf("0 - Sair\n");

        printf("\nEscolha: ");
        scanf("%d", &opcao);

        switch(opcao) {

            case 1:
                registrarVenda();
                break;

            case 2:
                historicoVendas();
                break;

            case 3:
                registrarCaixa();
                break;

            case 4:
                historicoCaixa();
                break;

            case 5:
                historicoFuncionarios();
                break;

            case 0:
                printf("\nSaindo...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while(opcao != 0);

    return 0;
}
