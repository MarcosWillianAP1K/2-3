#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Arv23 {
    int info1, info2;
    int nInfo; // 1 ou 2
    struct Arv23 *esq, *cen, *dir;
} Arv23;

// ==================== FUNÇÕES AUXILIARES ====================

// Cria um novo nó
Arv23 *criaNo(int info, Arv23 *esq, Arv23 *cen) {
    Arv23 *novo = (Arv23*)malloc(sizeof(Arv23));
    novo->info1 = info;
    novo->nInfo = 1;
    novo->esq = esq;
    novo->cen = cen;
    novo->dir = NULL;
    return novo;
}

// Verifica se um nó é folha
bool ehFolha(Arv23 *no) {
    return (no != NULL && no->esq == NULL && no->cen == NULL && no->dir == NULL);
}

// Encontra o predecessor (maior valor à esquerda)
Arv23 *encontrarPredecessor(Arv23 *no) {
    Arv23 *atual = no->esq;
    while (atual->dir != NULL) {
        atual = atual->dir;
    }
    return atual;
}

// Encontra o sucessor (menor valor à direita)
Arv23 *encontrarSucessor(Arv23 *no) {
    Arv23 *atual = no->cen;
    while (atual->esq != NULL) {
        atual = atual->esq;
    }
    return atual;
}

// ==================== INSERÇÃO ====================

void adicionaInfo(Arv23 **no, int info, Arv23 *subarvore) {
    if (info > (*no)->info1) {
        (*no)->info2 = info;
        (*no)->dir = subarvore;
    } else {
        (*no)->info2 = (*no)->info1;
        (*no)->info1 = info;
        (*no)->dir = (*no)->cen;
        (*no)->cen = subarvore;
    }
    (*no)->nInfo = 2;
}

Arv23 *quebraNo(Arv23 **no, int info, int *sobe, Arv23 *subarvore) {
    Arv23 *maior;
    if (info > (*no)->info2) {
        *sobe = (*no)->info2;
        maior = criaNo(info, (*no)->dir, subarvore);
    } else if (info > (*no)->info1) {
        *sobe = info;
        maior = criaNo((*no)->info2, subarvore, (*no)->dir);
    } else {
        *sobe = (*no)->info1;
        maior = criaNo((*no)->info2, (*no)->cen, (*no)->dir);
        (*no)->info1 = info;
        (*no)->cen = subarvore;
    }
    (*no)->nInfo = 1;
    return maior;
}

int insereRecursivo(Arv23 **raiz, int valor, Arv23 *pai, int *sobe, Arv23 **maiorNo) {
    if (*raiz == NULL) {
        *raiz = criaNo(valor, NULL, NULL);
        return 1;
    }
    if (ehFolha(*raiz)) {
        if ((*raiz)->nInfo == 1) {
            adicionaInfo(raiz, valor, NULL);
            return 1;
        } else {
            *maiorNo = quebraNo(raiz, valor, sobe, NULL);
            return 1;
        }
    } else {
        if (valor < (*raiz)->info1) {
            insereRecursivo(&(*raiz)->esq, valor, *raiz, sobe, maiorNo);
        } else if ((*raiz)->nInfo == 1 || (valor < (*raiz)->info2)) {
            insereRecursivo(&(*raiz)->cen, valor, *raiz, sobe, maiorNo);
        } else {
            insereRecursivo(&(*raiz)->dir, valor, *raiz, sobe, maiorNo);
        }
    }
    if (*maiorNo != NULL) {
        if ((*raiz)->nInfo == 1) {
            adicionaInfo(raiz, *sobe, *maiorNo);
            *maiorNo = NULL;
            return 1;
        } else {
            *maiorNo = quebraNo(raiz, *sobe, sobe, *maiorNo);
            return 1;
        }
    }
    return 0;
}

int insere(Arv23 **raiz, int valor) {
    int sobe;
    Arv23 *maiorNo = NULL;
    return insereRecursivo(raiz, valor, NULL, &sobe, &maiorNo);
}

// ==================== REMOÇÃO ====================

// Caso 1: Nó folha com 1 valor (underflow)
int caso1Folha(Arv23 **raiz) {
    int valor = (*raiz)->info1;
    free(*raiz);
    *raiz = NULL;
    return valor;
}

// Caso 2: Nó interno com 1 valor (substitui e trata underflow)
int caso2Interno(Arv23 **raiz, Arv23 **pai) {
    if (!ehFolha((*raiz)->esq)) {
        Arv23 *pred = encontrarPredecessor(*raiz);
        if (pred->nInfo == 2) {
            (*raiz)->info1 = pred->info2;
            pred->nInfo = 1;
        } else {
            // Fusão necessária (implementação simplificada)
            (*raiz)->info1 = pred->info1;
            free(pred);
        }
    } else {
        Arv23 *suc = encontrarSucessor(*raiz);
        if (suc->nInfo == 2) {
            (*raiz)->info1 = suc->info1;
            suc->info1 = suc->info2;
            suc->nInfo = 1;
        } else {
            // Fusão necessária
            (*raiz)->info1 = suc->info1;
            free(suc);
        }
    }
    return (*raiz)->info1;
}

// Caso 3: Nó folha com 2 valores (remoção direta)
int caso3Folha(Arv23 **raiz, int valor) {
    if (valor == (*raiz)->info1) {
        (*raiz)->info1 = (*raiz)->info2;
    }
    (*raiz)->nInfo = 1;
    return valor;
}

// Caso 4: Nó interno com 2 valores (substitui e ajusta)
int caso4Interno(Arv23 **raiz, int valor) {
    if (valor == (*raiz)->info1) {
        Arv23 *pred = encontrarPredecessor(*raiz);
        (*raiz)->info1 = pred->nInfo == 2 ? pred->info2 : pred->info1;
        if (pred->nInfo == 2) pred->nInfo = 1;
        else free(pred);
    } else {
        Arv23 *suc = encontrarSucessor(*raiz);
        (*raiz)->info2 = suc->nInfo == 2 ? suc->info1 : suc->info1;
        if (suc->nInfo == 2) {
            suc->info1 = suc->info2;
            suc->nInfo = 1;
        } else free(suc);
    }
    return valor;
}

int removeRecursivo(Arv23 **raiz, int valor, Arv23 **pai) {
    if (*raiz == NULL) return 0;

    if (valor < (*raiz)->info1) {
        return removeRecursivo(&(*raiz)->esq, valor, raiz);
    } else if (((*raiz)->nInfo == 1) || (valor < (*raiz)->info2)) {
        return removeRecursivo(&(*raiz)->cen, valor, raiz);
    } else if ((*raiz)->nInfo == 2 && valor > (*raiz)->info2) {
        return removeRecursivo(&(*raiz)->dir, valor, raiz);
    } else {
        if ((*raiz)->nInfo == 1) {
            return ehFolha(*raiz) ? caso1Folha(raiz) : caso2Interno(raiz, pai);
        } else {
            return ehFolha(*raiz) ? caso3Folha(raiz, valor) : caso4Interno(raiz, valor);
        }
    }
}

int remove23(Arv23 **raiz, int valor) {
    return removeRecursivo(raiz, valor, NULL);
}

// ==================== BUSCA ====================

bool busca(Arv23 *raiz, int valor) {
    if (raiz == NULL) return false;
    if (valor == raiz->info1 || (raiz->nInfo == 2 && valor == raiz->info2)) {
        return true;
    } else if (valor < raiz->info1) {
        return busca(raiz->esq, valor);
    } else if (raiz->nInfo == 1 || valor < raiz->info2) {
        return busca(raiz->cen, valor);
    } else {
        return busca(raiz->dir, valor);
    }
}

// ==================== EXIBIÇÃO ====================

void emOrdem(Arv23 *raiz) {
    if (raiz != NULL) {
        emOrdem(raiz->esq);
        printf("%d ", raiz->info1);
        emOrdem(raiz->cen);
        if (raiz->nInfo == 2) {
            printf("%d ", raiz->info2);
            emOrdem(raiz->dir);
        }
    }
}

void exibir(Arv23 *raiz, int nivel) {
    if (raiz != NULL) {
        exibir(raiz->dir, nivel + 1);
        for (int i = 0; i < nivel; i++) printf("    ");
        printf("%d", raiz->info1);
        if (raiz->nInfo == 2) printf(", %d", raiz->info2);
        printf("\n");
        exibir(raiz->cen, nivel + 1);
        exibir(raiz->esq, nivel + 1);
    }
}

// ==================== MENU PRINCIPAL ====================

int main() {
    Arv23 *raiz = NULL;
    int opcao, valor;

    do {
        printf("\n=== MENU ARVORE 2-3 ===\n");
        printf("1. Inserir\n");
        printf("2. Remover\n");
        printf("3. Buscar\n");
        printf("4. Exibir em ordem\n");
        printf("5. Exibir estrutura\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Valor a inserir: ");
                scanf("%d", &valor);
                if (insere(&raiz, valor)) printf("Inserido com sucesso!\n");
                else printf("Erro na inserção.\n");
                break;
            case 2:
                printf("Valor a remover: ");
                scanf("%d", &valor);
                if (remove23(&raiz, valor)) printf("Removido com sucesso!\n");
                else printf("Valor não encontrado.\n");
                break;
            case 3:
                printf("Valor a buscar: ");
                scanf("%d", &valor);
                printf(busca(raiz, valor) ? "Encontrado!\n" : "Não encontrado.\n");
                break;
            case 4:
                printf("Árvore em ordem: ");
                emOrdem(raiz);
                printf("\n");
                break;
            case 5:
                printf("Estrutura da árvore:\n");
                exibir(raiz, 0);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 0);

    return 0;
}