#include <stdio.h>
#include <stdlib.h>
#define M 3

struct no
{
    int quantidade;
    int chaves[M - 1];
    struct no *filhos[M];
} no;

struct no *raiz = NULL;

enum EstadoChave
{
    Duplicada,
    FalhaPesquisa,
    Sucesso,
    Inserir,
    MenosChaves
};

void inserir(int chave);
void exibir(struct no *raiz, int);
void removerNo(int x);
enum EstadoChave inserirNo(struct no r, int x, int y, struct no **u);
int buscarPosicao(int x, int *arr_chaves, int n);
enum EstadoChave remover(struct no *r, int x);
void limparLinha(void);
void imprime_23_em_ordem(struct no *raiz);

int main()
{
    int chave;
    int escolha;
    while (1)
    {
        printf("1. Adicionar\n");
        printf("2. Remover\n");
        printf("3. Visualizar\n");
        printf("4. Sair\n");
        printf("Digite sua escolha: ");
        scanf("%d", &escolha);
        limparLinha();
        switch (escolha)
        {
        case 1:
            printf("Digite o numero: ");
            scanf("%d", &chave);
            limparLinha();
            inserir(chave);
            break;
        case 2:
            printf("Digite o numero: ");
            scanf("%d", &chave);
            limparLinha();
            removerNo(chave);
            break;
        case 3:
            printf("Arvore 2-3:\n");
            exibir(raiz, 0);
            printf("\nEm ordem: ");
            imprime_23_em_ordem(raiz);
            printf("\n");
            break;
        case 4:
            exit(1);
        default:
            printf("Escolha invalida.\n");
            break;
        }
    }
    return 0;
}

void inserir(int chave)
{
    struct no *novoNo;
    int chaveSuperior;
    enum EstadoChave valor;
    valor = inserirNo(raiz, chave, &chaveSuperior, &novoNo);
    if (valor == Duplicada)
        printf("Numero ja existe\n");
    if (valor == Inserir)
    {
        struct no *raizSuperior = raiz;
        raiz = malloc(sizeof(struct no));
        raiz->quantidade = 1;
        raiz->chaves[0] = chaveSuperior;
        raiz->filhos[0] = raizSuperior;
        raiz->filhos[1] = novoNo;
    }
}

enum EstadoChave inserirNo(struct no *ptr, int chave, int *chaveSuperior, struct no **novoNo)
{
    struct no *novoPtr, *ultimoPtr;
    int pos, i, n, posDivisao;
    int novaChave, ultimaChave;
    enum EstadoChave valor;
    if (ptr == NULL)
    {
        *novoNo = NULL;
        *chaveSuperior = chave;
        return Inserir;
    }
    n = ptr->quantidade;
    pos = buscarPosicao(chave, ptr->chaves, n);
    if (pos < n && chave == ptr->chaves[pos])
        return Duplicada;
    valor = inserirNo(ptr->filhos[pos], chave, &novaChave, &novoPtr);
    if (valor != Inserir)
        return valor;
    if (n < M - 1)
    {
        pos = buscarPosicao(novaChave, ptr->chaves, n);
        for (i = n; i > pos; i--)
        {
            ptr->chaves[i] = ptr->chaves[i - 1];
            ptr->filhos[i + 1] = ptr->filhos[i];
        }
        ptr->chaves[pos] = novaChave;
        ptr->filhos[pos + 1] = novoPtr;
        ++ptr->quantidade;
        return Sucesso;
    }
    if (pos == M - 1)
    {
        ultimaChave = novaChave;
        ultimoPtr = novoPtr;
    }
    else
    {
        ultimaChave = ptr->chaves[M - 2];
        ultimoPtr = ptr->filhos[M - 1];
        for (i = M - 2; i > pos; i--)
        {
            ptr->chaves[i] = ptr->chaves[i - 1];
            ptr->filhos[i + 1] = ptr->filhos[i];
        }
        ptr->chaves[pos] = novaChave;
        ptr->filhos[pos + 1] = novoPtr;
    }
    posDivisao = (M - 1) / 2;
    (*chaveSuperior) = ptr->chaves[posDivisao];

    (*novoNo) = malloc(sizeof(struct no));
    ptr->quantidade = posDivisao;
    (*novoNo)->quantidade = M - 1 - posDivisao;
    for (i = 0; i < (*novoNo)->quantidade; i++)
    {
        (*novoNo)->filhos[i] = ptr->filhos[i + posDivisao + 1];
        if (i < (*novoNo)->quantidade - 1)
            (*novoNo)->chaves[i] = ptr->chaves[i + posDivisao + 1];
        else
            (*novoNo)->chaves[i] = ultimaChave;
    }
    (*novoNo)->filhos[(*novoNo)->quantidade] = ultimoPtr;
    return Inserir;
}

void exibir(struct no *ptr, int espacos)
{
    if (ptr)
    {
        int i;
        for (i = 1; i <= espacos; i++)
            printf(" ");
        for (i = 0; i < ptr->quantidade; i++)
            printf("%d ", ptr->chaves[i]);
        printf("\n");
        for (i = 0; i <= ptr->quantidade; i++)
            exibir(ptr->filhos[i], espacos + 10);
    }
}

void imprime_23_em_ordem(struct no *raiz)
{

    if (raiz != NULL)
    {
        imprime_23_em_ordem(raiz->filhos[0]);
        printf("%d ", raiz->chaves[0]);
        imprime_23_em_ordem(raiz->filhos[1]);
        if (raiz->quantidade == 2)
        {
            printf("%d ", raiz->chaves[1]);
            imprime_23_em_ordem(raiz->filhos[2]);
        }
    }
}

int buscarPosicao(int chave, int *arr_chaves, int n)
{
    int pos = 0;
    while (pos < n && chave > arr_chaves[pos])
        pos++;
    return pos;
}

void removerNo(int chave)
{
    struct no *raizSuperior;
    enum EstadoChave valor;
    valor = remover(raiz, chave);
    switch (valor)
    {
    case FalhaPesquisa:
        printf("Numero %d nao encontrado\n", chave);
        break;
    case MenosChaves:
        raizSuperior = raiz;
        raiz = raiz->filhos[0];
        free(raizSuperior);
        break;
    case Duplicada:
        printf("Operacao com numero duplicado %d\n", chave);
        break;
    case Sucesso:
        printf("Numero %d removido com sucesso\n", chave);
        break;
    case Inserir:
        printf("Operacao de insercao nao esperada durante remocao\n");
        break;
    }
}

enum EstadoChave remover(struct no *ptr, int chave)
{
    int pos, i, pivo, n, minimo;
    int *arr_chaves;
    enum EstadoChave valor;
    struct no **filhos, *noEsquerda, *noDireita;

    if (ptr == NULL)
        return FalhaPesquisa;
    n = ptr->quantidade;
    arr_chaves = ptr->chaves;
    filhos = ptr->filhos;
    minimo = (M - 1) / 2;

    pos = buscarPosicao(chave, arr_chaves, n);
    if (filhos[0] == NULL)
    {
        if (pos == n || chave < arr_chaves[pos])
            return FalhaPesquisa;
        for (i = pos + 1; i < n; i++)
        {
            arr_chaves[i - 1] = arr_chaves[i];
            filhos[i] = filhos[i + 1];
        }
        return --ptr->quantidade >= (ptr == raiz ? 1 : minimo) ? Sucesso : MenosChaves;
    }

    if (pos < n && chave == arr_chaves[pos])
    {
        struct no *qp = filhos[pos], *qp1;
        int nchave;
        while (1)
        {
            nchave = qp->quantidade;
            qp1 = qp->filhos[nchave];
            if (qp1 == NULL)
                break;
            qp = qp1;
        }
        arr_chaves[pos] = qp->chaves[nchave - 1];
        qp->chaves[nchave - 1] = chave;
    }
    valor = remover(filhos[pos], chave);
    if (valor != MenosChaves)
        return valor;

    if (pos > 0 && filhos[pos - 1]->quantidade > minimo)
    {
        pivo = pos - 1;
        noEsquerda = filhos[pivo];
        noDireita = filhos[pos];

        noDireita->filhos[noDireita->quantidade + 1] = noDireita->filhos[noDireita->quantidade];
        for (i = noDireita->quantidade; i > 0; i--)
        {
            noDireita->chaves[i] = noDireita->chaves[i - 1];
            noDireita->filhos[i] = noDireita->filhos[i - 1];
        }
        noDireita->quantidade++;
        noDireita->chaves[0] = arr_chaves[pivo];
        noDireita->filhos[0] = noEsquerda->filhos[noEsquerda->quantidade];
        arr_chaves[pivo] = noEsquerda->chaves[--noEsquerda->quantidade];
        return Sucesso;
    }

    if (pos < n && filhos[pos + 1]->quantidade > minimo)
    {
        pivo = pos;
        noEsquerda = filhos[pivo];
        noDireita = filhos[pivo + 1];

        noEsquerda->chaves[noEsquerda->quantidade] = arr_chaves[pivo];
        noEsquerda->filhos[noEsquerda->quantidade + 1] = noDireita->filhos[0];
        arr_chaves[pivo] = noDireita->chaves[0];
        noEsquerda->quantidade++;
        noDireita->quantidade--;
        for (i = 0; i < noDireita->quantidade; i++)
        {
            noDireita->chaves[i] = noDireita->chaves[i + 1];
            noDireita->filhos[i] = noDireita->filhos[i + 1];
        }
        noDireita->filhos[noDireita->quantidade] = noDireita->filhos[noDireita->quantidade + 1];
        return Sucesso;
    }

    if (pos == n)
        pivo = pos - 1;
    else
        pivo = pos;

    noEsquerda = filhos[pivo];
    noDireita = filhos[pivo + 1];

    noEsquerda->chaves[noEsquerda->quantidade] = arr_chaves[pivo];
    noEsquerda->filhos[noEsquerda->quantidade + 1] = noDireita->filhos[0];
    for (i = 0; i < noDireita->quantidade; i++)
    {
        noEsquerda->chaves[noEsquerda->quantidade + 1 + i] = noDireita->chaves[i];
        noEsquerda->filhos[noEsquerda->quantidade + 2 + i] = noDireita->filhos[i + 1];
    }
    noEsquerda->quantidade = noEsquerda->quantidade + noDireita->quantidade + 1;
    free(noDireita);
    for (i = pos + 1; i < n; i++)
    {
        arr_chaves[i - 1] = arr_chaves[i];
        filhos[i] = filhos[i + 1];
    }
    return --ptr->quantidade >= (ptr == raiz ? 1 : minimo) ? Sucesso : MenosChaves;
}

void limparLinha(void)
{
    while (getchar() != '\n')
        ;
}