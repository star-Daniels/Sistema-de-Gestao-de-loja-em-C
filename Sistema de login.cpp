
#include <stdio.h>
#include <string.h>

int main() {

    // =========================
    // DADOS DO GERENTE
    // =========================

    char gerenteCorreto[] = "Gustavo";
    char senhaGerenteCorreta[] = "123";

    // =========================
    // VARIÁVEIS
    // =========================

    char usuario[50];
    char senha[50];

    int opcao;
    int login = 0;

    FILE *arquivo;

    printf("=====================================\n");
    printf("         SISTEMA DE LOGIN            \n");
    printf("=====================================\n");

    // =========================
    // MENU PRINCIPAL
    // =========================

    printf("\n1 - Login do Gerente\n");
    printf("2 - Login do Funcionário\n");

    printf("\nEscolha uma opcao: ");
    scanf("%d", &opcao);

    // ==================================================
    // LOGIN DO GERENTE
    // ==================================================

    if(opcao == 1) {

        for(int tentativa = 1; tentativa <= 3; tentativa++) {

            printf("\n===== LOGIN DO GERENTE =====\n");
            printf("Tentativa %d de 3\n", tentativa);

            printf("Usuario: ");
            scanf("%s", usuario);

            printf("Senha: ");
            scanf("%s", senha);

            if(strcmp(usuario, gerenteCorreto) == 0 &&
               strcmp(senha, senhaGerenteCorreta) == 0) {

                login = 1;
                break;
            }
            else {

                printf("\n[ERRO] Usuario ou senha incorretos!\n");
            }
        }

        // =========================
        // MENU DO GERENTE
        // =========================

        if(login) {

            int menuGerente;
            char novoUsuario[50];
            char novaSenha[50];

            printf("\n=====================================\n");
            printf(" LOGIN DO GERENTE REALIZADO!\n");
            printf("=====================================\n");

            printf("\n1 - Entrar no aplicativo\n");
            printf("2 - Cadastrar novo usuario\n");

            printf("\nEscolha uma opcao: ");
            scanf("%d", &menuGerente);

            // =========================
            // ENTRAR
            // =========================

            if(menuGerente == 1) {

                printf("\nEntrando no aplicativo...\n");
            }

            // =========================
            // CADASTRAR USUÁRIO
            // =========================

            else if(menuGerente == 2) {

                printf("\n===== CADASTRO =====\n");

                printf("Novo usuario: ");
                scanf("%s", novoUsuario);

                printf("Nova senha: ");
                scanf("%s", novaSenha);

                // Abrir arquivo para salvar
                arquivo = fopen("usuarios.txt", "a");

                if(arquivo == NULL) {

                    printf("\nErro ao abrir banco de dados!\n");
                    return 0;
                }

                // Salvar usuário e senha
                fprintf(arquivo, "%s %s\n", novoUsuario, novaSenha);

                fclose(arquivo);

                printf("\nUsuario cadastrado e salvo no banco de dados!\n");
            }

            else {

                printf("\n[ERRO] Opcao invalida!\n");
            }

        } else {

            printf("\n=====================================\n");
            printf(" ACESSO BLOQUEADO!\n");
            printf(" Numero maximo de tentativas atingido.\n");
            printf("=====================================\n");
        }
    }

    // ==================================================
    // LOGIN DO FUNCIONÁRIO
    // ==================================================

    else if(opcao == 2) {

        char usuarioArquivo[50];
        char senhaArquivo[50];

        for(int tentativa = 1; tentativa <= 3; tentativa++) {

            printf("\n===== LOGIN DO FUNCIONÁRIO =====\n");
            printf("Tentativa %d de 3\n", tentativa);

            printf("Usuario: ");
            scanf("%s", usuario);

            printf("Senha: ");
            scanf("%s", senha);

            // Abrir banco de dados
            arquivo = fopen("usuarios.txt", "r");

            if(arquivo == NULL) {

                printf("\nNenhum usuario cadastrado!\n");
                return 0;
            }

            login = 0;

            // Ler arquivo
            while(fscanf(arquivo, "%s %s", usuarioArquivo, senhaArquivo) != EOF) {

                if(strcmp(usuario, usuarioArquivo) == 0 &&
                   strcmp(senha, senhaArquivo) == 0) {

                    login = 1;
                    break;
                }
            }

            fclose(arquivo);

            if(login) {
                break;
            }
            else {
                printf("\n[ERRO] Usuario ou senha incorretos!\n");
            }
        }

        // =========================
        // RESULTADO LOGIN
        // =========================

        if(login) {

            printf("\n=====================================\n");
            printf(" LOGIN REALIZADO COM SUCESSO!\n");
            printf(" Bem-vindo, %s.\n", usuario);
            printf("=====================================\n");

        } else {

            printf("\n=====================================\n");
            printf(" ACESSO BLOQUEADO!\n");
            printf(" Numero maximo de tentativas atingido.\n");
            printf("=====================================\n");
        }
    }

    // ==================================================
    // OPÇÃO INVÁLIDA
    // ==================================================

    else {

        printf("\n[ERRO] Opcao invalida!\n");
    }

    return 0;
}

