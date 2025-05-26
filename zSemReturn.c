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

// Prototipos das Funcoes (Mantidos consistentes com as definicoes)
Arv23 *cria_no(int info, Arv23 *F_esq, Arv23 *F_cen);
int eh_folha(Arv23 *no);
void adiciona_infos(Arv23 **no, int info, Arv23 *Sub_Arv_Info);
Arv23 *quebra_no(Arv23 **no, int info, int *sobe, Arv23 *F_dir);
int insere_23_recursivo(Arv23 **raiz, int valor, Arv23 *pai, int *sobe, Arv23 **maiorNo);
int insere_23(Arv23 **raiz, int valor);
void imprime_23_em_ordem(Arv23 *raiz);
void imprime_arvore_visual(Arv23 *raiz, char *prefixo, int eh_ultimo, int eh_raiz);
Arv23* buscar_menor_elemento(Arv23 *no);
StatusRemocao remover_23_recursivo(Arv23 **ponteiro_no_atual, int valor, Arv23 *pai);
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

//================ VERIFICA FOLHA (Refatorado: Unico Return) ==================
int eh_folha(Arv23 *no)
{
    int resultado = -1; // Valor padrao se no for NULL
    if (no != NULL)
    {
        resultado = (no->esq == NULL && no->cen == NULL && no->dir == NULL);
    }
    return resultado;
}

//================ BUSCAR MENOR (Refatorado: Unico Return) ==================
Arv23* buscar_menor_elemento(Arv23 *no) {
    Arv23 *resultado = NULL; // Valor padrao se no for NULL
    if (no != NULL) {
        Arv23 *atual = no;
        while (atual != NULL && atual->esq != NULL) {
            atual = atual->esq;
        }
        resultado = atual;
    }
    return resultado;
}

//================ FUNCOES AUXILIARES REMOCAO ==================

// Funcao principal para tratar underflow (Refatorado: Unico Return)
StatusRemocao tratar_underflow(Arv23 **ponteiro_filho_no_pai, Arv23 *pai) {
    StatusRemocao status_final = OK;
    Arv23 *filho_com_underflow = *ponteiro_filho_no_pai;

    if (filho_com_underflow == NULL || filho_com_underflow->nInfo > 0) {
        status_final = OK;
    }
    else if (pai == NULL) {
        status_final = UNDERFLOW;
    }
    else {
        printf("Tratando underflow no filho (nInfo=0) do pai [%d%s%d]...\n",
               pai->info1, pai->nInfo==2 ? ", " : " ", pai->nInfo==2 ? pai->info2 : -1);

        Arv23 *irmao_esq = NULL;
        Arv23 *irmao_dir = NULL;
        int pos_filho = -1;

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

        if (irmao_dir != NULL && irmao_dir->nInfo == 2) {
            printf("Tentando redistribuicao com irmao direito [%d, %d]\n", irmao_dir->info1, irmao_dir->info2);
            status_final = redistribuir_com_irmao_direito(ponteiro_filho_no_pai, pai, irmao_dir, pos_filho);
        }
        else if (irmao_esq != NULL && irmao_esq->nInfo == 2) {
            printf("Tentando redistribuicao com irmao esquerdo [%d, %d]\n", irmao_esq->info1, irmao_esq->info2);
            status_final = redistribuir_com_irmao_esquerdo(ponteiro_filho_no_pai, pai, irmao_esq, pos_filho);
        }
        else if (irmao_dir != NULL && irmao_dir->nInfo == 1) {
            printf("Tentando fusao com irmao direito [%d]\n", irmao_dir->info1);
            status_final = fundir_com_irmao_direito(ponteiro_filho_no_pai, pai, irmao_dir, pos_filho);
        }
        else if (irmao_esq != NULL && irmao_esq->nInfo == 1) {
            printf("Tentando fusao com irmao esquerdo [%d]\n", irmao_esq->info1);
            status_final = fundir_com_irmao_esquerdo(ponteiro_filho_no_pai, pai, irmao_esq, pos_filho);
        }
        else {
            fprintf(stderr, "Erro critico: Nao foi possivel tratar underflow. Pai: [%d%s%d], Filho pos: %d\n",
                    pai->info1, pai->nInfo==2 ? ", " : " ", pai->nInfo==2 ? pai->info2 : -1, pos_filho);
            status_final = OK;
        }
    }

    return status_final;
}

// Redistribui do irmao esquerdo (3-no) para o filho (underflow) - CORRIGIDO
StatusRemocao redistribuir_com_irmao_esquerdo(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_esq, int pos_filho) {
    printf("Redistribuindo com irmao esquerdo...\n");
    Arv23 *filho = *ponteiro_filho_no_pai; // Node in underflow
    int chave_pai_desce;
    Arv23 *filho_transferido = irmao_esq->dir; // Child coming from sibling

    // Salvar filho esquerdo original do nó em underflow (pode ser resultado de fusao anterior)
    Arv23 *filho_original_esq = filho->esq;

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
    // Ajustar filhos do nó restaurado
    filho->esq = filho_transferido;       // Filho transferido vira esquerdo
    filho->cen = filho_original_esq;      // Filho esquerdo original vira central
    filho->dir = NULL;                    // Garante que nao ha filho direito

    // Ajusta o irmao esquerdo (era 3-no, vira 2-no)
    irmao_esq->nInfo = 1;
    irmao_esq->info2 = 0;
    irmao_esq->dir = irmao_esq->cen; // O filho central antigo vira o direito
    irmao_esq->cen = NULL; // Nao tem mais filho central

    return OK;
}

// Redistribui do irmao direito (3-no) para o filho (underflow) - CORRIGIDO
StatusRemocao redistribuir_com_irmao_direito(Arv23 **ponteiro_filho_no_pai, Arv23 *pai, Arv23 *irmao_dir, int pos_filho) {
    printf("Redistribuindo com irmao direito...\n");
    Arv23 *filho = *ponteiro_filho_no_pai; // Node in underflow
    int chave_pai_desce;
    Arv23 *filho_transferido = irmao_dir->esq; // Child coming from sibling

    // Salvar filho esquerdo original do nó em underflow (pode ser resultado de fusao anterior)
    Arv23 *filho_original_esq = filho->esq;

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
    // Ajustar filhos do nó restaurado
    filho->esq = filho_original_esq; // Mantem o filho esquerdo original
    filho->cen = filho_transferido;  // Filho transferido vira central
    filho->dir = NULL;               // Garante que nao ha filho direito

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

    Arv23 *filho_u_remanescente = (filho_underflow->esq != NULL) ? filho_underflow->esq : filho_underflow->cen;
    Arv23 *irmao_e_esq = irmao_esq->esq;
    Arv23 *irmao_e_cen = irmao_esq->cen;

    if (pai->nInfo == 1) {
        chave_pai_desce = pai->info1;
        pai->nInfo = 0;
        pai->cen = NULL;
        status_pai = UNDERFLOW;
    } else {
        if (pos_filho == 1) {
            chave_pai_desce = pai->info1;
            pai->info1 = pai->info2;
            pai->info2 = 0;
            pai->cen = pai->dir;
            pai->dir = NULL;
            pai->nInfo = 1;
        } else {
            chave_pai_desce = pai->info2;
            pai->info2 = 0;
            pai->dir = NULL;
            pai->nInfo = 1;
        }
        status_pai = OK;
    }

    irmao_esq->info2 = chave_pai_desce;
    irmao_esq->nInfo = 2;
    irmao_esq->esq = irmao_e_esq;
    irmao_esq->cen = irmao_e_cen;
    irmao_esq->dir = filho_u_remanescente;

    printf("Liberando no em underflow (anteriormente filho na pos %d do pai)\n", pos_filho);
    free(filho_underflow);

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

    Arv23 *filho_u_remanescente = (filho_underflow->esq != NULL) ? filho_underflow->esq : filho_underflow->cen;
    Arv23 *irmao_d_esq = irmao_dir->esq;
    Arv23 *irmao_d_cen = irmao_dir->cen;

    if (pai->nInfo == 1) {
        chave_pai_desce = pai->info1;
        pai->nInfo = 0;
        pai->esq = NULL;
        status_pai = UNDERFLOW;
    } else {
        if (pos_filho == 0) {
            chave_pai_desce = pai->info1;
            pai->info1 = pai->info2;
            pai->info2 = 0;
            pai->esq = pai->cen;
            pai->cen = pai->dir;
            pai->dir = NULL;
            pai->nInfo = 1;
        } else {
            chave_pai_desce = pai->info2;
            pai->info2 = 0;
            pai->cen = pai->dir;
            pai->dir = NULL;
            pai->nInfo = 1;
        }
         status_pai = OK;
    }

    irmao_dir->info2 = irmao_dir->info1;
    irmao_dir->info1 = chave_pai_desce;
    irmao_dir->nInfo = 2;
    irmao_dir->esq = filho_u_remanescente;
    irmao_dir->cen = irmao_d_esq;
    irmao_dir->dir = irmao_d_cen;

    printf("Liberando no em underflow (anteriormente filho na pos %d do pai)\n", pos_filho);
    free(filho_underflow);

    printf("Fusao com irmao direito concluida. No resultante [%d, %d]. Status pai: %s\n",
           irmao_dir->info1, irmao_dir->info2, status_pai == OK ? "OK" : "UNDERFLOW");

    return status_pai;
}

//================ REMOCAO (Recursiva - Refatorado: Unico Return, Warning tratado) ==================
StatusRemocao remover_23_recursivo(Arv23 **ponteiro_no_atual, int valor, Arv23 *pai) {
    (void)pai; // Silencia warning de parametro nao utilizado
    StatusRemocao status_final = OK;
    Arv23 *no_atual = *ponteiro_no_atual;

    if (no_atual == NULL) {
        printf("Valor %d nao encontrado na subarvore.\n", valor);
        status_final = OK;
    }
    else {
        printf("Visitando no [%d%s%d] para remover %d\n", no_atual->info1,
               no_atual->nInfo==2 ? ", " : " ", no_atual->nInfo==2 ? no_atual->info2 : -1, valor);

        Arv23 **proximo_ponteiro = NULL;
        int valor_encontrado = 0;

        if (valor == no_atual->info1 || (no_atual->nInfo == 2 && valor == no_atual->info2)) {
            valor_encontrado = 1;
        } else if (valor < no_atual->info1) {
            proximo_ponteiro = &(no_atual->esq);
        } else if (no_atual->nInfo == 1 || valor < no_atual->info2) {
            proximo_ponteiro = &(no_atual->cen);
        } else {
            proximo_ponteiro = &(no_atual->dir);
        }

        if (valor_encontrado) {
            printf("Valor %d encontrado no no [%d%s%d]\n", valor, no_atual->info1,
                   no_atual->nInfo==2 ? ", " : " ", no_atual->nInfo==2 ? no_atual->info2 : -1);

            if (eh_folha(no_atual)) {
                if (no_atual->nInfo == 2) {
                    printf("Removendo de folha 3-no...\n");
                    if (valor == no_atual->info1) {
                        no_atual->info1 = no_atual->info2;
                    }
                    no_atual->info2 = 0;
                    no_atual->nInfo = 1;
                    status_final = OK;
                } else {
                    printf("Removendo de folha 2-no... Causa UNDERFLOW\n");
                    no_atual->nInfo = 0;
                    status_final = UNDERFLOW;
                }
            }
            else {
                printf("Removendo de no interno... substituindo por sucessor\n");
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
                    fprintf(stderr, "Erro critico: Sucessor nao encontrado para valor %d!\n", valor);
                    status_final = OK;
                } else {
                    valor_sucessor = sucessor_node->info1;
                    printf("Sucessor encontrado: %d. Substituindo %d por %d.\n", valor_sucessor, valor, valor_sucessor);

                    if (valor == no_atual->info1) {
                        no_atual->info1 = valor_sucessor;
                    } else {
                        no_atual->info2 = valor_sucessor;
                    }

                    printf("Removendo recursivamente o sucessor %d da subarvore apropriada...\n", valor_sucessor);
                    StatusRemocao status_rec = remover_23_recursivo(ponteiro_subarvore_sucessor, valor_sucessor, no_atual);

                    if (status_rec == UNDERFLOW) {
                        printf("Underflow apos remover sucessor %d. Tratando na subarvore...\n", valor_sucessor);
                        status_final = tratar_underflow(ponteiro_subarvore_sucessor, no_atual);
                    } else {
                        status_final = status_rec;
                    }
                }
            }
        }
        else {
            StatusRemocao status_rec = remover_23_recursivo(proximo_ponteiro, valor, no_atual);

            if (status_rec == UNDERFLOW) {
                printf("Underflow retornado da subarvore. Tratando no nivel atual (pai=[%d%s%d])...\n",
                       no_atual->info1, no_atual->nInfo==2 ? ", " : " ", no_atual->nInfo==2 ? no_atual->info2 : -1);
                status_final = tratar_underflow(proximo_ponteiro, no_atual);
            }
            else {
                status_final = status_rec;
            }
        }
    }

    return status_final;
}

//================ REMOCAO (Principal - Refatorado: Unico Return) ==================
int remover_23(Arv23 **raiz, int valor) {
    int sucesso = 0;

    if (raiz == NULL || *raiz == NULL) {
        printf("Arvore vazia. Nao e possivel remover.\n");
        sucesso = 0;
    }
    else {
        printf("\n--- Iniciando remocao de %d ---\n", valor);
        StatusRemocao status = remover_23_recursivo(raiz, valor, NULL);

        if (status == UNDERFLOW && *raiz != NULL && (*raiz)->nInfo == 0) {
            printf("Raiz entrou em underflow (nInfo=0). Ajustando a raiz da arvore...\n");
            Arv23 *raiz_antiga = *raiz;

            if (raiz_antiga->esq != NULL) {
                *raiz = raiz_antiga->esq;
                printf("Nova raiz e o filho esquerdo da antiga raiz.\n");
            } else if (raiz_antiga->cen != NULL) {
                *raiz = raiz_antiga->cen;
                printf("Nova raiz e o filho central da antiga raiz.\n");
            } else {
                *raiz = NULL;
                printf("Arvore ficou vazia apos remocao da raiz.\n");
            }

            printf("Liberando no da raiz antiga em underflow.\n");
            free(raiz_antiga);
            status = OK;
        }

        if (status == OK) {
             printf("--- Remocao de %d concluida com sucesso ---\n", valor);
             sucesso = 1;
        } else {
             fprintf(stderr, "--- Erro critico: Underflow nao resolvido na raiz ao remover %d ---\n", valor);
             sucesso = 0;
        }
    }

    return sucesso;
}

//================ INSERCAO ==================

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

int insere_23_recursivo(Arv23 **raiz, int valor, Arv23 *pai, int *sobe, Arv23 **maiorNo) {
    (void)pai;
    int sucesso = 0;

    if (*raiz == NULL) {
        *raiz = cria_no(valor, NULL, NULL);
        *maiorNo = NULL;
        *sobe = 0;
        sucesso = 1;
    }
    else if (valor != (*raiz)->info1 && ((*raiz)->nInfo != 2 || valor != (*raiz)->info2)) {
        if (eh_folha(*raiz)) {
            if ((*raiz)->nInfo == 1) {
                adiciona_infos(raiz, valor, NULL);
                *maiorNo = NULL;
                *sobe = 0;
                sucesso = 1;
            }
            else {
                *maiorNo = quebra_no(raiz, valor, sobe, NULL);
                sucesso = 1;
            }
        }
        else {
            Arv23 **proximo_filho;
            if (valor < (*raiz)->info1) {
                proximo_filho = &(*raiz)->esq;
            }
            else if (((*raiz)->nInfo == 1) || (valor < (*raiz)->info2)) {
                proximo_filho = &(*raiz)->cen;
            }
            else {
                proximo_filho = &(*raiz)->dir;
            }
            int sucesso_rec = insere_23_recursivo(proximo_filho, valor, *raiz, sobe, maiorNo);

            if (*maiorNo != NULL) {
                int chave_que_subiu = *sobe;
                Arv23 *novo_filho_dir = *maiorNo;
                *maiorNo = NULL;
                *sobe = 0;

                if ((*raiz)->nInfo == 1) {
                    adiciona_infos(raiz, chave_que_subiu, novo_filho_dir);
                    sucesso = 1;
                }
                else {
                    *maiorNo = quebra_no(raiz, chave_que_subiu, sobe, novo_filho_dir);
                    sucesso = 1;
                }
            }
            else {
                 sucesso = sucesso_rec;
            }
        }
    }
    else {
        printf("Valor %d ja existe na arvore!\n", valor);
        *maiorNo = NULL;
        *sobe = 0;
        sucesso = 0;
    }
    return sucesso;
}

int insere_23(Arv23 **raiz, int valor)
{
    int sobe = 0;
    Arv23 *maiorNo = NULL;
    int sucesso = insere_23_recursivo(raiz, valor, NULL, &sobe, &maiorNo);

    if (maiorNo != NULL)
    {
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
        if (!eh_raiz) {
             printf("%s", eh_ultimo ? "`------ " : "+------ ");
        } else {
             printf(" Raiz--> ");
        }

        if (raiz->nInfo == 1)
        {
            printf("[%d]\n", raiz->info1);
        } else if (raiz->nInfo == 2) {
            printf("[%d, %d]\n", raiz->info1, raiz->info2);
        } else {
            printf("[UNDERFLOW!]\n");
        }

        char novo_prefixo[1024];
        sprintf(novo_prefixo, "%s%s", prefixo, eh_raiz? "         " : (eh_ultimo ? "         " : "|        "));

        int num_filhos = 0;
        if (raiz->esq) num_filhos++;
        if (raiz->cen) num_filhos++;
        if (raiz->dir) num_filhos++;

        int filhos_impressos = 0;
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
    int opcao=0, valor;
    char prefixo_inicial[10] = "";

    printf("Inserindo valores (1-30)...\n");
    for (int i = 1; i <= 30; i++){
        insere_23(&raiz, i);
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
             }
             else {
                 while (getchar() != '\n');
                 if (insere_23(&raiz, valor))
                 {
                     printf("Valor %d inserido com sucesso.\n", valor);
                 }
             }
            break;

        case 2:
            printf("Digite o valor a ser removido: ");
             if (scanf("%d", &valor) != 1) {
                 printf("Entrada invalida.\n");
                 while (getchar() != '\n');
             }
             else {
                 while (getchar() != '\n');
                 if (raiz == NULL) {
                     printf("Arvore esta vazia, nao e possivel remover.\n");
                 } else {
                     if (!remover_23(&raiz, valor))
                     {
                         // Mensagem de erro ja impressa por remover_23
                     }
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

    // libera_arvore(raiz); // Funcao nao implementada

    return 0;
}


