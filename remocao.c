// Função removida - verificação será feita diretamente no insere_23

#include <stdio.h>
#include <stdlib.h>

// --- Estrutura Básica do Nó ---

typedef struct No23
{
    int chaves[2];          // Armazena até 2 chaves
    int num_chaves;         // Número de chaves atualmente no nó (1 ou 2)
    struct No23 *filhos[3]; // Ponteiros para até 3 filhos
    struct No23 *pai;       // Ponteiro para o nó pai (útil na remoção)
    int eh_folha;           // Flag para indicar se é um nó folha
} No23;

// --- Funções Auxiliares (Protótipos) ---

No23 *criar_no(int chave1, int eh_folha);
No23 *buscar_no(No23 *raiz, int chave);
No23 *encontrar_sucessor_no_folha(No23 *no, int chave_idx); // Encontra sucessor para nó interno
void remover_chave_folha_simples(No23 *no, int chave);
void tratar_underflow(No23 *no); // Chama redistribuição ou fusão
void redistribuir(No23 *no_vazio, No23 *irmao, No23 *pai, int idx_no_pai);
void fundir(No23 *no_vazio, No23 *irmao, No23 *pai, int idx_no_pai);
No23 *remover_recursivo(No23 *no, int chave, int *underflow_propagado);

// --- Remoção em Nó Folha (Caso Simples) ---

// Remove uma chave de um nó folha que tem 2 chaves
void remover_chave_folha_simples(No23 *no, int chave)
{
    if (!no || !no->eh_folha || no->num_chaves != 2)
    {
        // Erro ou caso não aplicável
        return;
    }

    if (no->chaves[0] == chave)
    {
        no->chaves[0] = no->chaves[1]; // Move a segunda chave para a primeira posição
    }
    // Se for a segunda chave (no->chaves[1] == chave), ela é simplesmente "esquecida"

    no->num_chaves = 1;
    // Não precisa limpar chaves[1] ou filhos, pois num_chaves controla o acesso
}

// --- Remoção em Nó Interno (Redução à Folha) ---

// Função principal de remoção (inicia a busca e trata o caso interno)
No23 *remover(No23 *raiz, int chave)
{
    int underflow_ocorreu = 0;
    raiz = remover_recursivo(raiz, chave, &underflow_ocorreu);

    // Se a raiz ficou vazia após uma fusão no nível abaixo dela
    if (raiz && raiz->num_chaves == 0)
    {
        No23 *antiga_raiz = raiz;
        raiz = raiz->filhos[0]; // O nó fundido se torna a nova raiz
        if (raiz)
            raiz->pai = NULL;
        free(antiga_raiz);
    }
    return raiz;
}

// Função recursiva que desce na árvore e realiza a remoção
No23 *remover_recursivo(No23 *no, int chave, int *underflow_propagado)
{
    if (!no)
    {
        *underflow_propagado = 0; // Chave não encontrada, underflow não ocorre aqui
        return NULL;
    }

    int chave_idx = -1; // Índice da chave a ser removida ou substituída
    int filho_idx = 0;  // Índice do filho a descer

    // Encontra a chave ou o filho para descer
    if (chave == no->chaves[0])
    {
        chave_idx = 0;
    }
    else if (no->num_chaves == 2 && chave == no->chaves[1])
    {
        chave_idx = 1;
    }
    else if (chave < no->chaves[0])
    {
        filho_idx = 0;
    }
    else if (no->num_chaves == 1 || (no->num_chaves == 2 && chave < no->chaves[1]))
    {
        filho_idx = 1;
    }
    else
    { // chave > no->chaves[1]
        filho_idx = 2;
    }

    if (no->eh_folha)
    {
        if (chave_idx != -1)
        { // Chave encontrada na folha
            if (no->num_chaves == 2)
            {
                remover_chave_folha_simples(no, chave);
                *underflow_propagado = 0;
            }
            else
            {                       // Nó folha tem apenas 1 chave, remover causa underflow
                no->num_chaves = 0; // Marca como vazio (underflow)
                *underflow_propagado = 1;
                // O tratamento do underflow será feito pelo chamador (pai)
            }
        }
        else
        {
            // Chave não encontrada na folha
            *underflow_propagado = 0;
        }
    }
    else
    { // Nó interno
        if (chave_idx != -1)
        { // Chave encontrada em nó interno
            // Encontrar o sucessor em ordem (sempre em uma folha)
            No23 *sucessor_no = encontrar_sucessor_no_folha(no, chave_idx);
            int sucessor_chave = sucessor_no->chaves[0]; // Sucessor é sempre a menor chave da folha mais à esquerda da subárvore direita

            // Substituir a chave no nó interno pela chave sucessora
            no->chaves[chave_idx] = sucessor_chave;

            // Agora, remover recursivamente a chave sucessora de sua folha original
            // Começa a busca pela sucessora a partir do filho apropriado
            int filho_sucessor_idx = (chave_idx == 0) ? 1 : 2;
            no->filhos[filho_sucessor_idx] = remover_recursivo(no->filhos[filho_sucessor_idx], sucessor_chave, underflow_propagado);
        }
        else
        { // Chave não está neste nó, descer para o filho apropriado
            no->filhos[filho_idx] = remover_recursivo(no->filhos[filho_idx], chave, underflow_propagado);
        }

        // Se um underflow foi propagado do filho que foi modificado
        if (*underflow_propagado)
        {
            tratar_underflow(no->filhos[filho_idx]);      // Trata o underflow no filho
            *underflow_propagado = (no->num_chaves == 0); // Propaga se o PAI ficou vazio após fusão
        }
    }
    return no;
}

// --- Tratamento de Underflow (Redistribuição e Fusão) ---

// Função chamada quando um nó filho ficou com 0 chaves após remoção
void tratar_underflow(No23 *no_vazio)
{
    if (!no_vazio || no_vazio->num_chaves > 0 || !no_vazio->pai)
    {
        // Não há underflow ou é a raiz (tratado em remover())
        return;
    }

    No23 *pai = no_vazio->pai;
    int idx_no_pai = -1; // Índice do ponteiro no pai que aponta para no_vazio
    No23 *irmao_esq = NULL;
    No23 *irmao_dir = NULL;

    // Encontrar índice do nó no pai e irmãos adjacentes
    for (int i = 0; i <= pai->num_chaves; ++i)
    {
        if (pai->filhos[i] == no_vazio)
        {
            idx_no_pai = i;
            if (i > 0)
                irmao_esq = pai->filhos[i - 1];
            if (i < pai->num_chaves)
                irmao_dir = pai->filhos[i + 1];
            break;
        }
    }

    // Tentar Redistribuição com irmão esquerdo
    if (irmao_esq && irmao_esq->num_chaves == 2)
    {
        redistribuir(no_vazio, irmao_esq, pai, idx_no_pai);
    }
    // Tentar Redistribuição com irmão direito
    else if (irmao_dir && irmao_dir->num_chaves == 2)
    {
        redistribuir(no_vazio, irmao_dir, pai, idx_no_pai);
    }
    // Se não puder redistribuir, fazer Fusão com irmão esquerdo
    else if (irmao_esq)
    {
        fundir(no_vazio, irmao_esq, pai, idx_no_pai);
    }
    // Fazer Fusão com irmão direito
    else if (irmao_dir)
    {
        fundir(no_vazio, irmao_dir, pai, idx_no_pai);
    }
}

// Função para redistribuir chaves entre nó vazio, pai e irmão (3-nó)
void redistribuir(No23 *no_vazio, No23 *irmao, No23 *pai, int idx_no_pai)
{
    // Implementação da lógica de rotação/redistribuição
    // - Chave do pai desce para no_vazio
    // - Chave apropriada do irmão sobe para o pai
    // - Filho apropriado do irmão (se não for folha) é movido para no_vazio
    // Atualizar num_chaves de todos os nós envolvidos
    // Exemplo simplificado (considerando irmão direito):
    if (pai->filhos[idx_no_pai + 1] == irmao)
    { // Irmão à direita
        no_vazio->chaves[0] = pai->chaves[idx_no_pai];
        no_vazio->num_chaves = 1;
        pai->chaves[idx_no_pai] = irmao->chaves[0];
        irmao->chaves[0] = irmao->chaves[1];
        irmao->num_chaves = 1;
        if (!irmao->eh_folha)
        {
            no_vazio->filhos[1] = irmao->filhos[0];
            if (no_vazio->filhos[1])
                no_vazio->filhos[1]->pai = no_vazio;
            irmao->filhos[0] = irmao->filhos[1];
            irmao->filhos[1] = irmao->filhos[2];
            irmao->filhos[2] = NULL;
        }
    }
    else
    { // Irmão à esquerda
        // Lógica similar para rotação com irmão esquerdo
    }
    // ... (código completo da redistribuição) ...
    printf("Redistribuição realizada.\n");
}

// Função para fundir nó vazio, chave do pai e irmão (2-nó)
void fundir(No23 *no_vazio, No23 *irmao, No23 *pai, int idx_no_pai)
{
    // Implementação da lógica de fusão
    // - Chave do pai desce para o irmão
    // - Chave(s) e filho(s) do nó vazio (se houver, o que não deveria neste ponto) são movidos para o irmão
    // - Nó vazio é liberado
    // - Ponteiro e chave correspondentes no pai são removidos
    // - num_chaves do pai é decrementado (pode causar underflow no pai)
    // Exemplo simplificado (considerando irmão direito):
    if (pai->filhos[idx_no_pai + 1] == irmao)
    {                                               // Irmão à direita
        irmao->chaves[1] = irmao->chaves[0];        // Desloca chave existente
        irmao->chaves[0] = pai->chaves[idx_no_pai]; // Chave do pai desce
        irmao->num_chaves = 2;
        // Mover filhos se não for folha
        if (!irmao->eh_folha)
        {
            irmao->filhos[2] = irmao->filhos[1];
            irmao->filhos[1] = irmao->filhos[0];
            irmao->filhos[0] = no_vazio->filhos[0]; // Filho do nó que estava vazio
            if (irmao->filhos[0])
                irmao->filhos[0]->pai = irmao;
        }
        // Remover chave e ponteiro do pai
        for (int i = idx_no_pai; i < pai->num_chaves - 1; ++i)
        {
            pai->chaves[i] = pai->chaves[i + 1];
            pai->filhos[i + 1] = pai->filhos[i + 2];
        }
        pai->num_chaves--;
        pai->filhos[pai->num_chaves + 1] = NULL; // Limpa último ponteiro
        free(no_vazio);
    }
    else
    { // Irmão à esquerda
        // Lógica similar para fusão com irmão esquerdo
    }
    // ... (código completo da fusão) ...
    printf("Fusão realizada. Verificando underflow no pai.\n");
}

// Função auxiliar para encontrar o nó folha sucessor (simplificado)
No23 *encontrar_sucessor_no_folha(No23 *no, int chave_idx)
{
    if (no->eh_folha)
        return NULL; // Não deveria ser chamado para folha

    // Desce para a subárvore direita/meio
    No23 *atual = no->filhos[chave_idx + 1];

    // Segue para a esquerda até a folha
    while (!atual->eh_folha)
    {
        atual = atual->filhos[0];
    }
    return atual;
}

// --- Exemplo de Uso (main) ---
/*
int main() {
    // Criar árvore exemplo
    No23* raiz = NULL;
    // ... (código para popular a árvore) ...

    printf("Removendo chave X...\n");
    raiz = remover(raiz, X);

    // ... (código para imprimir ou verificar a árvore) ...

    // ... (liberar memória da árvore) ...
    return 0;
}
*/