#include <stdlib.h>
#include <string.h>
#include "Escrever_resultado.h"

void verificar_abertura_arquivos(FILE *arquivo)
{
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }
}


void criar_resetar_arquivo_resultado(char *nome)
{
    FILE *arquivo = fopen(nome, "w+");
    verificar_abertura_arquivos(arquivo);

    fprintf(arquivo, "Resultados dos Testes:\n\n");
    fclose(arquivo);
}

void escrever_resultado(char *nome, char *resultado)
{
    FILE *arquivo = fopen(nome, "a+");
    verificar_abertura_arquivos(arquivo);

    fprintf(arquivo, "%s", resultado);
    fclose(arquivo);
}