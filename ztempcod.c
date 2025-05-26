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
StatusRemocao redistribuir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq, int pos_filho);
StatusRemocao redistribuir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir, int pos_filho);
StatusRemocao fundir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq, int pos_filho);
StatusRemocao fundir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir, int pos_filho);

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
    // Se o filho nao existe ou nao esta em underflow, nao ha nada a fazer
    if (filho_com_underflow == NULL || filho_com_underflow->nInfo > 0) return OK;

    // Se o pai nao existe (significa que o filho e a raiz), o underflow sera tratado na funcao remover_23
    if (pai == NULL) return UNDERFLOW; // Sinaliza para tratar na raiz

    printf("Tratando underflow no filho (nInfo=0) do pai [%d%s%d]...\n",
           pai->info1, pai->nInfo==2 ? ", " : " ", pai->nInfo==2 ? pai->info2 : -1);

    // Identifica irmaos baseado na posicao do filho
    Arv23 *irmao_esq = NULL;
    Arv23 *irmao_dir = NULL;
    int pos_filho = -1; // 0=esq, 1=cen, 2=dir

    if (pai->esq == filho_com_underflow) {
        pos_filho = 0;
        irmao_dir = pai->cen;
    } else if (pai->cen == filho_com_underflow) {
        pos_filho = 1;
        irmao_esq = pai->esq;
        if (pai->nInfo == 2) {
            irmao_dir = pai->dir;
        }
    } else if (pai->nInfo == 2 && pai->dir == filho_com_underflow) {
        pos_filho = 2;
        irmao_esq = pai->cen;
    }

    // Tenta redistribuicao primeiro (irmao com 2 chaves)
    if (irmao_dir != NULL && irmao_dir->nInfo == 2) {
        printf("Tentando redistribuicao com irmao direito [%d, %d]\n", irmao_dir->info1, irmao_dir->info2);
        return redistribuir_com_irmao_direito(ponteiro_filho_no_pai, pai, irmao_dir, pos_filho);
    }
    if (irmao_esq != NULL && irmao_esq->nInfo == 2) {
        printf("Tentando redistribuicao com irmao esquerdo [%d, %d]\n", irmao_esq->info1, irmao_esq->info2);
        return redistribuir_com_irmao_esquerdo(ponteiro_filho_no_pai, pai, irmao_esq, pos_filho);
    }

    // Tenta fusao (irmao com 1 chave)
    if (irmao_dir != NULL && irmao_dir->nInfo == 1) {
        printf("Tentando fusao com irmao direito [%d]\n", irmao_dir->info1);
        return fundir_com_irmao_direito(ponteiro_filho_no_pai, pai, irmao_dir, pos_filho);
    }
    if (irmao_esq != NULL && irmao_esq->nInfo == 1) {
        printf("Tentando fusao com irmao esquerdo [%d]\n", irmao_esq->info1);
        return fundir_com_irmao_esquerdo(ponteiro_filho_no_pai, pai, irmao_esq, pos_filho);
    }

    fprintf(stderr, "Erro critico: Nao foi possivel tratar underflow. Pai: [%d%s%d], Filho pos: %d\n",
            pai->info1, pai->nInfo==2 ? ", " : " ", pai->nInfo==2 ? pai->info2 : -1, pos_filho);
    // Isso nao deveria acontecer em uma arvore 2-3 valida se a logica estiver correta
    return OK; // Evita loop infinito, mas indica um problema
}

// Redistribui do irmao esquerdo (3-no) para o filho (underflow)
StatusRemocao redistribuir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq, int pos_filho) {
    printf("Redistribuindo com irmao esquerdo...\n");
    Arv23 *filho = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    Arv23 *filho_transferido = irmao_esq->dir; // Filho mais a direita do irmao esq (que sera movido)

    // Determina qual chave do pai desce e qual sobe do irmao
    if (pos_filho == 1) { // Filho e o central (pai->cen)
        chave_pai_desce = pai->info1;
        pai->info1 = irmao_esq->info2;
    } else { // pos_filho == 2 (Filho e o direito, pai->dir)
        chave_pai_desce = pai->info2;
        pai->info2 = irmao_esq->info2;
    }

    // Restaura o filho em underflow para ser um 2-no
    filho->info1 = chave_pai_desce;
    filho->nInfo = 1;
    // O filho transferido do irmao esquerdo se torna o filho esquerdo do filho restaurado
    filho->cen = filho->esq; // Desloca o filho esquerdo original (se houver) para o centro
    filho->esq = filho_transferido;

    // Ajusta o irmao esquerdo (era 3-no, vira 2-no)
    irmao_esq->nInfo = 1;
    irmao_esq->info2 = 0;
    irmao_esq->dir = irmao_esq->cen; // O filho central antigo vira o direito
    irmao_esq->cen = NULL; // Nao tem mais filho central

    return OK;
}

// Redistribui do irmao direito (3-no) para o filho (underflow)
StatusRemocao redistribuir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir, int pos_filho) {
    printf("Redistribuindo com irmao direito...\n");
    Arv23 *filho = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    Arv23 *filho_transferido = irmao_dir->esq; // Filho mais a esquerda do irmao dir (que sera movido)

    // Determina qual chave do pai desce e qual sobe do irmao
    if (pos_filho == 0) { // Filho e o esquerdo (pai->esq)
        chave_pai_desce = pai->info1;
        pai->info1 = irmao_dir->info1;
    } else { // pos_filho == 1 (Filho e o central, pai->cen)
        chave_pai_desce = pai->info2;
        pai->info2 = irmao_dir->info1;
    }

    // Restaura o filho em underflow para ser um 2-no
    filho->info1 = chave_pai_desce;
    filho->nInfo = 1;
    // O filho transferido do irmao direito se torna o filho central do filho restaurado
    // O filho esquerdo original do filho (se houver) permanece em filho->esq
    filho->cen = filho_transferido;

    // Ajusta o irmao direito (era 3-no, vira 2-no)
    irmao_dir->info1 = irmao_dir->info2;
    irmao_dir->nInfo = 1;
    irmao_dir->info2 = 0;
    irmao_dir->esq = irmao_dir->cen;
    irmao_dir->cen = irmao_dir->dir;
    irmao_dir->dir = NULL;

    return OK;
}

// Funde o filho (underflow) com o irmao esquerdo (2-no)
StatusRemocao fundir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq, int pos_filho) {
    printf("Fundindo com irmao esquerdo [%d]...\n", irmao_esq->info1);
    Arv23 *filho_underflow = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    StatusRemocao status_pai = OK;

    // *** CORRECAO INICIO: Captura dos filhos do no em underflow ***
    // Um no em underflow (nInfo=0) antes da fusao deve ter apenas um filho valido
    // que contem a subarvore remanescente apos a remocao recursiva.
    Arv23 *filho_u_remanescente = (filho_underflow->esq != NULL) ? filho_underflow->esq : filho_underflow->cen;
    // *** CORRECAO FIM ***

    // Captura filhos do irmao esquerdo
    Arv23 *irmao_e_esq = irmao_esq->esq;
    Arv23 *irmao_e_cen = irmao_esq->cen;

    // Determina chave do pai que desce e ajusta pai, removendo o ponteiro para filho_underflow
    if (pai->nInfo == 1) {
        // Pai e 2-no, filho e central (pos_filho == 1)
        chave_pai_desce = pai->info1;
        pai->nInfo = 0; // Pai entra em underflow
        pai->cen = NULL; // Remove ponteiro central
        // irmao_esq continua em pai->esq
        status_pai = UNDERFLOW;
    } else { // Pai e 3-no
        if (pos_filho == 1) { // Filho e central (pai->cen)
            chave_pai_desce = pai->info1;
            pai->info1 = pai->info2;
            pai->info2 = 0;
            pai->cen = pai->dir; // Shift dir para cen
            pai->dir = NULL;
            pai->nInfo = 1;
            // irmao_esq continua em pai->esq
        } else { // pos_filho == 2 (Filho e direito, pai->dir)
            chave_pai_desce = pai->info2;
            pai->info2 = 0;
            pai->dir = NULL; // Remove ponteiro direito
            pai->nInfo = 1;
            // irmao_esq continua em pai->cen
        }
    }

    // Realiza fusao no irmao esquerdo (que agora sera um 3-no)
    // Chave do irmao esq (irmao_esq->info1) ja esta la
    irmao_esq->info2 = chave_pai_desce; // Chave do pai desce para info2
    irmao_esq->nInfo = 2;

    // Reorganiza os filhos do novo no [irmao_esq->info1, chave_pai_desce]
    // Filhos devem ser: irmao_e_esq, irmao_e_cen, filho_u_remanescente
    irmao_esq->esq = irmao_e_esq; // Mantem filho esquerdo original
    irmao_esq->cen = irmao_e_cen; // Mantem filho central original
    irmao_esq->dir = filho_u_remanescente; // Filho remanescente do underflow vira filho direito

    // Libera no em underflow
    printf("Liberando no em underflow (anteriormente filho na pos %d do pai)\n", pos_filho);
    free(filho_underflow);
    // *ponteiro_filho_no_pai = NULL; // CORRECAO: Esta linha estava incorreta, removia a referencia ao no fundido no pai.

    printf("Fusao com irmao esquerdo concluida. No resultante [%d, %d]. Status pai: %s\n",
           irmao_esq->info1, irmao_esq->info2, status_pai == OK ? "OK" : "UNDERFLOW");

    return status_pai;
}

// Funde o filho (underflow) com o irmao direito (2-no)
StatusRemocao fundir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir, int pos_filho) {
    printf("Fundindo com irmao direito [%d]...\n", irmao_dir->info1);
    Arv23 *filho_underflow = *ponteiro_filho_no_pai;
    int chave_pai_desce;
    StatusRemocao status_pai = OK;

    // *** CORRECAO INICIO: Captura dos filhos do no em underflow ***
    Arv23 *filho_u_remanescente = (filho_underflow->esq != NULL) ? filho_underflow->esq : filho_underflow->cen;
    // *** CORRECAO FIM ***

    // Captura filhos do irmao direito
    Arv23 *irmao_d_esq = irmao_dir->esq;
    Arv23 *irmao_d_cen = irmao_dir->cen;

    // Determina chave do pai que desce e ajusta pai, removendo o ponteiro para filho_underflow
    if (pai->nInfo == 1) {
        // Pai e 2-no, filho e esquerdo (pos_filho == 0)
        chave_pai_desce = pai->info1;
        pai->nInfo = 0; // Pai entra em underflow
        pai->esq = NULL; // Remove ponteiro esquerdo
        // irmao_dir continua em pai->cen
        status_pai = UNDERFLOW;
    } else { // Pai e 3-no
        if (pos_filho == 0) { // Filho e esquerdo (pai->esq)
            chave_pai_desce = pai->info1;
            pai->info1 = pai->info2;
            pai->info2 = 0;
            pai->esq = pai->cen; // Shift cen para esq
            pai->cen = pai->dir; // Shift dir para cen
            pai->dir = NULL;
            pai->nInfo = 1;
            // irmao_dir agora esta em pai->cen
        } else { // pos_filho == 1 (Filho e central, pai->cen)
            chave_pai_desce = pai->info2;
            pai->info2 = 0;
            // pai->esq continua o mesmo
            pai->cen = pai->dir; // Shift dir para cen
            pai->dir = NULL;
            pai->nInfo = 1;
            // irmao_dir agora esta em pai->cen
        }
    }

    // Realiza fusao no irmao direito (que agora sera um 3-no)
    // Move a chave atual do irmao direito para info2
    irmao_dir->info2 = irmao_dir->info1;
    // A chave que desce do pai vira info1
    irmao_dir->info1 = chave_pai_desce;
    irmao_dir->nInfo = 2;

    // Reorganiza os filhos corretamente para o novo no [chave_pai_desce, irmao_dir->info2]
    // Filhos devem ser: filho_u_remanescente, irmao_d_esq, irmao_d_cen
    irmao_dir->esq = filho_u_remanescente; // Filho remanescente do underflow vira esquerdo
    irmao_dir->cen = irmao_d_esq;          // Filho esquerdo original do irmao_dir vira central
    irmao_dir->dir = irmao_d_cen;          // Filho central original do irmao_dir vira direito

    // Libera no em underflow
    printf("Liberando no em underflow (anteriormente filho na pos %d do pai)\n", pos_filho);
    free(filho_underflow);
    // *ponteiro_filho_no_pai = NULL; // CORRECAO: Esta linha estava incorreta, removia a referencia ao no fundido no pai.

    printf("Fusao com irmao direito concluida. No resultante [%d, %d]. Status pai: %s\n",
           irmao_dir->info1, irmao_dir->info2, status_pai == OK ? "OK" : "UNDERFLOW");

    return status_pai;
}

//================ REMOCAO (Recursiva) ==================
// ponteiro_no_pai: Ponteiro para o ponteiro que aponta para o no atual (e.g., &pai->esq, &pai->cen, &raiz)
// valor: Valor a ser removido
// pai: Ponteiro para o no pai do no atual (NULL se no atual for raiz)
StatusRemocao remover_23_recursivo(Arv23 **ponteiro_no_atual, int valor, Arv23 *pai) {
    StatusRemocao status = OK;
    Arv23 *no_atual = *ponteiro_no_atual;

    if (no_atual == NULL) {
        printf("Valor %d nao encontrado na subarvore.\n", valor);
        return OK; // Valor nao existe, remocao bem-sucedida (nao fazer nada)
    }

    printf("Visitando no [%d%s%d] para remover %d\n", no_atual->info1,
           no_atual->nInfo==2 ? ", " : " ", no_atual->nInfo==2 ? no_atual->info2 : -1, valor);

    // Determina para qual filho descer
    Arv23 **proximo_ponteiro = NULL;
    int valor_encontrado = 0;

    if (valor == no_atual->info1 || (no_atual->nInfo == 2 && valor == no_atual->info2)) {
        valor_encontrado = 1;
    } else if (valor < no_atual->info1) {
        proximo_ponteiro = &(no_atual->esq);
    } else if (no_atual->nInfo == 1 || valor < no_atual->info2) { // Se nInfo==1, vai pro centro. Se nInfo==2 e valor < info2, vai pro centro.
        proximo_ponteiro = &(no_atual->cen);
    } else { // nInfo == 2 e valor > info2
        proximo_ponteiro = &(no_atual->dir);
    }

    if (valor_encontrado) {
        printf("Valor %d encontrado no no [%d%s%d]\n", valor, no_atual->info1,
               no_atual->nInfo==2 ? ", " : " ", no_atual->nInfo==2 ? no_atual->info2 : -1);

        if (eh_folha(no_atual)) {
            // Caso 1: Remocao em no folha
            if (no_atual->nInfo == 2) {
                // Subcaso 1.1: Folha com 2 chaves (3-no)
                printf("Removendo de folha 3-no...\n");
                if (valor == no_atual->info1) {
                    no_atual->info1 = no_atual->info2;
                }
                no_atual->info2 = 0;
                no_atual->nInfo = 1;
                status = OK;
            } else {
                // Subcaso 1.2: Folha com 1 chave (2-no)
                printf("Removendo de folha 2-no... Causa UNDERFLOW\n");
                no_atual->nInfo = 0; // Marca para underflow
                // Nao libera o no aqui, o tratamento de underflow fara isso se necessario (fusao)
                status = UNDERFLOW;
            }
        } else {
            // Caso 2: Remocao em no interno
            printf("Removendo de no interno... substituindo por sucessor\n");
            Arv23 *sucessor_node = NULL;
            int valor_sucessor;
            Arv23 **ponteiro_subarvore_sucessor;

            // Encontra o sucessor in-order
            if (valor == no_atual->info1) {
                // Sucessor esta na subarvore central
                sucessor_node = buscar_menor_elemento(no_atual->cen);
                ponteiro_subarvore_sucessor = &(no_atual->cen);
            } else { // valor == no_atual->info2
                // Sucessor esta na subarvore direita
                sucessor_node = buscar_menor_elemento(no_atual->dir);
                ponteiro_subarvore_sucessor = &(no_atual->dir);
            }

            if (sucessor_node == NULL) {
                fprintf(stderr, "Erro critico: Sucessor nao encontrado para valor %d!\n", valor);
                return OK; // Evita crash, mas indica erro
            }

            valor_sucessor = sucessor_node->info1; // Sucessor sempre e info1 do no mais a esquerda
            printf("Sucessor encontrado: %d. Substituindo %d por %d.\n", valor_sucessor, valor, valor_sucessor);

            // Substitui o valor no no atual pelo sucessor
            if (valor == no_atual->info1) {
                no_atual->info1 = valor_sucessor;
            } else {
                no_atual->info2 = valor_sucessor;
            }

            // Remove recursivamente o sucessor de sua posicao original
            printf("Removendo recursivamente o sucessor %d da subarvore apropriada...\n", valor_sucessor);
            status = remover_23_recursivo(ponteiro_subarvore_sucessor, valor_sucessor, no_atual);

            // Se a remocao do sucessor causou underflow, trata aqui
            if (status == UNDERFLOW) {
                printf("Underflow apos remover sucessor %d. Tratando na subarvore...\n", valor_sucessor);
                status = tratar_underflow(ponteiro_subarvore_sucessor, no_atual);
            }
        }
    } else {
        // Desce para a subarvore apropriada
        status = remover_23_recursivo(proximo_ponteiro, valor, no_atual);

        // Se a chamada recursiva retornou underflow, trata aqui
        if (status == UNDERFLOW) {
            printf("Underflow retornado da subarvore. Tratando no nivel atual (pai=[%d%s%d])...\n",
                   no_atual->info1, no_atual->nInfo==2 ? ", " : " ", no_atual->nInfo==2 ? no_atual->info2 : -1);
            status = tratar_underflow(proximo_ponteiro, no_atual);
        }
    }

    return status;
}

//================ REMOCAO (Principal) ==================
int remover_23(Arv23 **raiz, int valor) {
    if (raiz == NULL || *raiz == NULL) {
        printf("Arvore vazia. Nao e possivel remover.\n");
        return 0;
    }

    printf("\n--- Iniciando remocao de %d ---\n", valor);
    StatusRemocao status = remover_23_recursivo(raiz, valor, NULL);

    // Tratamento especial se a raiz entrou em underflow
    // Isso acontece quando a raiz original era um 2-no e seu unico filho
    // precisou ser fundido com outro (inexistente), ou quando a raiz
    // era um 2-no e a remocao em seu filho causou uma fusao que esvaziou a raiz.
    if (status == UNDERFLOW && *raiz != NULL && (*raiz)->nInfo == 0) {
        printf("Raiz entrou em underflow (nInfo=0). Ajustando a raiz da arvore...\n");
        Arv23 *raiz_antiga = *raiz;

        // A nova raiz e o unico filho remanescente da raiz antiga apos a fusao.
        // Este filho foi movido para esq ou cen pela logica de fusao.
        if (raiz_antiga->esq != NULL) {
            *raiz = raiz_antiga->esq;
            printf("Nova raiz e o filho esquerdo da antiga raiz.\n");
        } else if (raiz_antiga->cen != NULL) {
            *raiz = raiz_antiga->cen;
            printf("Nova raiz e o filho central da antiga raiz.\n");
        } else {
            // Se a raiz era um 2-no folha e foi removida, a arvore fica vazia.
            *raiz = NULL;
            printf("Arvore ficou vazia apos remocao da raiz.\n");
        }

        printf("Liberando no da raiz antiga em underflow.\n");
        free(raiz_antiga);
        status = OK; // O underflow da raiz foi resolvido
    }

    if (status == OK) {
         printf("--- Remocao de %d concluida com sucesso ---\n", valor);
         return 1;
    } else {
         // Se status ainda for UNDERFLOW aqui, algo deu muito errado
         fprintf(stderr, "--- Erro critico: Underflow nao resolvido na raiz ao remover %d ---\n", valor);
         return 0;
    }
}

//================ INSERCAO (Mantida do original) ==================
void adiciona_infos(Arv23 **no, int info, Arv23 *Sub_Arv_Info)
{
    // Assume que no aponta para um 2-no (*no)->nInfo == 1
    if (info > (*no)->info1)
    {
        (*no)->info2 = info;
        (*no)->dir = Sub_Arv_Info;
    }
    else
    {
        (*no)->info2 = (*no)->info1;
        (*no)->info1 = info;
        (*no)->dir = (*no)->cen; // Antigo filho central vira direito
        (*no)->cen = Sub_Arv_Info; // Novo filho vira central
    }
    (*no)->nInfo = 2;
}

Arv23 *quebra_no(Arv23 **no, int info, int *sobe, Arv23 *F_dir)
{
    // Assume que no aponta para um 3-no temporario (apos adicao)
    Arv23 *maior;
    if (info > (*no)->info2) // Info e o maior dos 3
    {
        *sobe = (*no)->info2; // Chave do meio sobe
        maior = cria_no(info, (*no)->dir, F_dir); // Cria novo no com a maior chave
        (*no)->nInfo = 1; // No original mantem a menor chave
        (*no)->info2 = 0;
        (*no)->dir = NULL;
    }
    else if (info > (*no)->info1) // Info e a chave do meio
    {
        *sobe = info; // Info sobe
        maior = cria_no((*no)->info2, F_dir, (*no)->dir); // Cria novo no com a maior chave original
        (*no)->nInfo = 1; // No original mantem a menor chave
        (*no)->info2 = 0;
        (*no)->dir = NULL;
    }
    else // Info e o menor dos 3
    {
        *sobe = (*no)->info1; // Chave do meio original sobe
        maior = cria_no((*no)->info2, (*no)->cen, (*no)->dir); // Cria novo no com a maior chave original
        (*no)->info1 = info; // No original mantem a menor chave (nova)
        (*no)->cen = F_dir; // Atualiza filho central
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
        // Caso base 1: Arvore/Subarvore vazia, cria folha
        *raiz = cria_no(valor, NULL, NULL);
        *maiorNo = NULL; // Nao ha no dividido para subir
        *sobe = 0; // Nada sobe
        retorno = 1;
    }
    else if (valor != (*raiz)->info1 && ((*raiz)->nInfo != 2 || valor != (*raiz)->info2))
    {
        // Valor nao existe no no atual
        if (eh_folha(*raiz))
        {
            // Caso base 2: Insercao em no folha
            if ((*raiz)->nInfo == 1)
            {
                // Subcaso 2.1: Folha 2-no tem espaco
                adiciona_infos(raiz, valor, NULL);
                *maiorNo = NULL;
                *sobe = 0;
                retorno = 1;
            }
            else
            {
                // Subcaso 2.2: Folha 3-no esta cheia, precisa quebrar
                *maiorNo = quebra_no(raiz, valor, sobe, NULL);
                // 'sobe' contem a chave que subiu, 'maiorNo' o novo no a direita
                // A propagacao (insercao de 'sobe' no pai) acontece no retorno da recursao
                retorno = 1;
            }
        }
        else
        {
            // Caso recursivo: Descer para a subarvore correta
            Arv23 **proximo_filho;
            if (valor < (*raiz)->info1)
            {
                proximo_filho = &(*raiz)->esq;
            }
            else if (((*raiz)->nInfo == 1) || (valor < (*raiz)->info2))
            {
                proximo_filho = &(*raiz)->cen;
            }
            else
            {
                proximo_filho = &(*raiz)->dir;
            }
            retorno = insere_23_recursivo(proximo_filho, valor, *raiz, sobe, maiorNo);

            // Se a chamada recursiva resultou em um no quebrando (maiorNo != NULL)
            if (*maiorNo != NULL)
            {
                // Tenta inserir a chave 'sobe' e o 'maiorNo' no no atual (*raiz)
                int chave_que_subiu = *sobe;
                Arv23 *novo_filho_dir = *maiorNo;
                *maiorNo = NULL; // Reseta para a proxima iteracao
                *sobe = 0;

                if ((*raiz)->nInfo == 1)
                {
                    // No atual tem espaco
                    adiciona_infos(raiz, chave_que_subiu, novo_filho_dir);
                    // Nao ha mais nada para propagar
                }
                else
                {
                    // No atual esta cheio, precisa quebrar tambem
                    *maiorNo = quebra_no(raiz, chave_que_subiu, sobe, novo_filho_dir);
                    // 'sobe' e 'maiorNo' serao propagados para o nivel acima no retorno
                }
            }
        }
    }
    else
    {
        printf("Valor %d ja existe na arvore!\n", valor);
        *maiorNo = NULL;
        *sobe = 0;
        retorno = 0; // Falha na insercao (duplicado)
    }
    return retorno;
}

int insere_23(Arv23 **raiz, int valor)
{
    int sobe = 0;
    Arv23 *maiorNo = NULL;
    int sucesso = insere_23_recursivo(raiz, valor, NULL, &sobe, &maiorNo);

    // Se maiorNo nao for NULL apos a chamada recursiva inicial, significa que a raiz original quebrou
    if (maiorNo != NULL)
    {
        // Cria uma nova raiz
        printf("Raiz original quebrou. Criando nova raiz com valor %d.\n", sobe);
        Arv23 *nova_raiz = cria_no(sobe, *raiz, maiorNo);
        *raiz = nova_raiz;
    }
    return sucesso;
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
        // Conector: raiz nao tem, outros tem '+---' ou '`---'
        if (!eh_raiz) {
             printf("%s", eh_ultimo ? "`------ " : "+------ ");
        } else {
             printf(" Raiz--> "); // Indica a raiz
        }

        // Imprime chaves do no
        if (raiz->nInfo == 1)
        {
            printf("[%d]\n", raiz->info1);
        } else if (raiz->nInfo == 2) {
            printf("[%d, %d]\n", raiz->info1, raiz->info2);
        } else {
            printf("[UNDERFLOW!]\n"); // Indica no em estado invalido (exceto transiente)
        }

        // Prepara prefixo para os filhos
        char novo_prefixo[1024];
        // Se for raiz, nao adiciona espaco vertical. Se nao for raiz, adiciona '|   ' ou '    '
        sprintf(novo_prefixo, "%s%s", prefixo, eh_raiz? "         " : (eh_ultimo ? "         " : "|        "));

        // Conta filhos para determinar qual e o ultimo
        int num_filhos = 0;
        if (raiz->esq) num_filhos++;
        if (raiz->cen) num_filhos++;
        if (raiz->dir) num_filhos++;

        int filhos_impressos = 0;
        // Imprime filhos recursivamente
        if (raiz->esq != NULL) {
            filhos_impressos++;
            imprime_arvore_visual(raiz->esq, novo_prefixo, filhos_impressos == num_filhos, 0);
        }
        if (raiz->cen != NULL) {
            filhos_impressos++;
            imprime_arvore_visual(raiz->cen, novo_prefixo, filhos_impressos == num_filhos, 0);
        }
        if (raiz->dir != NULL) {
            filhos_impressos++;
            imprime_arvore_visual(raiz->dir, novo_prefixo, filhos_impressos == num_filhos, 0);
        }
    }
}

//================ MAIN ==================
int main()
{
    Arv23 *raiz = NULL;
    int opcao, valor;
    char prefixo_inicial[10] = "";

    // Insercao inicial para teste (1 a 30)
    printf("Inserindo valores (1-30)...\n");
    for (int i = 1; i <= 30; i++){
        // printf("Inserindo %d...\n", i);
        insere_23(&raiz, i);
        // Descomente para ver a arvore a cada passo:
        // printf("Arvore apos inserir %d:\n", i);
        // imprime_arvore_visual(raiz, prefixo_inicial, 1, 1);
        // printf("------------------------\n");
    }
    printf("Insercao (1-30) concluida.\n");

    do
    {
        printf("\n=========================\n");
        printf("Arvore 2-3 Atual:\n");
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
             // Limpa buffer de entrada
             while (getchar() != '\n');
             opcao = 0; // Forca repeticao do loop
             continue;
        }
        // Limpa buffer de entrada apos scanf bem-sucedido
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

            // Verifica se a arvore nao esta vazia antes de remover
            if (raiz == NULL) {
                printf("Arvore esta vazia, nao e possivel remover.\n");
            } else {
                if (!remover_23(&raiz, valor))
                {
                    // A funcao remover_23 ja imprime mensagens de erro/nao encontrado
                    // printf("Falha ao remover %d (nao encontrado ou erro).\n", valor);
                }
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

    // Liberar memoria da arvore (implementacao opcional, mas recomendada)
    // libera_arvore(raiz);

    return 0;
}

// Funcao para liberar a memoria (exemplo)
/*
void libera_arvore(Arv23 *raiz) {
    if (raiz != NULL) {
        libera_arvore(raiz->esq);
        libera_arvore(raiz->cen);
        libera_arvore(raiz->dir);
        free(raiz);
    }
}
*/

