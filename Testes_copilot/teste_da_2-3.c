#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../2-3_funcional_por_enquanto.h"
#include "bibs_auxiliares/funcao_sistema.h"
#include "bibs_auxiliares/Escrever_resultado.h"

// gcc -o teste teste_da_2-3.c ../2-3_funcional_por_enquanto.c bibs_auxiliares/funcao_sistema.c bibs_auxiliares/Escrever_resultado.c

// Define o tamanho do vetor para inserção aleatória
#define TAM_VETOR 10

// Função para preencher um vetor com valores crescentes
void preencher_vetor_crescente(int *vetor, int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        vetor[i] = i + 1;
    }
}

// Função para preencher um vetor com valores decrescentes
void preencher_vetor_decrescente(int *vetor, int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        vetor[i] = tamanho - i;
    }
}

// Função para preencher um vetor com números aleatórios sem repetição
void preencher_vetor_aleatorio(int *vetor, int tamanho)
{
    // Inicializa o gerador de números aleatórios com o tempo atual
    srand(time(NULL));

    // Array para rastrear quais números já foram usados
    int usado[TAM_VETOR * 2] = {0};
    int valor, contador = 0;

    // Continue gerando números até preencher o vetor
    while (contador < tamanho)
    {
        valor = rand() % (tamanho * 5) + 1; // Gera valores entre 1 e tamanho*5 (para ter um bom intervalo)

        int repetido = 0;
        for (int j = 0; j < contador; j++)
        {
            if (vetor[j] == valor)
            {
                repetido = 1;
                break; // Se o número já foi usado, não o adiciona novamente
            }
        }

        if (!repetido)
        {
            vetor[contador++] = valor;
        }
    }
}

void testar_insercao_repetida(Arv23 **raiz)
{
    print_amarelo("\n\n=== TESTE DE INSERÇÃO DE VALORES REPETIDOS ===\n\n");

    // Se a árvore estiver vazia, insira alguns valores primeiro
    if (*raiz == NULL)
    {
        printf("Inserindo alguns valores iniciais na arvore...\n");
        int valores_iniciais[] = {10, 20, 30};
        for (int i = 0; i < 3; i++)
        {
            insere_23(raiz, valores_iniciais[i]);
        }

        printf("\nArvore inicial:\n");
        imprime_arvore_visual(*raiz, "", 1, 1);
        printf("\n\n");
    }

    // Pegar alguns valores que já existem na árvore
    int valores_repetidos[3];

    // Garantir que temos pelo menos um valor para repetir
    if (*raiz && (*raiz)->nInfo > 0)
    {
        valores_repetidos[0] = (*raiz)->info1;

        if ((*raiz)->nInfo > 1)
        {
            valores_repetidos[1] = (*raiz)->info2;
        }
        else
        {
            valores_repetidos[1] = valores_repetidos[0]; // Repetir o mesmo valor
        }

        if ((*raiz)->esq && (*raiz)->esq->nInfo > 0)
        {
            valores_repetidos[2] = (*raiz)->esq->info1;
        }
        else
        {
            valores_repetidos[2] = valores_repetidos[0]; // Repetir o mesmo valor
        }
    }
    else
    {
        // Valores padrão se a árvore estiver vazia
        valores_repetidos[0] = 10;
        valores_repetidos[1] = 10; // Mesmo valor para testar repetição
        valores_repetidos[2] = 20;

        // Inserir pelo menos um valor para testar repetição depois
        insere_23(raiz, valores_repetidos[0]);
    }

    // Tentar inserir valores repetidos
    for (int i = 0; i < 3; i++)
    {
        printf("\n------------------------------------------------\n");
        printf("Tentando inserir o valor repetido: %d\n", valores_repetidos[i]);

        int resultado = insere_23(raiz, valores_repetidos[i]);

        if (!resultado)
        {
            mensagem_sucesso("Comportamento correto: valor repetido nao foi inserido!\n");
        }
        else
        {
            mensagem_erro("Erro: valor repetido foi inserido na arvore!\n");
        }

        printf("\nEstado atual da arvore:\n\n");
        imprime_arvore_visual(*raiz, "", 1, 1);
        printf("\n------------------------------------------------\n");

        // Pequena pausa para melhor visualização
        printf("\nPressione Enter para continuar...\n");
        pausar_tela();
    }

    printf("\nTeste de insercao de valores repetidos concluido!\n");
}

void testar_insercao(Arv23 **raiz, int *valores, int tamanho, char *tipo_vetor)
{
    // Corrigido: print_amarelo não aceita formatação como printf
    char buffer[100];
    sprintf(buffer, "\n\n=== TESTE DE INSERCAO NA ARVORE 2-3 COM VETOR %s ===\n\n", tipo_vetor);
    print_amarelo(buffer);

    printf("\nSequencia de valores para insercao: ");
    for (int i = 0; i < tamanho; i++)
    {
        printf("%d ", valores[i]);
    }
    printf("\n\n");

    // Insere cada valor e mostra a árvore após cada inserção
    for (int i = 0; i < tamanho; i++)
    {
        printf("\n------------------------------------------------\n");
        printf("Inserindo o valor: %d\n", valores[i]);

        if (insere_23(raiz, valores[i]))
        {
            mensagem_sucesso("Valor inserido com sucesso!\n");
        }
        else
        {
            mensagem_erro("Falha ao inserir o valor!\n");
        }

        printf("\nEstado atual da arvore:\n\n");
        imprime_arvore_visual(*raiz, "", 1, 1);
        printf("\n------------------------------------------------\n");
    }

    printf("\n\nTodos os valores do vetor %s foram inseridos!\n", tipo_vetor);
    printf("\nArvore final apos insercoes com vetor %s:\n\n", tipo_vetor);
    imprime_arvore_visual(*raiz, "", 1, 1);
    printf("\n");
}

void testar_remocao(Arv23 **raiz, int *valores, int tamanho, char *tipo_vetor)
{
    // Corrigido: print_amarelo não aceita formatação como printf
    char buffer[100];
    sprintf(buffer, "\n\n=== TESTE DE REMOCAO NA ARVORE 2-3 COM VETOR %s ===\n\n", tipo_vetor);
    print_amarelo(buffer);

    printf("Arvore antes das remocoes:\n\n");
    imprime_arvore_visual(*raiz, "", 1, 1);
    printf("\n\n");

    // Remover cada valor do vetor
    for (int i = 0; i < tamanho; i++)
    {
        printf("\n------------------------------------------------\n");
        printf("Removendo o valor: %d\n\n", valores[i]);

        if (remover_23(raiz, valores[i]))
        {
            mensagem_sucesso("Valor removido com sucesso!\n");
        }
        else
        {
            mensagem_erro("Falha ao remover o valor!\n");
        }

        printf("\nArvore apos remocao:\n\n");
        imprime_arvore_visual(*raiz, "", 1, 1);
        printf("\n------------------------------------------------\n");
    }

    printf("\n\nTodos os valores do vetor %s foram removidos!\n", tipo_vetor);
    printf("\nArvore final apos remocoes com vetor %s:\n\n", tipo_vetor);
    imprime_arvore_visual(*raiz, "", 1, 1);
    printf("\n");
}

void testar_impressao(Arv23 *raiz)
{
    print_amarelo("\n\n=== TESTE DE IMPRESSAO EM ORDEM ===\n\n");

    printf("Elementos em ordem: ");
    imprime_23_em_ordem(raiz);
    printf("\n\n");
    mensagem_sucesso("Impressao em ordem realizada com sucesso!\n");

    print_amarelo("\n\n=== TESTE DE IMPRESSAO VISUAL DA ARVORE ===\n\n");
    imprime_arvore_visual(raiz, "", 1, 1);
    printf("\n\n");
    mensagem_sucesso("Impressao visual realizada com sucesso!\n");
}

int main()
{
    limpar_tela();
    print_amarelo("\n\n====================================");
    print_amarelo("\n    INICIANDO TESTES DA ARVORE 2-3    ");
    print_amarelo("\n====================================\n\n");

    // Criar os três vetores para teste
    int vetor_crescente[TAM_VETOR];
    int vetor_decrescente[TAM_VETOR];
    int vetor_aleatorio[TAM_VETOR];

    // Preencher os vetores
    preencher_vetor_crescente(vetor_crescente, TAM_VETOR);
    preencher_vetor_decrescente(vetor_decrescente, TAM_VETOR);
    preencher_vetor_aleatorio(vetor_aleatorio, TAM_VETOR);

    // Inicializando a árvore vazia
    Arv23 *raiz = NULL;

    // Testando operações com árvore vazia
    print_amarelo("\n=== TESTE COM ARVORE VAZIA ===\n\n");
    printf("Arvore vazia: ");
    imprime_23_em_ordem(raiz);
    printf("\n\n");

    pausar_tela();

    // Teste de inserção com vetor crescente
    limpar_tela();
    testar_insercao(&raiz, vetor_crescente, TAM_VETOR, "CRESCENTE");
    pausar_tela();

    // Teste de impressão
    limpar_tela();
    testar_impressao(raiz);
    pausar_tela();

    // Teste de remoção com vetor crescente
    limpar_tela();
    testar_remocao(&raiz, vetor_crescente, TAM_VETOR, "CRESCENTE");
    pausar_tela();

    // Teste de inserção com vetor decrescente
    limpar_tela();
    testar_insercao(&raiz, vetor_decrescente, TAM_VETOR, "DECRESCENTE");
    pausar_tela();

    // Teste de impressão
    limpar_tela();
    testar_impressao(raiz);
    pausar_tela();

    // Teste de remoção com vetor decrescente
    limpar_tela();
    testar_remocao(&raiz, vetor_decrescente, TAM_VETOR, "DECRESCENTE");
    pausar_tela();

    // Teste de inserção com vetor aleatório
    limpar_tela();
    testar_insercao(&raiz, vetor_aleatorio, TAM_VETOR, "ALEATORIO");
    pausar_tela();

    // Teste de impressão
    limpar_tela();
    testar_impressao(raiz);
    pausar_tela();

    // Teste de remoção com vetor aleatório
    limpar_tela();
    testar_remocao(&raiz, vetor_aleatorio, TAM_VETOR, "ALEATORIO");
    pausar_tela();

    // Teste de liberação da árvore
    limpar_tela();
    print_amarelo("TESTE DE LIBERACAO DA ARVORE");
    libera_arvore(&raiz);
    if (raiz == NULL)
    {
        mensagem_sucesso("Arvore liberada com sucesso!");
    }
    else
    {
        mensagem_erro("Falha ao liberar a arvore!");
    }

    printf("\n");
    print_verde("TODOS OS TESTES CONCLUIDOS!");
    printf("\n\n");
    pausar_tela();

    return 0;
}
