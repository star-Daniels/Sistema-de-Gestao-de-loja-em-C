#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_PRODUTOS 5
#define MAX_ITENS 20


typedef struct {
    int codigo;
    char nome[50];
    float preco;
    int estoque;
    char validade[20];
} Produto;


typedef struct {
    char nome[50];
    int quantidade;
    float precoUnitario;
    float subtotal;
} ItemVenda;


int produtoVencido(char validade[]) {

    int dia, mes, ano;

    sscanf(validade, "%d/%d/%d", &dia, &mes, &ano);

    time_t t = time(NULL);
    struct tm dataAtual = *localtime(&t);

    int diaAtual = dataAtual.tm_mday;
    int mesAtual = dataAtual.tm_mon + 1;
    int anoAtual = dataAtual.tm_year + 1900;

    if (ano < anoAtual) {
        return 1;
    }

    if (ano == anoAtual && mes < mesAtual) {
        return 1;
    }

    if (ano == anoAtual && mes == mesAtual && dia < diaAtual) {
        return 1;
    }

    return 0;
}


void mostrarProdutos(Produto produtos[]) {

    printf("\n========== PRODUTOS ==========\n");

    for (int i = 0; i < MAX_PRODUTOS; i++) {

        printf("Codigo: %d | Produto: %s | Preco: R$ %.2f | Estoque: %d | Validade: %s\n",
               produtos[i].codigo,
               produtos[i].nome,
               produtos[i].preco,
               produtos[i].estoque,
               produtos[i].validade);
    }
}


void mostrarCarrinho(
    ItemVenda itensVenda[],
    int totalItens,
    float valorTotal
) {

    printf("\n======= CARRINHO ATUAL =======\n");

    if (totalItens == 0) {

        printf("Nenhum produto adicionado.\n");
        return;
    }

    for (int i = 0; i < totalItens; i++) {

        printf("%d. %s | Qtd: %d | Subtotal: R$ %.2f\n",
               i + 1,
               itensVenda[i].nome,
               itensVenda[i].quantidade,
               itensVenda[i].subtotal);
    }

    printf("\nTOTAL ATUAL: R$ %.2f\n", valorTotal);
}


int procurarProduto(Produto produtos[], int codigo) {

    for (int i = 0; i < MAX_PRODUTOS; i++) {

        if (produtos[i].codigo == codigo) {
            return i;
        }
    }

    return -1;
}


void adicionarItemVenda(
    Produto produtos[],
    int indiceProduto,
    ItemVenda itensVenda[],
    int *totalItens,
    float *valorTotal
) {


    if (produtoVencido(produtos[indiceProduto].validade)) {

        printf("\nProduto vencido! Venda nao permitida.\n");
        return;
    }

    int quantidade;

    printf("Digite a quantidade: ");
    scanf("%d", &quantidade);


    if (quantidade > produtos[indiceProduto].estoque) {

        printf("\nEstoque insuficiente!\n");
        return;
    }

    
    float subtotal = quantidade * produtos[indiceProduto].preco;

    
    strcpy(itensVenda[*totalItens].nome, produtos[indiceProduto].nome);

    itensVenda[*totalItens].quantidade = quantidade;
    itensVenda[*totalItens].precoUnitario = produtos[indiceProduto].preco;
    itensVenda[*totalItens].subtotal = subtotal;

    
    produtos[indiceProduto].estoque -= quantidade;

    
    *valorTotal += subtotal;

    
    (*totalItens)++;

    printf("\nProduto adicionado no carrinho!\n");
}


void mostrarNotaFiscal(
    ItemVenda itensVenda[],
    int totalItens,
    float valorTotal
) {

    printf("\n=========== NOTA FISCAL ===========\n");

    for (int i = 0; i < totalItens; i++) {

        printf("\nProduto: %s\n", itensVenda[i].nome);
        printf("Quantidade: %d\n", itensVenda[i].quantidade);
        printf("Preco Unitario: R$ %.2f\n", itensVenda[i].precoUnitario);
        printf("Subtotal: R$ %.2f\n", itensVenda[i].subtotal);
    }

    printf("\n===================================\n");
    printf("VALOR TOTAL: R$ %.2f\n", valorTotal);
}


void mostrarEstoque(Produto produtos[]) {

    printf("\n======= ESTOQUE ATUALIZADO =======\n");

    for (int i = 0; i < MAX_PRODUTOS; i++) {

        printf("%s | Estoque: %d\n",
               produtos[i].nome,
               produtos[i].estoque);
    }
}


int main() {

    Produto produtos[MAX_PRODUTOS] = {
        {1, "Arroz", 25.50, 20, "10/12/2026"},
        {2, "Feijao", 10.00, 15, "05/11/2027"},
        {3, "Macarrao", 8.50, 30, "20/08/2026"},
        {4, "Leite", 6.75, 25, "15/06/2027"},
        {5, "Cafe", 18.90, 10, "01/09/2027"}
    };

    int novaVenda = 1;

    printf("===== SISTEMA DE VENDAS =====\n");

    
    while (novaVenda == 1) {

        ItemVenda itensVenda[MAX_ITENS];

        int totalItens = 0;
        float valorTotal = 0;

        int continuar = 1;

        printf("\n======= NOVA VENDA =======\n");

        
        while (continuar == 1) {

            int codigo;

            
            mostrarProdutos(produtos);
            
            mostrarCarrinho(
                itensVenda,
                totalItens,
                valorTotal
            );

            
            printf("\nDigite o codigo do produto: ");
            scanf("%d", &codigo);

            
            int indice = procurarProduto(produtos, codigo);

            if (indice == -1) {

                printf("\nProduto nao encontrado!\n");

            } else {

                adicionarItemVenda(
                    produtos,
                    indice,
                    itensVenda,
                    &totalItens,
                    &valorTotal
                );
            }

            
            mostrarCarrinho(
                itensVenda,
                totalItens,
                valorTotal
            );

            
            printf("\nDeseja adicionar mais produtos?\n");
            printf("1 - Sim\n");
            printf("0 - Nao\n");
            printf("Opcao: ");
            scanf("%d", &continuar);
        }

       
        mostrarNotaFiscal(
            itensVenda,
            totalItens,
            valorTotal
        );

        
        mostrarEstoque(produtos);

        
        printf("\nDeseja realizar uma nova venda?\n");
        printf("1 - Sim\n");
        printf("0 - Nao\n");
        printf("Opcao: ");
        scanf("%d", &novaVenda);
    }

    printf("\nSistema encerrado!\n");

    return 0;
}
