
#pragma once

// Estrutura do No da Arvore 2-3
typedef struct Arv23
{
    int info1, info2;
    int nInfo; // 0 (underflow temporario), 1 ou 2
    struct Arv23 *esq, *cen, *dir;
} Arv23;

// Enum para status da remocao
typedef enum {
    OK,
    UNDERFLOW
} StatusRemocao;


int insere_23(Arv23 **raiz, int valor);

void imprime_23_em_ordem(Arv23 *raiz);

void imprime_arvore_visual(Arv23 *raiz, char *prefixo, int eh_ultimo, int eh_raiz);


int remover_23(Arv23 **raiz, int valor);

void libera_arvore(Arv23 **raiz);
