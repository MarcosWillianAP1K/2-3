#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Arv23
{
    int info1, info2;
    int nInfo;
    struct Arv23 *esq, *cen, *dir;
} Arv23;

int remover_23_recursivo(Arv23 **raiz, int valor, int *sobe, Arv23 **pai);

Arv23 *cria_no(int info, Arv23 *F_esq, Arv23 *F_cen)
{
    Arv23 *no = (Arv23 *)malloc(sizeof(Arv23));

    if (no != NULL)
    {
        no->info1 = info;
        no->nInfo = 1;
        no->esq = F_esq;
        no->cen = F_cen;
        no->dir = NULL;
    }

    return no;
}

//================AUXILIARES REMOÇÃO==================

int eh_folha(Arv23 *no)
{
    int retorno = -1;

    if (no != NULL)
    {
        retorno = 0; // Não é uma folha

        if (no->esq == NULL && no->cen == NULL && no->dir == NULL)
        {
            retorno = 1; // É uma folha
        }
    }

    return retorno;
}

Arv23 *pegar_maior_da_esquerda(Arv23 *no)
{
    Arv23 *atual = no;

    while (atual != NULL && (atual->dir != NULL || atual->cen != NULL))
    {
        if (atual->dir != NULL)
            atual = atual->dir;
        else if (atual->cen != NULL)
            atual = atual->cen;
        else
            break;
    }

    return atual;
}

Arv23 *pegar_menor_da_direita(Arv23 *no)
{
    Arv23 *atual = no;

    while (atual != NULL && atual->esq != NULL)
    {
        atual = atual->esq;
    }

    return atual;
}

// Função para redistribuir valores entre nós irmãos
int redistribuir_valores(Arv23 **pai, Arv23 **no_vazio, int posicao_filho)
{
    int sucesso = 0;
    
    if (posicao_filho == 0) // Filho esquerdo está vazio
    {
        if ((*pai)->cen != NULL && (*pai)->cen->nInfo == 2) // Irmão do centro tem 2 valores
        {
            (*no_vazio)->info1 = (*pai)->info1;
            (*no_vazio)->nInfo = 1;
            (*pai)->info1 = (*pai)->cen->info1;
            (*pai)->cen->info1 = (*pai)->cen->info2;
            (*pai)->cen->nInfo = 1;
            sucesso = 1;
        }
    }
    else if (posicao_filho == 1) // Filho central está vazio
    {
        if ((*pai)->esq != NULL && (*pai)->esq->nInfo == 2) // Irmão da esquerda tem 2 valores
        {
            (*no_vazio)->info1 = (*pai)->info1;
            (*no_vazio)->nInfo = 1;
            (*pai)->info1 = (*pai)->esq->info2;
            (*pai)->esq->nInfo = 1;
            sucesso = 1;
        }
        else if ((*pai)->nInfo == 2 && (*pai)->dir != NULL && (*pai)->dir->nInfo == 2) // Irmão da direita tem 2 valores
        {
            (*no_vazio)->info1 = (*pai)->info2;
            (*no_vazio)->nInfo = 1;
            (*pai)->info2 = (*pai)->dir->info1;
            (*pai)->dir->info1 = (*pai)->dir->info2;
            (*pai)->dir->nInfo = 1;
            sucesso = 1;
        }
    }
    else if (posicao_filho == 2) // Filho direito está vazio
    {
        if ((*pai)->cen != NULL && (*pai)->cen->nInfo == 2) // Irmão do centro tem 2 valores
        {
            (*no_vazio)->info1 = (*pai)->info2;
            (*no_vazio)->nInfo = 1;
            (*pai)->info2 = (*pai)->cen->info2;
            (*pai)->cen->nInfo = 1;
            sucesso = 1;
        }
    }
    
    return sucesso;
}

// Função para fazer merge de nós
void fazer_merge(Arv23 **pai, int posicao_filho)
{
    if (posicao_filho == 0) // Merge filho esquerdo com central
    {
        if ((*pai)->esq != NULL && (*pai)->cen != NULL)
        {
            // Mescla info1 do pai e info1 do centro no filho esquerdo
            (*pai)->esq->info2 = (*pai)->cen->info1;
            (*pai)->esq->nInfo = 2;
            // Ajusta ponteiros dos filhos do nó mesclado
            if ((*pai)->esq->cen == NULL) {
                (*pai)->esq->cen = (*pai)->cen->esq;
                (*pai)->esq->dir = (*pai)->cen->cen;
            } else {
                (*pai)->esq->dir = (*pai)->cen->esq;
            }
            // Libera o nó central
            free((*pai)->cen);
            (*pai)->cen = (*pai)->dir;
            (*pai)->dir = NULL;
            if ((*pai)->nInfo == 2)
            {
                (*pai)->info1 = (*pai)->info2;
                (*pai)->nInfo = 1;
            }
            else
            {
                (*pai)->nInfo = 0; // Pai fica vazio
            }
        }
    }
    else if (posicao_filho == 1) // Merge filho central com esquerdo
    {
        if ((*pai)->esq != NULL && (*pai)->cen != NULL)
        {
            // Mescla info1 do pai e info1 do centro no filho esquerdo
            (*pai)->esq->info2 = (*pai)->cen->info1;
            (*pai)->esq->nInfo = 2;
            // Ajusta ponteiros dos filhos do nó mesclado
            if ((*pai)->esq->cen == NULL) {
                (*pai)->esq->cen = (*pai)->cen->esq;
                (*pai)->esq->dir = (*pai)->cen->cen;
            } else {
                (*pai)->esq->dir = (*pai)->cen->esq;
            }
            // Libera o nó central
            free((*pai)->cen);
            (*pai)->cen = (*pai)->dir;
            (*pai)->dir = NULL;
            if ((*pai)->nInfo == 2)
            {
                (*pai)->info1 = (*pai)->info2;
                (*pai)->nInfo = 1;
            }
            else
            {
                (*pai)->nInfo = 0;
            }
        }
    }
    else if (posicao_filho == 2) // Merge filho direito com central
    {
        if ((*pai)->cen != NULL && (*pai)->dir != NULL)
        {
            // Mescla info2 do pai e info1 do direito no filho central
            (*pai)->cen->info2 = (*pai)->dir->info1;
            (*pai)->cen->nInfo = 2;
            // Ajusta ponteiros dos filhos do nó mesclado
            if ((*pai)->cen->cen == NULL) {
                (*pai)->cen->cen = (*pai)->dir->esq;
                (*pai)->cen->dir = (*pai)->dir->cen;
            } else {
                (*pai)->cen->dir = (*pai)->dir->esq;
            }
            // Libera o nó direito
            free((*pai)->dir);
            (*pai)->dir = NULL;
            (*pai)->nInfo = 1;
        }
    }
}

//================CASOS DA REMOÇÃO==================

// Caso 1: Nó folha com 1 informação
int no_folha_com_1_info(Arv23 **raiz, int *sobe)
{
    int retorno = (*raiz)->info1;
    
    free(*raiz);
    *raiz = NULL;
    *sobe = 1; // Indica que o nó foi removido e precisa de ajuste
    
    return retorno;
}

// Caso 2: Nó interno com 1 informação
int no_interno_com_1_info(Arv23 **raiz)
{
    int retorno = (*raiz)->info1;
    Arv23 *sucessor = NULL;
    int sobe_aux = 0;
    
    // Encontra o sucessor (menor valor da subárvore central)
    sucessor = pegar_menor_da_direita((*raiz)->cen);
    
    if (sucessor != NULL)
    {
        // Substitui o valor a ser removido pelo sucessor
        (*raiz)->info1 = sucessor->info1;
        
        // Remove o sucessor recursivamente
        remover_23_recursivo(&((*raiz)->cen), sucessor->info1, &sobe_aux, raiz);
    }
    
    return retorno;
}

// Caso 3: Nó folha com 2 informações (já implementado corretamente)
int no_folha_com_2_infos(Arv23 **raiz, int valor)
{
    int retorno = 0;

    if (valor == (*raiz)->info1)
    {
        retorno = (*raiz)->info1;
        (*raiz)->info1 = (*raiz)->info2;
        (*raiz)->nInfo = 1;
    }
    else
    {
        retorno = (*raiz)->info2;
        (*raiz)->nInfo = 1;
    }
    
    return retorno;
}

// Caso 4: Nó interno com 2 informações
int no_interno_com_2_infos(Arv23 **raiz, int valor)
{
    int retorno = 0;
    Arv23 *sucessor = NULL;
    int sobe_aux = 0;
    
    if (valor == (*raiz)->info1) // Remover info1
    {
        retorno = (*raiz)->info1;
        
        // Pega o sucessor da subárvore central (menor valor)
        sucessor = pegar_menor_da_direita((*raiz)->cen);
        
        if (sucessor != NULL)
        {
            (*raiz)->info1 = sucessor->info1;
            
            // Remove o sucessor recursivamente
            remover_23_recursivo(&((*raiz)->cen), sucessor->info1, &sobe_aux, raiz);
        }
    }
    else // Remover info2
    {
        retorno = (*raiz)->info2;
        
        // Pega o sucessor da subárvore direita (menor valor)
        sucessor = pegar_menor_da_direita((*raiz)->dir);
        
        if (sucessor != NULL)
        {
            (*raiz)->info2 = sucessor->info1;
            
            // Remove o sucessor recursivamente
            remover_23_recursivo(&((*raiz)->dir), sucessor->info1, &sobe_aux, raiz);
        }
    }
    
    return retorno;
}

//================REMOÇÃO PRINCIPAL==================

int remover_23_recursivo(Arv23 **raiz, int valor, int *sobe, Arv23 **pai)
{
    int retorno = 0;

    if (raiz != NULL && *raiz != NULL)
    {
        // Verifica se o valor está no nó atual
        if (valor == (*raiz)->info1 || ((*raiz)->nInfo == 2 && valor == (*raiz)->info2))
        {
            // Valor encontrado no nó atual
            *sobe = 0;
            
            if ((*raiz)->nInfo == 1)
            {
                if (eh_folha(*raiz) == 1)
                {
                    retorno = no_folha_com_1_info(raiz, sobe);
                }
                else
                {
                    retorno = no_interno_com_1_info(raiz);
                }
            }
            else // nInfo == 2
            {
                if (eh_folha(*raiz) == 1)
                {
                    retorno = no_folha_com_2_infos(raiz, valor);
                }
                else
                {
                    retorno = no_interno_com_2_infos(raiz, valor);
                }
            }
        }
        else if (valor < (*raiz)->info1)
        {
            retorno = remover_23_recursivo(&(*raiz)->esq, valor, sobe, raiz);
            
            // Tratamento quando filho esquerdo fica vazio
            if (*sobe && (*raiz)->esq == NULL)
            {
                if (redistribuir_valores(raiz, &(*raiz)->esq, 0) == 0)
                {
                    fazer_merge(raiz, 0);
                    if ((*raiz)->nInfo == 0)
                    {
                        *sobe = 1;
                    }
                    else
                    {
                        *sobe = 0;
                    }
                }
                else
                {
                    *sobe = 0;
                }
            }
        }
        else if ((*raiz)->nInfo == 1 || ((*raiz)->nInfo == 2 && valor < (*raiz)->info2))
        {
            retorno = remover_23_recursivo(&(*raiz)->cen, valor, sobe, raiz);
            
            // Tratamento quando filho central fica vazio
            if (*sobe && (*raiz)->cen == NULL)
            {
                if (redistribuir_valores(raiz, &(*raiz)->cen, 1) == 0)
                {
                    fazer_merge(raiz, 1);
                    if ((*raiz)->nInfo == 0)
                    {
                        *sobe = 1;
                    }
                    else
                    {
                        *sobe = 0;
                    }
                }
                else
                {
                    *sobe = 0;
                }
            }
        }
        else if ((*raiz)->nInfo == 2 && valor > (*raiz)->info2)
        {
            retorno = remover_23_recursivo(&(*raiz)->dir, valor, sobe, raiz);
            
            // Tratamento quando filho direito fica vazio
            if (*sobe && (*raiz)->dir == NULL)
            {
                if (redistribuir_valores(raiz, &(*raiz)->dir, 2) == 0)
                {
                    fazer_merge(raiz, 2);
                }
                *sobe = 0;
            }
        }
    }

    return retorno;
}

int remover_23(Arv23 **raiz, int valor)
{
    int sobe = 0;
    int resultado = 0;
    
    resultado = remover_23_recursivo(raiz, valor, &sobe, NULL);

    // Ajuste para manter as regras da árvore 2-3 após remoção na raiz
    if (*raiz != NULL && (*raiz)->nInfo == 0) {
        Arv23 *temp = *raiz;
        // Caso especial: raiz vazia com dois filhos folhas (merge correto)
        if ((*raiz)->esq && (*raiz)->cen && (*raiz)->dir == NULL &&
            eh_folha((*raiz)->esq) && eh_folha((*raiz)->cen)) {
            Arv23 *novo = cria_no((*raiz)->esq->info1, NULL, NULL);
            novo->info2 = (*raiz)->cen->info1;
            novo->nInfo = 2;
            free((*raiz)->esq);
            free((*raiz)->cen);
            free(temp);
            *raiz = novo;
        }
        // Se só existe um filho, promove ele para raiz
        else if ((*raiz)->esq != NULL && (*raiz)->cen == NULL && (*raiz)->dir == NULL) {
            *raiz = (*raiz)->esq;
            free(temp);
        } else if ((*raiz)->cen != NULL && (*raiz)->esq == NULL && (*raiz)->dir == NULL) {
            *raiz = (*raiz)->cen;
            free(temp);
        } else if ((*raiz)->dir != NULL && (*raiz)->esq == NULL && (*raiz)->cen == NULL) {
            *raiz = (*raiz)->dir;
            free(temp);
        } else {
            *raiz = NULL;
            free(temp);
        }
    }

    return resultado;
}

//================INSERÇÃO (MANTIDA IGUAL)==================

void adiciona_infos(Arv23 **no, int info, Arv23 *Sub_Arv_Info)
{
    if (info > (*no)->info1)
    {
        (*no)->info2 = info;
        (*no)->dir = Sub_Arv_Info;
    }
    else
    {
        (*no)->info2 = (*no)->info1;
        (*no)->info1 = info;
        (*no)->dir = (*no)->cen;
        (*no)->cen = Sub_Arv_Info;
    }

    (*no)->nInfo = 2;
}

Arv23 *quebra_no(Arv23 **no, int info, int *sobe, Arv23 *F_dir)
{
    Arv23 *maior = NULL;

    if (info > (*no)->info2)
    {
        *sobe = (*no)->info2;
        maior = cria_no(info, (*no)->dir, F_dir);
        (*no)->nInfo = 1;
    }
    else if (info > (*no)->info1)
    {
        *sobe = info;
        maior = cria_no((*no)->info2, F_dir, (*no)->dir);
        (*no)->nInfo = 1;
    }
    else
    {
        *sobe = (*no)->info1;
        maior = cria_no((*no)->info2, (*no)->cen, (*no)->dir);
        (*no)->info1 = info;
        (*no)->cen = F_dir;
        (*no)->nInfo = 1;
    }

    return maior;
}

int insere_23_recursivo(Arv23 **raiz, int valor, Arv23 *pai, int *sobe, Arv23 **maiorNo)
{
    int retorno = 0;

    if (*raiz == NULL)
    {
        *raiz = cria_no(valor, NULL, NULL);
        retorno = 1;
    }
    else if (valor != (*raiz)->info1 && ((*raiz)->nInfo != 2 || valor != (*raiz)->info2))
    {
        if ((*raiz)->esq == NULL)
        {
            if ((*raiz)->nInfo == 1)
            {
                adiciona_infos(raiz, valor, NULL);
                retorno = 1;
            }
            else
            {
                *maiorNo = quebra_no(raiz, valor, sobe, NULL);

                if (pai == NULL)
                {
                    *raiz = cria_no(*sobe, *raiz, *maiorNo);
                    *maiorNo = NULL;
                }

                retorno = 1;
            }
        }
        else
        {
            if (valor < (*raiz)->info1)
            {
                retorno = insere_23_recursivo(&(*raiz)->esq, valor, *raiz, sobe, maiorNo);
            }
            else if (((*raiz)->nInfo == 1) || (valor < (*raiz)->info2))
            {
                retorno = insere_23_recursivo(&(*raiz)->cen, valor, *raiz, sobe, maiorNo);
            }
            else
            {
                retorno = insere_23_recursivo(&(*raiz)->dir, valor, *raiz, sobe, maiorNo);
            }

            if (*maiorNo != NULL)
            {
                if ((*raiz)->nInfo == 1)
                {
                    adiciona_infos(raiz, *sobe, *maiorNo);
                    *maiorNo = NULL;
                    retorno = 1;
                }
                else
                {
                    *maiorNo = quebra_no(raiz, *sobe, sobe, *maiorNo);

                    if (pai == NULL)
                    {
                        *raiz = cria_no(*sobe, *raiz, *maiorNo);
                        *maiorNo = NULL;
                    }
                    retorno = 1;
                }
            }
        }
    }
    else
    {
        printf("Valor %d ja existe na arvore!\n", valor);
    }

    return retorno;
}

int insere_23(Arv23 **raiz, int valor)
{
    int sobe = 0;
    Arv23 *MaiorNo = NULL;
    
    return insere_23_recursivo(raiz, valor, NULL, &sobe, &MaiorNo);
}

//===============IMPRIMIR RESULTADOS==================

void imprime_23_em_ordem(Arv23 *raiz)
{
    if (raiz != NULL)
    {
        imprime_23_em_ordem(raiz->esq);
        printf("%d ", raiz->info1);
        imprime_23_em_ordem(raiz->cen);
        if (raiz->nInfo == 2)
        {
            printf("%d ", raiz->info2);
            imprime_23_em_ordem(raiz->dir);
        }
    }
}

void exibir(Arv23 *raiz, int espacos)
{
    if (raiz)
    {
        int i;
        for (i = 1; i <= espacos; i++)
            printf(" ");

        printf("%d ", raiz->info1);
        if (raiz->nInfo == 2)
        {
            printf("%d ", raiz->info2);
        }

        printf("\n");
        exibir(raiz->esq, espacos + 10);
        exibir(raiz->cen, espacos + 10);
        if (raiz->nInfo == 2)
        {
            exibir(raiz->dir, espacos + 10);
        }
    }
}

//===============MAIN==================

void pausar() {
    printf("\nPressione Enter para continuar...");
    while(getchar() != '\n'); // Limpa o buffer
    getchar(); // Espera Enter
}

int main()
{
    Arv23 *raiz = NULL;
    int opcao, valor;

    printf("=== PROGRAMA ARVORE 2-3 INICIADO ===\n");
    
    do
    {
        printf("\n=== MENU ARVORE 2-3 ===\n");
        printf("1. Adicionar valor\n");
        printf("2. Remover valor\n");
        printf("3. Exibir arvore\n");
        printf("4. Sair\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida! Digite um numero.\n");
            while(getchar() != '\n'); // Limpa buffer
            continue;
        }

        switch (opcao)
        {
        case 1:
            printf("Digite o valor a ser inserido: ");
            if (scanf("%d", &valor) == 1) {
                if (insere_23(&raiz, valor) != 0)
                {
                    printf("Valor %d inserido com sucesso.\n", valor);
                }
                else
                {
                    printf("Erro ao inserir valor %d.\n", valor);
                }
            } else {
                printf("Valor invalido!\n");
                while(getchar() != '\n'); // Limpa buffer
            }
            pausar();
            break;

        case 2:
            printf("Digite o valor a ser removido: ");
            if (scanf("%d", &valor) == 1) {
                if (remover_23(&raiz, valor) != 0)
                {
                    printf("Valor %d removido com sucesso.\n", valor);
                }
                else
                {
                    printf("Erro ao remover valor %d ou valor nao encontrado.\n", valor);
                }
            } else {
                printf("Valor invalido!\n");
                while(getchar() != '\n'); // Limpa buffer
            }
            pausar();
            break;

        case 3:
            if (raiz == NULL) {
                printf("\nArvore vazia!\n");
            } else {
                printf("\nArvore em ordem: ");
                imprime_23_em_ordem(raiz);
                printf("\n\nEstrutura da arvore:\n");
                exibir(raiz, 0);
            }
            pausar();
            break;

        case 4:
            printf("Saindo do programa...\n");
            break;

        default:
            printf("Opcao invalida! Escolha uma opcao de 1 a 4.\n");
            pausar();
            break;
        }
    } while (opcao != 4);

    printf("Programa finalizado.\n");
    return 0;
}