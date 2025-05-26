#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Includes/Utilitarios/funcao_sistema.h"

void limpar_buffer()
{
#ifdef _WIN32
    fflush(stdin); // Para Windows
#else
    char c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // Para Linux e MacOS
#endif
}

void limpar_tela()
{
#ifdef _WIN32
    system("cls"); // Para Windows
#else
    system("clear"); // Para Linux e MacOS
#endif
}

void pausar_tela()
{
#ifdef _WIN32
    printf("\n");
    system("pause"); // Para Windows
#else
    printf("\nPressione qualquer tecla para continuar...");
    getchar(); // Para Linux e MacOS
#endif
}

void print_amarelo(char *mensagem)
{
    printf("\033[1;33m%s\033[0m", mensagem);
}

void print_vermelho(char *mensagem)
{
    printf("\033[1;31m%s\033[0m", mensagem);
}

void print_verde(char *mensagem)
{
    printf("\033[1;32m%s\033[0m", mensagem);
}

void mensagem_erro(char *mensagem)
{
    print_vermelho("\nERROR: ");
    print_vermelho(mensagem);
    printf("\n\n");
}

void mensagem_sucesso(char *mensagem)
{
    print_verde("\nSUCESSO: ");
    print_verde(mensagem);
    printf("\n\n");
}

void verificar_alocacao(void *ponteiro)
{
    if (ponteiro == NULL)
    {
        mensagem_erro("Erro ao alocar memoria");
        exit(1);
    }
}

void verificar_realocacao(void *ponteiro)
{
    if (ponteiro == NULL)
    {
        mensagem_erro("Erro ao realocar memoria");
        exit(1);
    }
}

char *digitar_string()
{
    limpar_buffer();
#define TAM_PADRAO 20

    int tam, cont = 0;
    char *nome = (char *)malloc(TAM_PADRAO * sizeof(char));

    if (fgets(nome, TAM_PADRAO * sizeof(char), stdin) != NULL)
    {

        tam = strlen(nome);

        while (nome[tam - 1] != '\n')
        {
            cont++;
            char *temp = (char *)realloc(nome, (TAM_PADRAO * cont) * sizeof(char));

            verificar_realocacao(temp);

            nome = temp;

            if (fgets(nome + tam, ((TAM_PADRAO * cont) * sizeof(char)) - tam, stdin) == NULL)
            {
                break;
            }

            tam = strlen(nome);
        }
    }

    tam = strlen(nome);

    if (nome[tam - 1] == '\n')
    {
        nome[tam - 1] = '\0';
    }

    limpar_buffer();

    return nome;
}

short int digitar_short_int()
{
    short int numero = 0;

    while (scanf("%hd", &numero) != 1 || numero < 0)
    {
        mensagem_erro("Numero invalido ");
        limpar_buffer();
        printf("Digite novamente: ");
    }

    return numero;
}

char digitar_um_caracter()
{
    char c;
    scanf("%c", &c);
    limpar_buffer();

    return c;
}

void corrigir_espacos(char **str)
{
    if (str != NULL && *str != NULL)
    {
        // Remover espaços à esquerda
        char *inicio = *str;
        while (*inicio == ' ')
            inicio++;

        // Remover espaços à direita
        char *fim = inicio + strlen(inicio) - 1;
        while (fim > inicio && *fim == ' ')
            fim--;

        *(fim + 1) = '\0';

        // Verificar se há necessidade de alocar nova string
        if (inicio != *str || *(fim + 1) != '\0')
        {
            char *corrigida = NULL;
            // Realocar a string corrigida
            corrigida = (char *)malloc((strlen(inicio) + 1) * sizeof(char));
            verificar_alocacao(corrigida);
            strcpy(corrigida, inicio);
            free(*str); // Liberar a string original, se necessário
            *str = corrigida;
        }
    }
}