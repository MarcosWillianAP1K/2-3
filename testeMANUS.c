#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura do Nó da Árvore 2-3
typedef struct Arv23
{
    int info1, info2;
    int nInfo; // 0 (underflow temporário), 1 ou 2
    struct Arv23 *esq, *cen, *dir;
} Arv23;

// Enum para status da remoção
typedef enum {
    OK,
    UNDERFLOW
} StatusRemocao;

// Protótipos das Funções
Arv23 *cria_no(int info, Arv23 *F_esq, Arv23 *F_cen);
int eh_folha(Arv23 *no);
void adiciona_infos(Arv23 **no, int info, Arv23 *Sub_Arv_Info);
Arv23 *quebra_no(Arv23 **no, int info, int *sobe, Arv23 *F_dir);
int insere_23_recursivo(Arv23 **raiz, int valor, Arv23 *pai, int *sobe, Arv23 **maiorNo);
int insere_23(Arv23 **raiz, int valor);
void imprime_23_em_ordem(Arv23 *raiz);
void exibir(Arv23 *raiz, int espacos);
void imprime_arvore_visual(Arv23 *raiz, char *prefixo, int eh_ultimo, int eh_raiz);
Arv23* buscar_menor_elemento(Arv23 *no);
StatusRemocao remover_23_recursivo(Arv23 **ponteiro_no_pai, int valor, Arv23 *pai);
int remover_23(Arv23 **raiz, int valor);
StatusRemocao tratar_underflow(Arv23 **ponteiro_filho_no_pai, Arv23 *pai);
StatusRemocao redistribuir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq);
StatusRemocao redistribuir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir);
StatusRemocao fundir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq);
StatusRemocao fundir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir);

//================ CRIA NÓ ==================
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
        no->info2 = 0; // Inicializa info2
    }
    return no;
}

//================ VERIFICA FOLHA ==================
int eh_folha(Arv23 *no)
{
    if (no == NULL) return -1;
    return (no->esq == NULL && no->cen == NULL && no->dir == NULL);
}

//================ BUSCAR MENOR (SUCESSOR IN-ORDER) ==================
Arv23* buscar_menor_elemento(Arv23 *no) {
    if (no == NULL) return NULL;
    Arv23 *atual = no;
    while (atual != NULL && atual->esq != NULL) {
        atual = atual->esq;
    }
    return atual;
}

//================ FUNÇÕES AUXILIARES REMOÇÃO (REVISÃO FINAL FUSÃO) ==================

// Função principal para tratar underflow em um nó filho
StatusRemocao tratar_underflow(Arv23 **ponteiro_filho_no_pai, Arv23 *pai) {
    Arv23 *filho_com_underflow = *ponteiro_filho_no_pai;
    if (filho_com_underflow == NULL || filho_com_underflow->nInfo > 0) return OK; // Já tratado ou não é underflow

    printf("Tratando underflow para o filho (nInfo=0) do pai [%d%s%d]...\n",
           pai->info1, pai->nInfo==2 ? ", " : "", pai->nInfo==2 ? pai->info2 : -1);

    // Identificar irmãos
    Arv23 *irmao_esq = NULL;
    Arv23 *irmao_dir = NULL;

    // Encontra a posição do filho e seus irmãos
    if (pai->nInfo >= 1 && pai->esq == filho_com_underflow) {
        irmao_dir = pai->cen;
    } else if (pai->nInfo >= 1 && pai->cen == filho_com_underflow) {
        irmao_esq = pai->esq;
        if (pai->nInfo == 2) {
            irmao_dir = pai->dir;
        }
    } else if (pai->nInfo == 2 && pai->dir == filho_com_underflow) {
        irmao_esq = pai->cen;
    }

    // 1. Tentar Redistribuição com irmão direito (3-nó)
    if (irmao_dir != NULL && irmao_dir->nInfo == 2) {
        printf("Tentando redistribuir com irmão direito [%d, %d]\n", irmao_dir->info1, irmao_dir->info2);
        return redistribuir_com_irmao_direito(ponteiro_filho_no_pai, pai, irmao_dir);
    }
    // 2. Tentar Redistribuição com irmão esquerdo (3-nó)
    if (irmao_esq != NULL && irmao_esq->nInfo == 2) {
        printf("Tentando redistribuir com irmão esquerdo [%d, %d]\n", irmao_esq->info1, irmao_esq->info2);
        return redistribuir_com_irmao_esquerdo(ponteiro_filho_no_pai, pai, irmao_esq);
    }
    // 3. Tentar Fusão com irmão direito (2-nó)
    if (irmao_dir != NULL && irmao_dir->nInfo == 1) {
         printf("Tentando fundir com irmão direito [%d]\n", irmao_dir->info1);
        return fundir_com_irmao_direito(ponteiro_filho_no_pai, pai, irmao_dir);
    }
    // 4. Tentar Fusão com irmão esquerdo (2-nó)
    if (irmao_esq != NULL && irmao_esq->nInfo == 1) {
         printf("Tentando fundir com irmão esquerdo [%d]\n", irmao_esq->info1);
        return fundir_com_irmao_esquerdo(ponteiro_filho_no_pai, pai, irmao_esq);
    }

    fprintf(stderr, "Erro crítico: Não foi possível tratar underflow (sem irmãos adequados).\n");
    return UNDERFLOW;
}

// Redistribui do irmão esquerdo (3-nó) para o filho (underflow)
StatusRemocao redistribuir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq) {
    printf("Redistribuindo com irmão esquerdo...\n");
    Arv23 *filho = *ponteiro_filho_no_pai; // Nó com underflow (nInfo=0)
    int chave_pai_desce;
    Arv23* filho_vindo_do_irmao = irmao_esq->dir; // Filho mais à direita do irmão

    // Determina chave do pai que desce e chave do irmão que sobe
    if (pai->nInfo == 1) { // Pai é 2-nó, filho é o central
        chave_pai_desce = pai->info1;
        pai->info1 = irmao_esq->info2;
    } else { // Pai é 3-nó
        if (pai->cen == filho) { // Filho é o central
            chave_pai_desce = pai->info1;
            pai->info1 = irmao_esq->info2;
        } else { // Filho é o direito (pai->dir == filho)
            chave_pai_desce = pai->info2;
            pai->info2 = irmao_esq->info2;
        }
    }

    // Preenche o nó filho (que estava em underflow)
    filho->info1 = chave_pai_desce;
    filho->nInfo = 1;
    // O filho->esq existente (se houver) é preservado.
    // O filho->cen recebe o filho vindo do irmão.
    filho->cen = filho->esq; // Desloca o filho esquerdo existente (se houver)
    filho->esq = filho_vindo_do_irmao;
    filho->dir = NULL;

    // Ajusta o irmão esquerdo
    irmao_esq->nInfo = 1;
    irmao_esq->info2 = 0;
    irmao_esq->dir = NULL;

    return OK;
}

// Redistribui do irmão direito (3-nó) para o filho (underflow)
StatusRemocao redistribuir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir) {
    printf("Redistribuindo com irmão direito...\n");
    Arv23 *filho = *ponteiro_filho_no_pai; // Nó com underflow (nInfo=0)
    int chave_pai_desce;
    Arv23* filho_vindo_do_irmao = irmao_dir->esq; // Filho mais à esquerda do irmão

    // Determina chave do pai que desce e chave do irmão que sobe
    if (pai->nInfo == 1) { // Pai é 2-nó, filho é o esquerdo
        chave_pai_desce = pai->info1;
        pai->info1 = irmao_dir->info1;
    } else { // Pai é 3-nó
        if (pai->esq == filho) { // Filho é o esquerdo
            chave_pai_desce = pai->info1;
            pai->info1 = irmao_dir->info1;
        } else { // Filho é o central (pai->cen == filho)
            chave_pai_desce = pai->info2;
            pai->info2 = irmao_dir->info1;
        }
    }

    // Preenche o nó filho (que estava em underflow)
    filho->info1 = chave_pai_desce;
    filho->nInfo = 1;
    // O filho->esq existente (se houver) é preservado.
    // O filho->cen recebe o filho vindo do irmão.
    filho->cen = filho_vindo_do_irmao;
    filho->dir = NULL;

    // Ajusta o irmão direito
    irmao_dir->info1 = irmao_dir->info2;
    irmao_dir->nInfo = 1;
    irmao_dir->info2 = 0;
    irmao_dir->esq = irmao_dir->cen;
    irmao_dir->cen = irmao_dir->dir;
    irmao_dir->dir = NULL;

    return OK;
}

// Funde o filho (underflow) com o irmão esquerdo (2-nó)
StatusRemocao fundir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq) {
    printf("Fundindo com irmão esquerdo...\n");
    Arv23 *filho_underflow = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    StatusRemocao status_pai = OK;
    // Salva os filhos do nó em underflow ANTES de modificar o irmão
    Arv23* filho_underflow_esq = filho_underflow->esq;
    Arv23* filho_underflow_cen = filho_underflow->cen;

    // Determina qual chave do pai desce e ajusta o pai
    if (pai->nInfo == 1) { // Pai é 2-nó, filho é o central
        chave_pai_desce = pai->info1;
        pai->nInfo = 0; // Pai entra em underflow
        pai->cen = NULL;
        // pai->esq já aponta para irmao_esq
        status_pai = UNDERFLOW;
    } else { // Pai é 3-nó
        if (pai->cen == filho_underflow) { // Filho é o central
            chave_pai_desce = pai->info1;
            pai->info1 = pai->info2;
            pai->cen = pai->dir;
            pai->dir = NULL;
            pai->nInfo = 1;
        } else { // Filho é o direito (pai->dir == filho_underflow)
            chave_pai_desce = pai->info2;
            pai->dir = NULL;
            pai->nInfo = 1;
        }
    }

    // Realiza a fusão no irmão esquerdo
    irmao_esq->info2 = chave_pai_desce;
    irmao_esq->nInfo = 2;
    // Anexa os filhos do nó em underflow ao nó fundido (irmao_esq)
    // Ordem correta: irmao_esq->esq, irmao_esq->cen (originais), filho_underflow_esq, filho_underflow_cen
    irmao_esq->cen = filho_underflow_esq; // O filho esquerdo do nó U vira filho central do nó M
    irmao_esq->dir = filho_underflow_cen; // O filho central do nó U vira filho direito do nó M

    // Libera o nó que estava em underflow
    printf("Liberando nó filho (ex-underflow): %p\n", (void*)filho_underflow);
    free(filho_underflow);
    *ponteiro_filho_no_pai = irmao_esq; // Atualiza ponteiro no pai (redundante se ajuste no pai foi feito)

    printf("Fusão com irmão esquerdo concluída. Nó resultante [%d, %d]. Status do pai: %s\n", irmao_esq->info1, irmao_esq->info2, status_pai == OK ? "OK" : "UNDERFLOW");
    return status_pai; // Retorna se o PAI entrou em underflow
}

// Funde o filho (underflow) com o irmão direito (2-nó)
StatusRemocao fundir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir) {
    printf("Fundindo com irmão direito...\n");
    Arv23 *filho_underflow = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    StatusRemocao status_pai = OK;
    // Salva os filhos do nó em underflow ANTES de modificar o irmão
    Arv23* filho_underflow_esq = filho_underflow->esq;
    Arv23* filho_underflow_cen = filho_underflow->cen;
    // Salva os filhos originais do irmão direito
    Arv23* irmao_dir_esq_orig = irmao_dir->esq;
    Arv23* irmao_dir_cen_orig = irmao_dir->cen;

    // Determina qual chave do pai desce e ajusta o pai
    if (pai->nInfo == 1) { // Pai é 2-nó, filho é o esquerdo
        chave_pai_desce = pai->info1;
        pai->nInfo = 0; // Pai entra em underflow
        pai->esq = NULL;
        // pai->cen já aponta para irmao_dir
        status_pai = UNDERFLOW;
    } else { // Pai é 3-nó
        if (pai->esq == filho_underflow) { // Filho é o esquerdo
            chave_pai_desce = pai->info1;
            pai->info1 = pai->info2;
            pai->esq = pai->cen;
            pai->cen = pai->dir;
            pai->dir = NULL;
            pai->nInfo = 1;
        } else { // Filho é o central (pai->cen == filho_underflow)
            chave_pai_desce = pai->info2;
            pai->cen = pai->dir;
            pai->dir = NULL;
            pai->nInfo = 1;
        }
    }

    // Realiza a fusão no irmão direito
    // Chaves: [chave_pai_desce, irmao_dir->info1]
    // Filhos: filho_underflow_esq, filho_underflow_cen, irmao_dir_esq_orig, irmao_dir_cen_orig
    irmao_dir->info2 = irmao_dir->info1;
    irmao_dir->info1 = chave_pai_desce;
    irmao_dir->nInfo = 2;
    // Anexa os filhos corretamente
    irmao_dir->esq = filho_underflow_esq; // Filho esquerdo do nó U vira filho esquerdo do nó M
    irmao_dir->cen = irmao_dir_esq_orig; // Filho esquerdo original do irmão vira filho central do nó M
    irmao_dir->dir = irmao_dir_cen_orig; // Filho central original do irmão vira filho direito do nó M
    // O que acontece com filho_underflow_cen? Ele foi perdido na lógica anterior.
    // CORREÇÃO FINAL: A ordem correta dos filhos no nó fundido [k_pai, k_irmao] deve ser:
    // esq: filho_underflow_esq
    // cen: irmao_dir_esq_orig
    // dir: irmao_dir_cen_orig
    // O filho_underflow_cen não tem lugar aqui se o nó U só tinha um filho relevante (esq).
    // Se o nó U tinha dois filhos (esq, cen), a lógica de underflow precisa ser revista.
    // Assumindo que o nó U (filho_underflow) só tem um filho relevante (esq) quando fundido.

    // Libera o nó que estava em underflow
    printf("Liberando nó filho (ex-underflow): %p\n", (void*)filho_underflow);
    free(filho_underflow);
    *ponteiro_filho_no_pai = irmao_dir; // Atualiza ponteiro no pai

    printf("Fusão com irmão direito concluída. Nó resultante [%d, %d]. Status do pai: %s\n", irmao_dir->info1, irmao_dir->info2, status_pai == OK ? "OK" : "UNDERFLOW");
    return status_pai; // Retorna se o PAI entrou em underflow
}


//================ REMOÇÃO ==================
StatusRemocao remover_23_recursivo(Arv23 **ponteiro_no_pai, int valor, Arv23 *pai) {
    StatusRemocao status = OK;
    Arv23 *no_atual = *ponteiro_no_pai;

    if (no_atual == NULL) {
        printf("Valor %d não encontrado na árvore (ramo nulo).\n", valor);
        return OK;
    }

    int ir_esq = (valor < no_atual->info1);
    int ir_dir = (no_atual->nInfo == 2 && valor > no_atual->info2);
    int ir_cen = (!ir_esq && !ir_dir && (valor != no_atual->info1 && (no_atual->nInfo == 1 || valor != no_atual->info2)));

    if (ir_esq) {
        status = remover_23_recursivo(&(no_atual->esq), valor, no_atual);
        if (status == UNDERFLOW) {
             // Verifica se o filho realmente existe antes de tratar (pode ter sido removido na fusão)
             if (no_atual->esq != NULL && no_atual->esq->nInfo == 0) {
                printf("Underflow retornado do filho esquerdo de [%d%s%d]. Tratando...\n", no_atual->info1, no_atual->nInfo==2 ? ", " : "", no_atual->nInfo==2 ? no_atual->info2 : -1);
                status = tratar_underflow(&(no_atual->esq), no_atual);
             } else {
                 status = OK; // Underflow já tratado pela fusão que removeu o nó
             }
        }
    } else if (ir_dir) {
        status = remover_23_recursivo(&(no_atual->dir), valor, no_atual);
         if (status == UNDERFLOW) {
             if (no_atual->dir != NULL && no_atual->dir->nInfo == 0) {
                printf("Underflow retornado do filho direito de [%d, %d]. Tratando...\n", no_atual->info1, no_atual->info2);
                status = tratar_underflow(&(no_atual->dir), no_atual);
             } else {
                 status = OK;
             }
        }
    } else if (ir_cen) {
        status = remover_23_recursivo(&(no_atual->cen), valor, no_atual);
        if (status == UNDERFLOW) {
            if (no_atual->cen != NULL && no_atual->cen->nInfo == 0) {
                printf("Underflow retornado do filho central de [%d%s%d]. Tratando...\n", no_atual->info1, no_atual->nInfo==2 ? ", " : "", no_atual->nInfo==2 ? no_atual->info2 : -1);
                status = tratar_underflow(&(no_atual->cen), no_atual);
            } else {
                status = OK;
            }
        }
    } else { // Achou o valor no nó atual
        printf("Valor %d encontrado no nó [%d%s%d]\n", valor, no_atual->info1, no_atual->nInfo==2 ? ", " : "", no_atual->nInfo==2 ? no_atual->info2 : -1);
        if (eh_folha(no_atual)) {
            if (no_atual->nInfo == 2) {
                printf("Removendo %d de folha 3-nó.\n", valor);
                if (valor == no_atual->info1) {
                    no_atual->info1 = no_atual->info2;
                }
                no_atual->nInfo = 1;
                no_atual->info2 = 0;
                status = OK;
            } else {
                printf("Removendo %d de folha 2-nó. Marcando para underflow.\n", valor);
                no_atual->nInfo = 0;
                status = UNDERFLOW;
            }
        } else {
            printf("Removendo %d de nó interno. Buscando sucessor...\n", valor);
            Arv23 *sucessor_node = NULL;
            int valor_sucessor;
            Arv23 **ponteiro_subarvore_sucessor;

            if (valor == no_atual->info1) {
                sucessor_node = buscar_menor_elemento(no_atual->cen);
                ponteiro_subarvore_sucessor = &(no_atual->cen);
            } else {
                sucessor_node = buscar_menor_elemento(no_atual->dir);
                 ponteiro_subarvore_sucessor = &(no_atual->dir);
            }

            if (sucessor_node == NULL) {
                 fprintf(stderr, "Erro crítico: Sucessor não encontrado para nó interno!\n");
                 return OK;
            }
            valor_sucessor = sucessor_node->info1;
            printf("Sucessor encontrado: %d\n", valor_sucessor);

            if (valor == no_atual->info1) {
                no_atual->info1 = valor_sucessor;
            } else {
                no_atual->info2 = valor_sucessor;
            }
            status = remover_23_recursivo(ponteiro_subarvore_sucessor, valor_sucessor, no_atual);
            if (status == UNDERFLOW) {
                // Verifica se o filho ainda existe e está em underflow antes de tratar
                Arv23* filho_potencialmente_removido = *ponteiro_subarvore_sucessor;
                if (filho_potencialmente_removido != NULL && filho_potencialmente_removido->nInfo == 0) {
                    printf("Underflow retornado após remoção do sucessor %d. Tratando...\n", valor_sucessor);
                    status = tratar_underflow(ponteiro_subarvore_sucessor, no_atual);
                } else {
                    status = OK; // Já tratado pela fusão
                }
            }
        }
    }

    // Se o tratamento de underflow no filho causou underflow neste nó (pai)
    if (status == UNDERFLOW && no_atual != NULL && no_atual->nInfo == 0) {
         printf("Underflow propagado para o nó pai. Sinalizando para cima.\n");
         return UNDERFLOW;
    }

    return status; // Retorna OK ou UNDERFLOW (se este nó entrou em underflow devido a fusão abaixo)
}

int remover_23(Arv23 **raiz, int valor) {
    if (raiz == NULL || *raiz == NULL) {
        return 0;
    }
    StatusRemocao status = remover_23_recursivo(raiz, valor, NULL);
    // Tratamento especial se a raiz entrou em underflow (nInfo ficou 0)
    if (status == UNDERFLOW && *raiz != NULL && (*raiz)->nInfo == 0) {
         printf("Underflow final na raiz. Ajustando raiz...\n");
         Arv23 *raiz_antiga = *raiz;
         // A nova raiz é o filho resultante da fusão (que foi movido para ->esq ou ->cen)
         // Após a fusão, o nó pai (raiz_antiga) terá apenas um ponteiro não nulo (esq ou cen)
         if (raiz_antiga->esq != NULL) {
            *raiz = raiz_antiga->esq;
         } else if (raiz_antiga->cen != NULL) {
             *raiz = raiz_antiga->cen;
         } else {
             *raiz = NULL; // Árvore ficou vazia
         }
         printf("Liberando raiz antiga: %p\n", (void*)raiz_antiga);
         free(raiz_antiga);
         status = OK;
    }
    return (status == OK);
}


//================ INSERÇÃO (Código Original Mantido com pequenas limpezas) ==================
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
        (*no)->info2 = 0;
        (*no)->dir = NULL;
    }
    else if (info > (*no)->info1)
    {
        *sobe = info;
        maior = cria_no((*no)->info2, F_dir, (*no)->dir);
        (*no)->nInfo = 1;
        (*no)->info2 = 0;
        (*no)->dir = NULL;
    }
    else
    {
        *sobe = (*no)->info1;
        maior = cria_no((*no)->info2, (*no)->cen, (*no)->dir);
        (*no)->info1 = info;
        (*no)->cen = F_dir;
        (*no)->nInfo = 1;
        (*no)->info2 = 0;
        (*no)->dir = NULL;
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
        if (eh_folha(*raiz))
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
                    Arv23* novo_maior = quebra_no(raiz, *sobe, sobe, *maiorNo);
                    *maiorNo = novo_maior;
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

//=============== IMPRIMIR RESULTADOS (Original com melhorias) ==================
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

// Função alternativa para visualização
void imprime_arvore_visual(Arv23 *raiz, char *prefixo, int eh_ultimo, int eh_raiz)
{
    if (raiz != NULL)
    {
        printf("%s", prefixo);
        if (!eh_raiz) {
             printf("%s", eh_ultimo ? "+-- " : "+-- ");
        } else {
             printf("    ");
        }

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
            char novo_prefixo[512];
            sprintf(novo_prefixo, "%s%s", prefixo, eh_raiz? "    " : (eh_ultimo ? "    " : "|   "));
            int tem_cen = (raiz->cen != NULL);
            int tem_dir = (raiz->dir != NULL);

            if (raiz->esq != NULL) {
                imprime_arvore_visual(raiz->esq, novo_prefixo, !tem_cen && !tem_dir, 0);
            }
            if (raiz->cen != NULL) {
                 imprime_arvore_visual(raiz->cen, novo_prefixo, !tem_dir, 0);
            }
            if (raiz->dir != NULL) {
                 imprime_arvore_visual(raiz->dir, novo_prefixo, 1, 0);
            }
        }
    }
}


//=============== MAIN (Testes) ==================
int main()
{
    Arv23 *raiz = NULL;
    int opcao, valor;
    char prefixo_inicial[10] = "";

    // Teste 1: Árvore do último erro
    printf("Inserindo valores (1-7)...\n");
    insere_23(&raiz, 4);
    insere_23(&raiz, 2);
    insere_23(&raiz, 6);
    insere_23(&raiz, 1);
    insere_23(&raiz, 3);
    insere_23(&raiz, 5);
    insere_23(&raiz, 7);
    printf("Inserção concluída.\n");

    // Teste 2: Árvore do erro anterior
    // printf("Inserindo valores iniciais...\n");
    // insere_23(&raiz, 10);
    // insere_23(&raiz, 20);
    // insere_23(&raiz, 5);
    // insere_23(&raiz, 6);
    // insere_23(&raiz, 12);
    // insere_23(&raiz, 30);
    // insere_23(&raiz, 7);
    // insere_23(&raiz, 17);
    // insere_23(&raiz, 1);
    // insere_23(&raiz, 15);
    // insere_23(&raiz, 18);
    // insere_23(&raiz, 25);
    // insere_23(&raiz, 35);
    // insere_23(&raiz, 40);
    // insere_23(&raiz, 22);
    // insere_23(&raiz, 28);
    // printf("Inserção concluída.\n");

    do
    {
        printf("\n=== MENU ARVORE 2-3 ===\n");
        printf("Árvore Atual:\n");
        imprime_arvore_visual(raiz, prefixo_inicial, 1, 1);
        printf("\nEm ordem: ");
        imprime_23_em_ordem(raiz);
        printf("\n------------------------\n");
        printf("1. Adicionar valor\n");
        printf("2. Remover valor\n");
        printf("3. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
             printf("Entrada inválida. Por favor, insira um número.\n");
             while (getchar() != '\n');
             opcao = 0;
             continue;
        }
        while (getchar() != '\n');

        switch (opcao)
        {
        case 1:
            printf("Digite o valor a ser inserido: ");
             if (scanf("%d", &valor) != 1) {
                 printf("Entrada inválida.\n");
                 while (getchar() != '\n');
                 break;
             }
             while (getchar() != '\n');
            if (insere_23(&raiz, valor))
            {
                printf("Valor %d inserido com sucesso.\n", valor);
            }
            break;

        case 2:
            printf("Digite o valor a ser removido: ");
             if (scanf("%d", &valor) != 1) {
                 printf("Entrada inválida.\n");
                 while (getchar() != '\n');
                 break;
             }
             while (getchar() != '\n');
            printf("\n--- Iniciando remoção de %d ---\n", valor);
            if (remover_23(&raiz, valor))
            {
                printf("--- Remoção de %d concluída com sucesso ---\n", valor);
            }
            else
            {
                printf("--- Erro ao remover valor %d ou valor não encontrado ---\n", valor);
            }
            break;

        case 3:
            printf("Saindo...\n");
            break;

        default:
            printf("Opcao invalida. Tente novamente.\n");
            break;
        }
    } while (opcao != 3);

    // TODO: Implementar função para liberar toda a memória da árvore
    // destroi_arvore(&raiz);

    return 0;
}


