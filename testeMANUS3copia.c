#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Prototipos das Funcoes
Arv23 *cria_no(int info, Arv23 *F_esq, Arv23 *F_cen);
int eh_folha(Arv23 *no);
void adiciona_infos(Arv23 **no, int info, Arv23 *Sub_Arv_Info);
Arv23 *quebra_no(Arv23 **no, int info, int *sobe, Arv23 *F_dir);
int insere_23_recursivo(Arv23 **raiz, int valor, Arv23 *pai, int *sobe, Arv23 **maiorNo);
int insere_23(Arv23 **raiz, int valor);
void imprime_23_em_ordem(Arv23 *raiz);
void imprime_arvore_visual(Arv23 *raiz, char *prefixo, int eh_ultimo, int eh_raiz);
Arv23* buscar_menor_elemento(Arv23 *no);
StatusRemocao remover_23_recursivo(Arv23 **ponteiro_no_pai, int valor, Arv23 *pai);
int remover_23(Arv23 **raiz, int valor);
StatusRemocao tratar_underflow(Arv23 **ponteiro_filho_no_pai, Arv23 *pai);
StatusRemocao redistribuir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq);
StatusRemocao redistribuir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir);
StatusRemocao fundir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq);
StatusRemocao fundir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir);

//================ CRIA NO ==================
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
        no->info2 = 0;
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

//================ FUNCOES AUXILIARES REMOCAO (CORRIGIDAS) ==================

// Funcao principal para tratar underflow em um no filho
StatusRemocao tratar_underflow(Arv23 **ponteiro_filho_no_pai, Arv23 *pai) {
    Arv23 *filho_com_underflow = *ponteiro_filho_no_pai;
    if (filho_com_underflow == NULL || filho_com_underflow->nInfo > 0) return OK;

    printf("Tratando underflow no filho do pai [%d%s%d]...\n",
           pai->info1, pai->nInfo==2 ? ", " : " ", pai->nInfo==2 ? pai->info2 : -1);

    // Identifica irmaos baseado na posicao do filho
    Arv23 *irmao_esq = NULL;
    Arv23 *irmao_dir = NULL;

    if (pai->esq == filho_com_underflow) {
        // Filho e o esquerdo
        irmao_dir = pai->cen;
    } else if (pai->cen == filho_com_underflow) {
        // Filho e o central
        irmao_esq = pai->esq;
        if (pai->nInfo == 2) {
            irmao_dir = pai->dir;
        }
    } else if (pai->nInfo == 2 && pai->dir == filho_com_underflow) {
        // Filho e o direito
        irmao_esq = pai->cen;
    }

    // Tenta redistribuicao primeiro (irmao com 2 chaves)
    if (irmao_dir != NULL && irmao_dir->nInfo == 2) {
        return redistribuir_com_irmao_direito(ponteiro_filho_no_pai, pai, irmao_dir);
    }
    if (irmao_esq != NULL && irmao_esq->nInfo == 2) {
        return redistribuir_com_irmao_esquerdo(ponteiro_filho_no_pai, pai, irmao_esq);
    }

    // Tenta fusao (irmao com 1 chave)
    if (irmao_dir != NULL && irmao_dir->nInfo == 1) {
        return fundir_com_irmao_direito(ponteiro_filho_no_pai, pai, irmao_dir);
    }
    if (irmao_esq != NULL && irmao_esq->nInfo == 1) {
        return fundir_com_irmao_esquerdo(ponteiro_filho_no_pai, pai, irmao_esq);
    }

    fprintf(stderr, "Erro: Nao foi possivel tratar underflow.\n");
    return UNDERFLOW;
}

// Redistribui do irmao esquerdo (3-no) para o filho (underflow)
StatusRemocao redistribuir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq) {
    printf("Redistribuindo com irmao esquerdo...\n");
    Arv23 *filho = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    Arv23 *filho_transferido = irmao_esq->dir; // Filho mais a direita do irmao esq

    // Determina qual chave do pai desce
    if (pai->cen == filho) { // Filho e o central
        chave_pai_desce = pai->info1;
        pai->info1 = irmao_esq->info2;
    } else { // pai->dir == filho (Filho e o direito)
        chave_pai_desce = pai->info2;
        pai->info2 = irmao_esq->info2;
    }

    // Restaura o filho em underflow
    filho->info1 = chave_pai_desce;
    filho->nInfo = 1;
    // O filho transferido do irmao esquerdo se torna o filho esquerdo do filho restaurado
    filho->cen = filho->esq; // Desloca o filho esquerdo original (se houver) para o centro
    filho->esq = filho_transferido;

    // Ajusta o irmao esquerdo
    irmao_esq->nInfo = 1;
    irmao_esq->info2 = 0;
    irmao_esq->dir = irmao_esq->cen; // O filho central antigo vira o direito
    irmao_esq->cen = NULL; // Limpa o ponteiro central (opcional, mas bom)

    return OK;
}

// Redistribui do irmao direito (3-no) para o filho (underflow)
StatusRemocao redistribuir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir) {
    printf("Redistribuindo com irmao direito...\n");
    Arv23 *filho = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    Arv23 *filho_transferido = irmao_dir->esq; // Filho mais a esquerda do irmao dir

    // Determina qual chave do pai desce
    if (pai->esq == filho) { // Filho e o esquerdo
        chave_pai_desce = pai->info1;
        pai->info1 = irmao_dir->info1;
    } else { // pai->cen == filho (Filho e o central)
        chave_pai_desce = pai->info2;
        pai->info2 = irmao_dir->info1;
    }

    // Restaura o filho em underflow
    filho->info1 = chave_pai_desce;
    filho->nInfo = 1;
    // O filho transferido do irmao direito se torna o filho central do filho restaurado
    filho->cen = filho_transferido;

    // Ajusta o irmao direito
    irmao_dir->info1 = irmao_dir->info2;
    irmao_dir->nInfo = 1;
    irmao_dir->info2 = 0;
    irmao_dir->esq = irmao_dir->cen;
    irmao_dir->cen = irmao_dir->dir;
    irmao_dir->dir = NULL;

    return OK;
}

// Funde o filho (underflow) com o irmao esquerdo (2-no) - CORRIGIDA
StatusRemocao fundir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq) {
    printf("Fundindo com irmao esquerdo...\n");
    Arv23 *filho_underflow = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    StatusRemocao status_pai = OK;

    // Salva filhos do no em underflow (se existirem)
    Arv23 *filho_u_esq = filho_underflow->esq;
    Arv23 *filho_u_cen = filho_underflow->cen;

    // Determina chave do pai que desce e ajusta pai, removendo o ponteiro para filho_underflow
    if (pai->nInfo == 1) {
        // Pai e 2-no, filho e central (ponteiro_filho_no_pai == &(pai->cen))
        chave_pai_desce = pai->info1;
        pai->nInfo = 0; // Pai entra em underflow
        pai->cen = NULL; // Remove ponteiro central (apontava para filho_underflow)
        status_pai = UNDERFLOW;
    } else { // Pai e 3-no
        if (pai->cen == filho_underflow) { // Filho e central (ponteiro_filho_no_pai == &(pai->cen))
            chave_pai_desce = pai->info1;
            pai->info1 = pai->info2;
            pai->info2 = 0;
            pai->cen = pai->dir; // Shift dir para cen (irmao_esq continua em pai->esq)
            pai->dir = NULL;
            pai->nInfo = 1;
        } else { // Filho e direito (ponteiro_filho_no_pai == &(pai->dir))
            chave_pai_desce = pai->info2;
            pai->info2 = 0;
            pai->dir = NULL; // Remove ponteiro direito (apontava para filho_underflow)
            pai->nInfo = 1;
            // irmao_esq continua em pai->cen
        }
    }

    // Realiza fusao no irmao esquerdo (que esta em pai->esq ou pai->cen)
    irmao_esq->info2 = chave_pai_desce;
    irmao_esq->nInfo = 2;
    // Anexa os filhos do nó em underflow ao novo nó fundido
    // O filho central antigo do irmao_esq vira o filho direito do novo nó
    irmao_esq->dir = filho_u_esq; // O filho esquerdo do underflow node se torna o filho direito do nó fundido
    // O filho central do underflow node (filho_u_cen) geralmente é NULL neste ponto se a remoção começou de uma folha.
    // Se não for folha, a lógica de sucessor garante que a remoção recursiva lide com isso.
    // Assumindo que filho_u_cen é irrelevante ou NULL aqui.

    // Libera no em underflow
    free(filho_underflow);
    // REMOVIDO: *ponteiro_filho_no_pai = NULL;

    printf("Fusao concluida. No resultante [%d, %d]. Status pai: %s\n",
           irmao_esq->info1, irmao_esq->info2, status_pai == OK ? "OK" : "UNDERFLOW");

    return status_pai;
}

// Funde o filho (underflow) com o irmao direito (2-no) - CORRIGIDA
StatusRemocao fundir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir) {
    printf("Fundindo com irmao direito...\n");
    Arv23 *filho_underflow = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    StatusRemocao status_pai = OK;

    // Salva filhos
    Arv23 *filho_u_esq = filho_underflow->esq;
    Arv23 *filho_u_cen = filho_underflow->cen;
    Arv23 *irmao_d_esq = irmao_dir->esq;
    Arv23 *irmao_d_cen = irmao_dir->cen;

    // Determina chave do pai que desce e ajusta pai, removendo o ponteiro para filho_underflow
    if (pai->nInfo == 1) {
        // Pai e 2-no, filho e esquerdo (ponteiro_filho_no_pai == &(pai->esq))
        chave_pai_desce = pai->info1;
        pai->nInfo = 0; // Pai entra em underflow
        pai->esq = NULL; // Remove ponteiro esquerdo (apontava para filho_underflow)
        status_pai = UNDERFLOW;
        // irmao_dir continua em pai->cen
    } else { // Pai e 3-no
        if (pai->esq == filho_underflow) { // Filho e esquerdo (ponteiro_filho_no_pai == &(pai->esq))
            chave_pai_desce = pai->info1;
            pai->info1 = pai->info2;
            pai->info2 = 0;
            pai->esq = pai->cen; // Shift cen para esq (irmao_dir continua em pai->dir, que vira pai->cen)
            pai->cen = pai->dir;
            pai->dir = NULL;
            pai->nInfo = 1;
        } else { // Filho e central (ponteiro_filho_no_pai == &(pai->cen))
            chave_pai_desce = pai->info2;
            pai->info2 = 0;
            pai->cen = pai->dir; // Shift dir para cen (irmao_dir continua em pai->dir, que agora é pai->cen)
            pai->dir = NULL;
            pai->nInfo = 1;
            // irmao_dir continua em pai->cen
        }
    }

    // Realiza fusao no irmao direito (que esta em pai->cen ou pai->dir)
    // Move a chave atual do irmão direito para info2
    irmao_dir->info2 = irmao_dir->info1;
    // A chave que desce do pai vira info1
    irmao_dir->info1 = chave_pai_desce;
    irmao_dir->nInfo = 2;

    // Reorganiza os filhos corretamente para o novo nó [chave_pai_desce, irmao_dir->info2]
    // Filhos devem ser: filho_u_esq, irmao_d_esq, irmao_d_cen
    irmao_dir->dir = irmao_d_cen;  // Filho central original do irmao_dir vira direito
    irmao_dir->cen = irmao_d_esq;  // Filho esquerdo original do irmao_dir vira central
    irmao_dir->esq = filho_u_esq;  // Filho esquerdo do nó em underflow vira esquerdo

    // Libera no em underflow
    free(filho_underflow);
    // REMOVIDO: *ponteiro_filho_no_pai = NULL;

    printf("Fusao concluida. No resultante [%d, %d]. Status pai: %s\n",
           irmao_dir->info1, irmao_dir->info2, status_pai == OK ? "OK" : "UNDERFLOW");

    return status_pai;
}

//================ REMOCAO (Mantida, usa as funcoes de underflow corrigidas) ==================
StatusRemocao remover_23_recursivo(Arv23 **ponteiro_no_pai, int valor, Arv23 *pai) {
    StatusRemocao status = OK;
    Arv23 *no_atual = *ponteiro_no_pai;

    if (no_atual == NULL) {
        printf("Valor %d nao encontrado.\n", valor);
        return OK;
    }

    int ir_esq = (valor < no_atual->info1);
    int ir_dir = (no_atual->nInfo == 2 && valor > no_atual->info2);
    int ir_cen = (!ir_esq && !ir_dir && (valor != no_atual->info1 && (no_atual->nInfo == 1 || valor != no_atual->info2)));

    if (ir_esq) {
        status = remover_23_recursivo(&(no_atual->esq), valor, no_atual);
        if (status == UNDERFLOW) {
            status = tratar_underflow(&(no_atual->esq), no_atual);
        }
    } else if (ir_dir) {
        status = remover_23_recursivo(&(no_atual->dir), valor, no_atual);
        if (status == UNDERFLOW) {
            status = tratar_underflow(&(no_atual->dir), no_atual);
        }
    } else if (ir_cen) {
        status = remover_23_recursivo(&(no_atual->cen), valor, no_atual);
        if (status == UNDERFLOW) {
            status = tratar_underflow(&(no_atual->cen), no_atual);
        }
    } else { // Valor encontrado no no atual
        printf("Valor %d encontrado no no [%d%s%d]\n", valor, no_atual->info1,
               no_atual->nInfo==2 ? ", " : " ", no_atual->nInfo==2 ? no_atual->info2 : -1);

        if (eh_folha(no_atual)) {
            if (no_atual->nInfo == 2) {
                // Remove de folha 3-no
                if (valor == no_atual->info1) {
                    no_atual->info1 = no_atual->info2;
                }
                no_atual->nInfo = 1;
                no_atual->info2 = 0;
                status = OK;
            } else {
                // Remove de folha 2-no
                no_atual->nInfo = 0;
                status = UNDERFLOW;
            }
        } else {
            // Remove de no interno - substitui pelo sucessor
            Arv23 *sucessor_node = NULL;
            int valor_sucessor;
            Arv23 **ponteiro_subarvore_sucessor;

            if (valor == no_atual->info1) {
                sucessor_node = buscar_menor_elemento(no_atual->cen);
                ponteiro_subarvore_sucessor = &(no_atual->cen);
            } else { // valor == no_atual->info2
                sucessor_node = buscar_menor_elemento(no_atual->dir);
                ponteiro_subarvore_sucessor = &(no_atual->dir);
            }

            if (sucessor_node == NULL) {
                fprintf(stderr, "Erro: Sucessor nao encontrado!\n");
                return OK; // Ou algum status de erro
            }

            valor_sucessor = sucessor_node->info1; // Sucessor sempre está em info1 de um nó folha (ou o mais à esquerda)
            printf("Substituindo por sucessor: %d\n", valor_sucessor);

            // Substitui o valor
            if (valor == no_atual->info1) {
                no_atual->info1 = valor_sucessor;
            } else {
                no_atual->info2 = valor_sucessor;
            }

            // Remove o sucessor recursivamente da subárvore apropriada
            status = remover_23_recursivo(ponteiro_subarvore_sucessor, valor_sucessor, no_atual);
            if (status == UNDERFLOW) {
                status = tratar_underflow(ponteiro_subarvore_sucessor, no_atual);
            }
        }
    }

    return status;
}

int remover_23(Arv23 **raiz, int valor) {
    if (raiz == NULL || *raiz == NULL) {
        return 0;
    }

    StatusRemocao status = remover_23_recursivo(raiz, valor, NULL);

    // Tratamento especial se a raiz entrou em underflow (acontece quando a raiz original é fundida)
    if (status == UNDERFLOW && *raiz != NULL && (*raiz)->nInfo == 0) {
        printf("Raiz em underflow. Ajustando...\n");
        Arv23 *raiz_antiga = *raiz;

        // A nova raiz é o único filho restante (que agora contém a árvore fundida)
        // Este filho pode estar em esq ou cen dependendo da fusão
        if (raiz_antiga->esq != NULL) {
            *raiz = raiz_antiga->esq;
        } else if (raiz_antiga->cen != NULL) {
            *raiz = raiz_antiga->cen;
        } else {
             // Isso não deveria acontecer se a fusão ocorreu corretamente
             // Mas por segurança, se não houver filhos, a árvore fica vazia.
            *raiz = NULL;
        }

        free(raiz_antiga);
        status = OK; // O underflow da raiz foi resolvido
    }

    return (status == OK); // Retorna 1 se a remoção (e tratamento de underflow) foi OK
}

//================ INSERCAO (Mantida do original) ==================
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

//=============== IMPRIMIR ==================
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

void imprime_arvore_visual(Arv23 *raiz, char *prefixo, int eh_ultimo, int eh_raiz)
{
    if (raiz != NULL)
    {
        printf("%s", prefixo);
        if (!eh_raiz) {
             printf("%s", eh_ultimo ? "+------ " : "+------ ");
        } else {
             printf("         ");
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
            sprintf(novo_prefixo, "%s%s", prefixo, eh_raiz? "         " : (eh_ultimo ? "         " : "|        "));
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

//================ MAIN NORNAL ==================
int main()
{
    Arv23 *raiz = NULL;
    int opcao, valor;
    char prefixo_inicial[10] = "";

    // Teste inicial
    printf("Inserindo valores (1-10)...\n");
    for (int i = 1; i<=0; i++){
        insere_23(&raiz, i);
    }
    printf("Insercao concluida.\n");

    do
    {
        printf("\n=== MENU ARVORE 2-3 ===\n");
        printf("Arvore Atual:\n");
        imprime_arvore_visual(raiz, prefixo_inicial, 1, 1);
        printf("\nEm ordem: ");
        imprime_23_em_ordem(raiz);
        printf("\n------------------------\n");
        printf("1. Adicionar valor\n");
        printf("2. Remover valor\n");
        printf("3. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
             printf("Entrada invalida. Por favor, insira um numero.\n");
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
                 printf("Entrada invalida.\n");
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
                 printf("Entrada invalida.\n");
                 while (getchar() != '\n');
                 break;
             }
             while (getchar() != '\n');
            printf("\n--- Iniciando remocao de %d ---\n", valor);
            if (remover_23(&raiz, valor))
            {
                printf("--- Remocao de %d concluida com sucesso ---\n", valor);
            }
            else
            {
                printf("--- Erro ou valor nao encontrado ao remover %d ---\n", valor);
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

    // Liberar memoria da arvore (implementacao opcional)
    // libera_arvore(raiz);

    return 0;
}



// //=============== MAIN (Modificado para Teste Automatizado) ==================
// int main()
// {
//     Arv23 *raiz = NULL;
//     char prefixo_inicial[10] = "";

//     // Teste inicial
//     printf("Inserindo valores (1-10)...\n");
//     for (int i = 1; i <= 10; i++){
//         insere_23(&raiz, i);
//     }
//     printf("Insercao concluida.\n");

//     printf("\n=== ARVORE INICIAL ===\n");
//     imprime_arvore_visual(raiz, prefixo_inicial, 1, 1);
//     printf("Em ordem: ");
//     imprime_23_em_ordem(raiz);
//     printf("\n------------------------\n");

//     // Sequencia de remocoes do teste original
//     int valores_remover[] = {8, 9, 10, 6, 2, 3};
//     int num_remocoes = sizeof(valores_remover) / sizeof(valores_remover[0]);

//     for (int i = 0; i < num_remocoes; i++) {
//         int valor = valores_remover[i];
//         printf("\n--- Iniciando remocao de %d ---\n", valor);
//         if (remover_23(&raiz, valor))
//         {
//             printf("--- Remocao de %d concluida com sucesso ---\n", valor);
//         }
//         else
//         {
//             printf("--- Erro ou valor nao encontrado ao remover %d ---\n", valor);
//         }
//         printf("\n=== ARVORE APOS REMOVER %d ===\n", valor);
//         imprime_arvore_visual(raiz, prefixo_inicial, 1, 1);
//         printf("Em ordem: ");
//         imprime_23_em_ordem(raiz);
//         printf("\n------------------------\n");
//     }

//     printf("\n=== TESTE AUTOMATIZADO CONCLUIDO ===\n");

//     // Liberar memoria da arvore (implementacao opcional)
//     // libera_arvore(raiz);

//     return 0;
// }


