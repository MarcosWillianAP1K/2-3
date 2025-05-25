#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Arv23
{
    int info1, info2;
    int nInfo;
    struct Arv23 *esq, *cen, *dir;
} Arv23;

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

//================BUSCA==================

//================REMOÇÃO==================


int eh_folha(Arv23 *no)
{
   // -1 é null
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

Arv23 *pegar_maior_da_direita(Arv23 *no)
{
    Arv23 *atual = no;

    while (atual->dir != NULL)
    {
        atual = atual->dir;
    }

    return atual;
}

int remover_23_recursivo(Arv23 **raiz, int valor, int *sobe, Arv23 **pai)
{
    //O retorno é apenas para simular como seria pra voltar a informação do no removido
    int retorno = 0;

    if (raiz != NULL && *raiz != NULL)
    {
        if (valor < (*raiz)->info1)
        {
            retorno = remover_23_recursivo(&(*raiz)->esq, valor, sobe, raiz);
        }
        else if ((*raiz)->nInfo == 1 || valor < (*raiz)->info2)
        {
            retorno = remover_23_recursivo(&(*raiz)->cen, valor, sobe, raiz);
        }
        else if ((*raiz)->nInfo == 2 && valor > (*raiz)->info2)
        {
            retorno = remover_23_recursivo(&(*raiz)->dir, valor, sobe, raiz);
        }
        else
        {
            //Verifica quanta infos tem no no
            if ((*raiz)->nInfo == 1)
            {
                // Caso 1: nó folha com apenas um valor
                if (eh_folha(*raiz))
                {   
                    retorno = (*raiz)->info1;

                    

                    free(*raiz); // Libera o nó
                    *raiz = NULL; // Define o nó como NULL
                }
                else
                {
                    // Caso 2: nó interno com apenas um valor
                    retorno = (*raiz)->info1; 

                    *sobe = (*raiz)->info1;
                    free(*raiz);
                    *raiz = NULL; // A lógica de remoção deve ser tratada pelo pai
                }
            }
            else //Else para 2 infos
            {
                // Caso 3: nó com dois valores
                if (eh_folha(*raiz))
                {
                    // Remover o valor e ajustar o nó
                    if (valor == (*raiz)->info1)
                    {
                        retorno = (*raiz)->info1; // Retorna o valor removido

                        (*raiz)->info1 = (*raiz)->info2;
                        (*raiz)->nInfo = 1;
                    }
                    else
                    {
                        retorno = (*raiz)->info2; // Retorna o valor removido

                        (*raiz)->nInfo = 1; // Remove o segundo valor
                    }
                }
                else
                {
                    // Lógica para remover de um nó interno com dois valores
                    if (valor == (*raiz)->info1)
                    {
                        *sobe = (*raiz)->info2;
                        (*raiz)->nInfo = 1; // Ajusta o nó para ter apenas um valor
                        retorno = (*raiz)->cen; // Retorna a subárvore central para subir
                    }
                    else if (valor == (*raiz)->info2)
                    {
                        *sobe = (*raiz)->info2; // Valor a subir é o segundo valor do nó
                        (*raiz)->nInfo = 1; // Ajusta o nó para ter apenas um valor
                        retorno = (*raiz)->dir; // Retorna a subárvore direita para subir
                    }
                }
            }
            
        }
    }

    return retorno;
}

Arv23 *remover_23(Arv23 **raiz, int valor)
{
    int sobe = 0;
    return remover_23_recursivo(raiz, valor, &sobe, NULL);
}

//================INSERÇÃO==================

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
    Arv23 *maior;

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

Arv23 *insere_23_recursivo(Arv23 **raiz, int valor, Arv23 *pai, int *sobe)
{
    Arv23 *maiorNo = NULL;

    if (*raiz == NULL)
    {
        *raiz = cria_no(valor, NULL, NULL);
    }
    else if (valor != (*raiz)->info1 && ((*raiz)->nInfo != 2 || valor != (*raiz)->info2))
    {
        // Valor é diferente dos existentes no nó - processa inserção

        if ((*raiz)->esq == NULL)
        { // cheguei na folha

            if ((*raiz)->nInfo == 1)
            { // nó folha com apenas um valor posso adicionar o novo valor
                adiciona_infos(raiz, valor, NULL);
            }
            else
            {
                maiorNo = quebra_no(raiz, valor, sobe, NULL);

                if (pai == NULL)
                {
                    *raiz = cria_no(*sobe, *raiz, maiorNo);
                    maiorNo = NULL;
                }
            }
        }
        else
        { // nó interno
            if (valor < (*raiz)->info1)
            {
                maiorNo = insere_23_recursivo(&(*raiz)->esq, valor, *raiz, sobe);
            }
            else if (((*raiz)->nInfo == 1) || (valor < (*raiz)->info2))
            {
                maiorNo = insere_23_recursivo(&(*raiz)->cen, valor, *raiz, sobe);
            }
            else
            {
                maiorNo = insere_23_recursivo(&(*raiz)->dir, valor, *raiz, sobe);
            }

            if (maiorNo != NULL)
            {
                if ((*raiz)->nInfo == 1)
                {
                    adiciona_infos(raiz, *sobe, maiorNo);
                    maiorNo = NULL;
                }
                else
                {
                    maiorNo = quebra_no(raiz, *sobe, sobe, maiorNo);

                    if (pai == NULL)
                    {
                        *raiz = cria_no(*sobe, *raiz, maiorNo);
                        maiorNo = NULL;
                    }
                }
            }
        }
    }
    else
    {
        // Valor duplicado - caso padrão do else
        printf("Valor %d ja existe na arvore!\n", valor); // no geral vai retornar NULL e verifica isso fora da função
    }

    return maiorNo;
}

Arv23 *insere_23(Arv23 **raiz, int valor)
{
    int sobe = 0;
    return insere_23_recursivo(raiz, valor, NULL, &sobe);
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

// Função para imprimir a estrutura da árvore de forma mais visual
void imprime_estrutura(Arv23 *raiz, int nivel)
{
    if (raiz != NULL)
    {
        // Imprime o nó direito primeiro (se existir)
        if (raiz->dir != NULL)
        {
            imprime_estrutura(raiz->dir, nivel + 1);
        }

        // Imprime o segundo valor (se existir)
        if (raiz->nInfo == 2)
        {
            for (int i = 0; i < nivel; i++)
                printf("    ");
            printf("+-- %d\n", raiz->info2);
        }

        // Imprime o nó central
        if (raiz->cen != NULL)
        {
            imprime_estrutura(raiz->cen, nivel + 1);
        }

        // Imprime o primeiro valor
        for (int i = 0; i < nivel; i++)
            printf("    ");
        if (raiz->nInfo == 1)
        {
            printf("--- [%d]\n", raiz->info1);
        }
        else
        {
            printf("+-- %d\n", raiz->info1);
        }

        // Imprime o nó esquerdo
        if (raiz->esq != NULL)
        {
            imprime_estrutura(raiz->esq, nivel + 1);
        }
    }
}

// Função alternativa: imprime como uma árvore "deitada" mais clara
void imprime_arvore_visual(Arv23 *raiz, char *prefixo, int eh_ultimo)
{
    if (raiz != NULL)
    {
        printf("%s", prefixo);
        printf("%s", eh_ultimo ? "+-- " : "+-- ");

        if (raiz->nInfo == 1)
        {
            printf("[%d]\n", raiz->info1);
        }
        else
        {
            printf("[%d, %d]\n", raiz->info1, raiz->info2);
        }

        if (raiz->esq != NULL || raiz->cen != NULL || raiz->dir != NULL)
        {
            char novo_prefixo[256];
            sprintf(novo_prefixo, "%s%s", prefixo, eh_ultimo ? "    " : "|   ");

            int filhos_existentes = 0;
            if (raiz->esq != NULL)
                filhos_existentes++;
            if (raiz->cen != NULL)
                filhos_existentes++;
            if (raiz->dir != NULL)
                filhos_existentes++;

            int contador = 0;

            if (raiz->esq != NULL)
            {
                contador++;
                imprime_arvore_visual(raiz->esq, novo_prefixo, contador == filhos_existentes);
            }

            if (raiz->cen != NULL)
            {
                contador++;
                imprime_arvore_visual(raiz->cen, novo_prefixo, contador == filhos_existentes);
            }

            if (raiz->dir != NULL)
            {
                contador++;
                imprime_arvore_visual(raiz->dir, novo_prefixo, contador == filhos_existentes);
            }
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

int main()
{
    Arv23 *raiz = NULL;
    int valor;

    printf("Digite os valores a serem inseridos na Arvore 2-3 (digite -1 para sair):\n");
    while (1)
    {
        scanf("%d", &valor);
        if (valor == -1)
        {
            break;
        }
        insere_23(&raiz, valor);
        printf("Arvore apos inserir %d: ", valor);
        imprime_23_em_ordem(raiz);
        printf("\n");
    }

    printf("\n=== RESULTADO FINAL ===\n");
    printf("Valores em ordem: ");
    imprime_23_em_ordem(raiz);
    printf("\n\n");

    printf("Estrutura da arvore (visual):\n");
    if (raiz == NULL)
    {
        printf("Árvore vazia\n");
    }
    else
    {
        exibir(raiz, 0);
        printf("\n");
    }

    printf("\nEstrutura da arvore (alternativa):\n");
    if (raiz == NULL)
    {
        printf("Arvore vazia\n");
    }
    else
    {
        exibir(raiz, 0);
    }

    return 0;
}