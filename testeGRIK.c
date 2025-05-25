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

//================AUXILIARES REMOÇÃO==================

int eh_folha(Arv23 *no)
{
    int retorno = -1;

    if (no != NULL)
    {
        retorno = 0;
        if (no->esq == NULL && no->cen == NULL && no->dir == NULL)
        {
            retorno = 1;
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

Arv23 *pegar_menor_da_esquerda(Arv23 *no)
{
    Arv23 *atual = no;

    while (atual->esq != NULL)
    {
        atual = atual->esq;
    }

    return atual;
}

//================CASOS DA REMOÇÃO==================

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

void corrige_underflow(Arv23 **raiz, Arv23 **pai, int *sobe, int posicao)
{
    if (*pai == NULL)
    {
        // Caso raiz: se vazia, promove o filho (se houver)
        if (*raiz == NULL && (*pai)->nInfo == 1)
        {
            Arv23 *temp = *pai;
            *pai = (*pai)->esq ? (*pai)->esq : (*pai)->cen;
            free(temp);
        }
    }
    else
    {
        Arv23 *irmao = NULL;
        int valor_emprestado = 0;

        if (posicao == 0)
        { // *raiz é filho esquerdo
            irmao = (*pai)->cen;
            if (irmao && irmao->nInfo == 2)
            { // Empresta do irmão central
                valor_emprestado = (*pai)->info1;
                (*pai)->info1 = irmao->info1;
                irmao->info1 = irmao->info2;
                irmao->nInfo = 1;
                irmao->dir = irmao->cen;
                irmao->cen = irmao->esq;
                *raiz = cria_no(valor_emprestado, NULL, NULL);
            }
            else
            { // Mescla com o irmão central
                irmao->info2 = (*pai)->info1;
                irmao->nInfo = 2;
                irmao->dir = irmao->cen;
                irmao->cen = irmao->esq;
                irmao->esq = NULL;
                (*pai)->esq = NULL;
                if ((*pai)->nInfo == 2)
                {
                    (*pai)->info1 = (*pai)->info2;
                    (*pai)->nInfo = 1;
                    (*pai)->cen = (*pai)->dir;
                    (*pai)->dir = NULL;
                }
                else
                {
                    Arv23 *temp = *pai;
                    *pai = irmao;
                    free(temp);
                }
            }
        }
        else if (posicao == 1)
        { // *raiz é filho central
            irmao = (*pai)->esq;
            if (irmao && irmao->nInfo == 2)
            { // Empresta do irmão esquerdo
                valor_emprestado = (*pai)->info1;
                (*pai)->info1 = irmao->info2;
                irmao->nInfo = 1;
                *raiz = cria_no(valor_emprestado, NULL, NULL);
            }
            else
            {
                irmao = (*pai)->dir;
                if (irmao && irmao->nInfo == 2)
                { // Empresta do irmão direito
                    valor_emprestado = (*pai)->info2;
                    (*pai)->info2 = irmao->info1;
                    irmao->info1 = irmao->info2;
                    irmao->nInfo = 1;
                    irmao->dir = irmao->cen;
                    irmao->cen = irmao->esq;
                    *raiz = cria_no(valor_emprestado, NULL, NULL);
                }
                else
                { // Mescla com o irmão esquerdo
                    irmao = (*pai)->esq;
                    irmao->info2 = (*pai)->info1;
                    irmao->nInfo = 2;
                    irmao->dir = *raiz;
                    irmao->cen = NULL;
                    (*pai)->esq = NULL;
                    if ((*pai)->nInfo == 2)
                    {
                        (*pai)->info1 = (*pai)->info2;
                        (*pai)->nInfo = 1;
                        (*pai)->cen = (*pai)->dir;
                        (*pai)->dir = NULL;
                    }
                    else
                    {
                        Arv23 *temp = *pai;
                        *pai = irmao;
                        free(temp);
                    }
                }
            }
        }
        else if (posicao == 2)
        { // *raiz é filho direito
            irmao = (*pai)->cen;
            if (irmao && irmao->nInfo == 2)
            { // Empresta do irmão central
                valor_emprestado = (*pai)->info2;
                (*pai)->info2 = irmao->info2;
                irmao->nInfo = 1;
                *raiz = cria_no(valor_emprestado, NULL, NULL);
            }
            else
            { // Mescla com o irmão central
                irmao->info2 = (*pai)->info2;
                irmao->nInfo = 2;
                irmao->dir = *raiz;
                irmao->esq = irmao->cen;
                irmao->cen = NULL;
                (*pai)->dir = NULL;
                (*pai)->nInfo = 1;
            }
        }
    }
}

int no_interno_com_2_infos(Arv23 **raiz, int valor, int *sobe, Arv23 **pai)
{
    int retorno = 0;
    Arv23 *sucessor = NULL;

    if (valor == (*raiz)->info1)
    {
        retorno = (*raiz)->info1;
        sucessor = pegar_maior_da_direita((*raiz)->cen);
        (*raiz)->info1 = sucessor->info1;
        remover_23_recursivo(&(*raiz)->cen, sucessor->info1, sobe, raiz);
        if (*raiz && (*raiz)->cen == NULL && !eh_folha(*raiz))
        {
            corrige_underflow(&(*raiz)->cen, raiz, sobe, 1);
        }
    }
    else
    {
        retorno = (*raiz)->info2;
        sucessor = pegar_menor_da_esquerda((*raiz)->dir);
        (*raiz)->info2 = sucessor->info1;
        remover_23_recursivo(&(*raiz)->dir, sucessor->info1, sobe, raiz);
        if (*raiz && (*raiz)->dir == NULL && !eh_folha(*raiz))
        {
            corrige_underflow(&(*raiz)->dir, raiz, sobe, 2);
        }
    }

    return retorno;
}

int remover_23_recursivo(Arv23 **raiz, int valor, int *sobe, Arv23 **pai)
{
    int retorno = 0;

    if (*raiz != NULL)
    {
        if (valor < (*raiz)->info1)
        {
            retorno = remover_23_recursivo(&(*raiz)->esq, valor, sobe, raiz);
            if (*raiz && (*raiz)->esq == NULL && (*raiz)->nInfo == 1 && !eh_folha(*raiz))
            {
                corrige_underflow(&(*raiz)->esq, raiz, sobe, 0);
            }
        }
        else if ((*raiz)->nInfo == 1 || (valor < (*raiz)->info2 && valor > (*raiz)->info1))
        {
            retorno = remover_23_recursivo(&(*raiz)->cen, valor, sobe, raiz);
            if (*raiz && (*raiz)->cen == NULL && (*raiz)->nInfo == 1 && !eh_folha(*raiz))
            {
                corrige_underflow(&(*raiz)->cen, raiz, sobe, 1);
            }
        }
        else if ((*raiz)->nInfo == 2 && valor > (*raiz)->info2)
        {
            retorno = remover_23_recursivo(&(*raiz)->dir, valor, sobe, raiz);
            if (*raiz && (*raiz)->dir == NULL && (*raiz)->nInfo == 2 && !eh_folha(*raiz))
            {
                corrige_underflow(&(*raiz)->dir, raiz, sobe, 2);
            }
        }
        else
        {
            if (eh_folha(*raiz))
            {
                if ((*raiz)->nInfo == 2)
                {
                    retorno = no_folha_com_2_infos(raiz, valor);
                }
                else
                {
                    retorno = (*raiz)->info1;
                    free(*raiz);
                    *raiz = NULL;
                    if (pai && *pai)
                    {
                        corrige_underflow(raiz, pai, sobe, 
                            (*pai)->esq == *raiz ? 0 : 
                            (*pai)->cen == *raiz ? 1 : 2);
                    }
                }
            }
            else
            {
                if ((*raiz)->nInfo == 2)
                {
                    retorno = no_interno_com_2_infos(raiz, valor, sobe, pai);
                }
                else
                {
                    retorno = (*raiz)->info1;
                    Arv23 *sucessor = pegar_maior_da_direita((*raiz)->cen);
                    (*raiz)->info1 = sucessor->info1;
                    retorno = remover_23_recursivo(&(*raiz)->cen, sucessor->info1, sobe, raiz);
                    if (*raiz && (*raiz)->cen == NULL && !eh_folha(*raiz))
                    {
                        corrige_underflow(&(*raiz)->cen, raiz, sobe, 1);
                    }
                }
            }
        }
    }

    return retorno;
}

int remover_23(Arv23 **raiz, int valor)
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
    if (raiz == NULL)
    {
        printf("Arvore vazia!\n");
    }
    else
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

void imprime_estrutura(Arv23 *raiz, int nivel)
{
    if (raiz == NULL)
    {
        for (int i = 0; i < nivel; i++)
            printf("    ");
        printf("--- [vazio]\n");
    }
    else
    {
        if (raiz->dir != NULL)
        {
            imprime_estrutura(raiz->dir, nivel + 1);
        }

        if (raiz->nInfo == 2)
        {
            for (int i = 0; i < nivel; i++)
                printf("    ");
            printf("+-- %d\n", raiz->info2);
        }

        if (raiz->cen != NULL)
        {
            imprime_estrutura(raiz->cen, nivel + 1);
        }

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

        if (raiz->esq != NULL)
        {
            imprime_estrutura(raiz->esq, nivel + 1);
        }
    }
}

void imprime_arvore_visual(Arv23 *raiz, char *prefixo, int eh_ultimo)
{
    if (raiz == NULL)
    {
        printf("%s%s[vazio]\n", prefixo, eh_ultimo ? "+-- " : "+-- ");
    }
    else
    {
        printf("%s%s", prefixo, eh_ultimo ? "+-- " : "+-- ");
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
    if (raiz == NULL)
    {
        for (int i = 1; i <= espacos; i++)
            printf(" ");
        printf("[vazio]\n");
    }
    else
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
    int opcao, valor;

    do
    {
        printf("\n=== MENU ARVORE 2-3 ===\n");
        printf("1. Adicionar valor\n");
        printf("2. Remover valor\n");
        printf("3. Exibir arvore\n");
        printf("4. Sair\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1)
        {
            while (getchar() != '\n');
            printf("Entrada invalida. Tente novamente.\n");
            continue;
        }
        while (getchar() != '\n');

        switch (opcao)
        {
        case 1:
            printf("Digite o valor a ser inserido: ");
            if (scanf("%d", &valor) != 1)
            {
                while (getchar() != '\n');
                printf("Entrada invalida. Tente novamente.\n");
            }
            else
            {
                if (insere_23(&raiz, valor) != 0)
                {
                    printf("Valor %d inserido com sucesso.\n", valor);
                }
                else
                {
                    printf("Erro ao inserir valor %d.\n", valor);
                }
            }
            break;

        case 2:
            printf("Digite o valor a ser removido: ");
            if (scanf("%d", &valor) != 1)
            {
                while (getchar() != '\n');
                printf("Entrada invalida. Tente novamente.\n");
            }
            else
            {
                if (remover_23(&raiz, valor) != 0)
                {
                    printf("Valor %d removido com sucesso.\n", valor);
                }
                else
                {
                    printf("Erro ao remover valor %d ou valor nao encontrado.\n", valor);
                }
            }
            break;

        case 3:
            printf("\n\nArvore em ordem: ");
            imprime_23_em_ordem(raiz);
            printf("\nEstrutura da arvore:\n");
            exibir(raiz, 0);
            printf("\nVisualizacao detalhada:\n");
            imprime_arvore_visual(raiz, "", 1);
            printf("\n");
            break;

        case 4:
            printf("Saindo...\n");
            break;

        default:
            printf("Opcao invalida. Tente novamente.\n");
            break;
        }
        printf("Pressione Enter para continuar...\n");
        while (getchar() != '\n');
    } while (opcao != 4);

    return 0;
}