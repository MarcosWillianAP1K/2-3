
#ifndef ESCREVER_RESULTADO_H
#define ESCREVER_RESULTADO_H

#include <stdio.h>




void verificar_abertura_arquivos(FILE *arquivo);

void criar_resetar_arquivo_resultado(char *nome);

void escrever_resultado(char *nome, char *resultado);

#endif