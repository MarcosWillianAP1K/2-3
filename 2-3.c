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

//================AUXILIARES REMOÇÃO==================

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

    return atual; // Retorna o maior nó da subárvore direita
}

Arv23 *pegar_menor_da_esquerda(Arv23 *no)
{
    Arv23 *atual = no;

    while (atual->esq != NULL)
    {
        atual = atual->esq;
    }

    return atual; // Retorna o menor nó da subárvore esquerda
}

//================CASOS DA REMOÇÃO==================

int remover_23_recursivo(Arv23 **raiz, int valor, int *sobe, Arv23 **pai);

// Caso 1:

// Caso 2:

// Caso 3:

int no_folha_com_2_infos(Arv23 **raiz, int valor)
{
    int retorno = 0;

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
    system("pause");
    return retorno;
}

// Caso 4:

int no_interno_com_2_infos(Arv23 **raiz, int valor, int *sobe, Arv23 **pai)
{
    int retorno = 0;
    int sobe = 0;

    if (valor == (*raiz)->info1) // Remover info 1
    {
        retorno = (*raiz)->info1; // Retorna o valor removido
    }
    else // remover info 2
    {
        retorno = (*raiz)->info2;                           // Retorna o valor removido
        Arv23 *dir = pegar_menor_da_esquerda((*raiz)->dir); // Pega o menor valor da direita
        Arv23 *cen = pegar_maior_da_direita((*raiz)->cen);  // Pega o maior valor do meio

        if (dir->nInfo == 2)
        {

            (*raiz)->info2 = dir->info1;               // Pega o primeiro valor do nó da direita
            (*raiz)->dir->info1 = (*raiz)->dir->info2; // Passa o segundo valor do nó da direita para o primeiro
            (*raiz)->dir->nInfo = 1;                   // Atualiza o número de informações no nó da direita
        }
        else if ((*raiz)->dir->nInfo == 1 && (*raiz)->cen->nInfo == 2)
        {
            (*raiz)->info2 = (*raiz)->cen->info2; // Pega o primeiro valor do nó do meio
            (*raiz)->cen->nInfo = 1;              // Atualiza o número de informações no nó do meio
        }
        else if (dir->nInfo == 1 && cen->nInfo == 1)
        {
           (*raiz)->info2 = dir->info1; // Pega o segundo valor do nó do meio
            
            remover_23_recursivo((*raiz)->dir, dir->info1, &sobe, raiz);
        }
    }

    return retorno;
}

//================REMOÇÃO==================

int remover_23_recursivo(Arv23 **raiz, int valor, int *sobe, Arv23 **pai)
{
    // O retorno é apenas para simular como seria pra voltar a informação do no removido
    int retorno = 0;

    if (raiz != NULL && *raiz != NULL)
    {
        if (valor < (*raiz)->info1)
        {
            retorno = remover_23_recursivo(&(*raiz)->esq, valor, sobe, raiz);
        }
        else if ((*raiz)->nInfo == 1 || ((*raiz)->nInfo == 2 && valor < (*raiz)->info2 && valor > (*raiz)->info1))
        {
            retorno = remover_23_recursivo(&(*raiz)->cen, valor, sobe, raiz);
        }
        else if ((*raiz)->nInfo == 2 && valor > (*raiz)->info2)
        {
            retorno = remover_23_recursivo(&(*raiz)->dir, valor, sobe, raiz);
        }
        else
        {
            //!!!ATENÇÃO!!!
            // APENAS O CASO SIMPLES ESTA IMPLEMNTADO
            // A ideia é fazer funções pros casos, pois tem muitos Ifs, ai fica um caos.
            // No caso aqui fica apenas os ifs de casos bases,
            // Caso 1: no folha com apenas um valor
            // Caso 2: no interno com apenas um valor
            // Caso 3: no folha com dois valores
            // Caso 4: no interno com dois valores

            // NO COM 1 INFO
            if ((*raiz)->nInfo == 1)
            {
                // Caso 1: nó folha com apenas um valor
                if (eh_folha(*raiz) == 1)
                {
                    retorno = (*raiz)->info1;

                    free(*raiz);  // Libera o nó
                    *raiz = NULL; // Define o nó como NULL
                }
                else // Caso 2: nó interno com apenas um valor
                {

                    retorno = (*raiz)->info1;

                    *sobe = (*raiz)->info1;
                    free(*raiz);
                    *raiz = NULL; // A lógica de remoção deve ser tratada pelo pai
                }
            }
            else // ELSE PARA 2 INFOS
            {
                // Caso 3: nó folha com dois valores
                if (eh_folha(*raiz) == 1)
                {
                    retorno = no_folha_com_2_infos(raiz, valor);
                }
                else // Caso 4: nó interno com dois valores
                {

                    retorno = no_interno_com_2_infos(raiz, valor, sobe, pai);
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
        retorno = 1; // Indica que o nó foi criado com sucesso
    }
    else if (valor != (*raiz)->info1 && ((*raiz)->nInfo != 2 || valor != (*raiz)->info2))
    {
        // Valor é diferente dos existentes no nó - processa inserção

        if ((*raiz)->esq == NULL)
        { // cheguei na folha

            if ((*raiz)->nInfo == 1)
            { // nó folha com apenas um valor posso adicionar o novo valor
                adiciona_infos(raiz, valor, NULL);
                retorno = 1; // Indica que o nó foi atualizado com sucesso
            }
            else
            {
                *maiorNo = quebra_no(raiz, valor, sobe, NULL);

                if (pai == NULL)
                {
                    *raiz = cria_no(*sobe, *raiz, *maiorNo);
                    *maiorNo = NULL;
                }

                retorno = 1; // Indica que o nó foi atualizado com sucesso
            }
        }
        else
        { // nó interno
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
                    retorno = 1; // Indica que o nó foi atualizado com sucesso
                }
                else
                {
                    *maiorNo = quebra_no(raiz, *sobe, sobe, *maiorNo);

                    if (pai == NULL)
                    {
                        *raiz = cria_no(*sobe, *raiz, *maiorNo);
                        *maiorNo = NULL;
                    }
                    retorno = 1; // Indica que o nó foi atualizado com sucesso
                }
            }
        }
    }
    else
    {
        // Valor duplicado - caso padrão do else
        printf("Valor %d ja existe na arvore!\n", valor); // no geral vai retornar NULL e verifica isso fora da função
    }

    return retorno;
}

// ATENÇÃO
// Notei que o inserir não retorna se deu certo ou não, então fiz a função intermediaria e agora o maiorNo é passado por referência inves de retornar

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
    int opcao, valor;

    do
    {
        system("cls||clear");
        printf("\n=== MENU ARVORE 2-3 ===\n");
        printf("1. Adicionar valor\n");
        printf("2. Remover valor\n");
        printf("3. Exibir arvore\n");
        printf("4. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        fflush(stdin); // Limpa o buffer de entrada

        switch (opcao)
        {
        case 1:
            printf("Digite o valor a ser inserido: ");
            scanf("%d", &valor);

            if (insere_23(&raiz, valor) != 0)
            {
                printf("Valor %d inserido com sucesso.\n", valor);
            }
            else
            {
                printf("Erro ao inserir valor %d.\n", valor);
            }

            break;

        case 2:
            printf("Digite o valor a ser removido: ");
            scanf("%d", &valor);
            if (remover_23(&raiz, valor) != 0)
            {
                printf("Valor %d removido com sucesso.\n", valor);
            }
            else
            {
                printf("Erro ao remover valor %d ou valor nao encontrado.\n", valor);
            }
            break;

        case 3:
            printf("\n\nArvore em ordem: ");
            imprime_23_em_ordem(raiz);
            printf("\n\nEstrutura da arvore:\n");
            exibir(raiz, 0);
            printf("\n");

            break;

        case 4:
            printf("Saindo...\n");
            break;

        default:
            printf("Opcao invalida. Tente novamente.\n");
            break;
        }
        system("pause");
    } while (opcao != 4);

    return 0;
}