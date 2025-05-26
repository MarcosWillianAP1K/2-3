#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h> // Para _mkdir no Windows
#include "../2-3_funcional_por_enquanto.h"
#include "bibs_auxiliares/funcao_sistema.h"
#include "bibs_auxiliares/Escrever_resultado.h"

// gcc -o teste teste_da_2-3.c ../2-3_funcional_por_enquanto.c bibs_auxiliares/funcao_sistema.c bibs_auxiliares/Escrever_resultado.c

// Define o tamanho do vetor para inserção aleatória
#define TAM_VETOR 10
#define DIR_RESULTADOS "Resultados"

// Função para criar diretório se não existir
void criar_diretorio_resultados() {
    // Cria o diretório se não existir
    #ifdef _WIN32
    _mkdir(DIR_RESULTADOS);
    #else
    mkdir(DIR_RESULTADOS, 0777);
    #endif
}

// Funções para redirecionar output para arquivo
FILE* redirecionar_para_arquivo(const char* nome_arquivo) {
    char caminho[100];
    sprintf(caminho, "%s/%s", DIR_RESULTADOS, nome_arquivo);
    
    // Salva o ponteiro stdout original
    FILE* stdout_original = stdout;
    
    // Redireciona stdout para o arquivo
    FILE* arquivo = freopen(caminho, "w", stdout);
    if (!arquivo) {
        printf("Erro ao abrir arquivo %s para escrita\n", caminho);
        return NULL;
    }
    
    return stdout_original;
}

// Função para restaurar stdout
void restaurar_stdout(FILE* stdout_original) {
    // Fecha o arquivo atual
    fclose(stdout);
    
    // Restaura stdout usando freopen para redirecionar para o console
    #ifdef _WIN32
    freopen("CON", "w", stdout);
    #else
    freopen("/dev/tty", "w", stdout);
    #endif
}

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

void testar_insercao(Arv23 **raiz, int *valores, int tamanho, char *tipo_vetor)
{
    // Preparar nome do arquivo
    char nome_arquivo[50];
    sprintf(nome_arquivo, "insercao_%s.txt", tipo_vetor);
    
    // Redirecionar stdout para arquivo
    FILE* stdout_original = redirecionar_para_arquivo(nome_arquivo);
    
    printf("\n\n=== TESTE DE INSERCAO NA ARVORE 2-3 COM VETOR %s ===\n\n", tipo_vetor);

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
            printf("SUCESSO: Valor inserido com sucesso!\n");
        }
        else
        {
            printf("ERROR: Falha ao inserir o valor!\n");
        }

        printf("\nEstado atual da arvore:\n\n");
        imprime_arvore_visual(*raiz, "", 1, 1);
        printf("\n------------------------------------------------\n");
    }

    printf("\n\nTodos os valores do vetor %s foram inseridos!\n", tipo_vetor);
    printf("\nArvore final apos insercoes com vetor %s:\n\n", tipo_vetor);
    imprime_arvore_visual(*raiz, "", 1, 1);
    printf("\n");
    
    // Restaurar stdout
    restaurar_stdout(stdout_original);
    printf("Resultado do teste salvo em %s/%s\n", DIR_RESULTADOS, nome_arquivo);
}

void testar_remocao(Arv23 **raiz, int *valores, int tamanho, char *tipo_vetor)
{
    // Preparar nome do arquivo
    char nome_arquivo[50];
    sprintf(nome_arquivo, "remocao_%s.txt", tipo_vetor);
    
    // Redirecionar stdout para arquivo
    FILE* stdout_original = redirecionar_para_arquivo(nome_arquivo);
    
    printf("\n\n=== TESTE DE REMOCAO NA ARVORE 2-3 COM VETOR %s ===\n\n", tipo_vetor);
    
    // Imprimir o vetor que será utilizado para remoção
    printf("Sequencia de valores para remocao: ");
    for (int i = 0; i < tamanho; i++)
    {
        printf("%d ", valores[i]);
    }
    printf("\n\n");

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
            printf("SUCESSO: Valor removido com sucesso!\n");
        }
        else
        {
            printf("ERROR: Falha ao remover o valor!\n");
        }

        printf("\nArvore apos remocao:\n\n");
        imprime_arvore_visual(*raiz, "", 1, 1);
        printf("\n------------------------------------------------\n");
    }

    printf("\n\nTodos os valores do vetor %s foram removidos!\n", tipo_vetor);
    printf("\nArvore final apos remocoes com vetor %s:\n\n", tipo_vetor);
    imprime_arvore_visual(*raiz, "", 1, 1);
    printf("\n");
    
    // Restaurar stdout
    restaurar_stdout(stdout_original);
    printf("Resultado do teste salvo em %s/%s\n", DIR_RESULTADOS, nome_arquivo);
}

void testar_impressao(Arv23 *raiz, char *tipo_arvore)
{
    // Preparar nome do arquivo
    char nome_arquivo[50];
    sprintf(nome_arquivo, "impressao_%s.txt", tipo_arvore);
    
    // Redirecionar stdout para arquivo
    FILE* stdout_original = redirecionar_para_arquivo(nome_arquivo);
    
    printf("\n\n=== TESTE DE IMPRESSAO DA ARVORE 2-3 (%s) ===\n\n", tipo_arvore);

    printf("Elementos em ordem: ");
    imprime_23_em_ordem(raiz);
    printf("\n\n");
    printf("SUCESSO: Impressao em ordem realizada com sucesso!\n");

    printf("\n\n=== TESTE DE IMPRESSAO VISUAL DA ARVORE (%s) ===\n\n", tipo_arvore);
    imprime_arvore_visual(raiz, "", 1, 1);
    printf("\n\n");
    printf("SUCESSO: Impressao visual realizada com sucesso!\n");
    
    // Restaurar stdout
    restaurar_stdout(stdout_original);
    printf("Resultado do teste salvo em %s/%s\n", DIR_RESULTADOS, nome_arquivo);
}

// Função para testar liberação das três árvores e salvar resultados em arquivo
void testar_liberacao_tres_arvores(Arv23 **raiz_crescente, Arv23 **raiz_decrescente, Arv23 **raiz_aleatorio)
{
    // Preparar nome do arquivo
    char nome_arquivo[50];
    sprintf(nome_arquivo, "liberacao_arvores.txt");
    
    // Redirecionar stdout para arquivo
    FILE* stdout_original = redirecionar_para_arquivo(nome_arquivo);
    
    printf("\n\n=== TESTE DE LIBERACAO DAS ARVORES 2-3 ===\n\n");
    
    // Liberar árvore crescente
    printf("=== LIBERANDO ARVORE CRESCENTE ===\n\n");
    printf("Estado da arvore crescente antes da liberacao:\n\n");
    
    if (*raiz_crescente == NULL) {
        printf("Arvore crescente ja esta vazia.\n\n");
    } else {
        imprime_arvore_visual(*raiz_crescente, "", 1, 1);
        printf("\n\n");
        printf("Liberando a arvore crescente...\n\n");
        libera_arvore(raiz_crescente);
        
        if (*raiz_crescente == NULL) {
            printf("SUCESSO: Arvore crescente liberada com sucesso!\n\n");
        } else {
            printf("ERROR: Falha ao liberar a arvore crescente!\n\n");
        }
    }
    
    // Liberar árvore decrescente
    printf("\n=== LIBERANDO ARVORE DECRESCENTE ===\n\n");
    printf("Estado da arvore decrescente antes da liberacao:\n\n");
    
    if (*raiz_decrescente == NULL) {
        printf("Arvore decrescente ja esta vazia.\n\n");
    } else {
        imprime_arvore_visual(*raiz_decrescente, "", 1, 1);
        printf("\n\n");
        printf("Liberando a arvore decrescente...\n\n");
        libera_arvore(raiz_decrescente);
        
        if (*raiz_decrescente == NULL) {
            printf("SUCESSO: Arvore decrescente liberada com sucesso!\n\n");
        } else {
            printf("ERROR: Falha ao liberar a arvore decrescente!\n\n");
        }
    }
    
    // Liberar árvore aleatória
    printf("\n=== LIBERANDO ARVORE ALEATORIA ===\n\n");
    printf("Estado da arvore aleatoria antes da liberacao:\n\n");
    
    if (*raiz_aleatorio == NULL) {
        printf("Arvore aleatoria ja esta vazia.\n\n");
    } else {
        imprime_arvore_visual(*raiz_aleatorio, "", 1, 1);
        printf("\n\n");
        printf("Liberando a arvore aleatoria...\n\n");
        libera_arvore(raiz_aleatorio);
        
        if (*raiz_aleatorio == NULL) {
            printf("SUCESSO: Arvore aleatoria liberada com sucesso!\n\n");
        } else {
            printf("ERROR: Falha ao liberar a arvore aleatoria!\n\n");
        }
    }
    
    printf("\n=== RESULTADO FINAL DA LIBERACAO ===\n\n");
    if (*raiz_crescente == NULL && *raiz_decrescente == NULL && *raiz_aleatorio == NULL) {
        printf("SUCESSO: Todas as arvores foram liberadas com sucesso!\n");
    } else {
        printf("ERROR: Falha ao liberar uma ou mais arvores!\n");
    }
    
    // Restaurar stdout
    restaurar_stdout(stdout_original);
    printf("Resultado do teste salvo em %s/%s\n", DIR_RESULTADOS, nome_arquivo);
}

// Função para combinar os resultados dos três vetores em um único arquivo
void salvar_resultados_combinados(int *vetor_crescente, int *vetor_decrescente, int *vetor_aleatorio, int tamanho)
{
    // Preparar nome do arquivo
    char nome_arquivo[50];
    sprintf(nome_arquivo, "todos_os_testes.txt");
    
    // Redirecionar stdout para arquivo
    FILE* stdout_original = redirecionar_para_arquivo(nome_arquivo);
    
    // Título geral
    printf("\n\n====================================\n");
    printf("    RESULTADOS COMPLETOS DOS TESTES DA ARVORE 2-3    \n");
    printf("====================================\n\n");
    
    // Seção de descrição dos vetores
    printf("=== VETORES DE TESTE UTILIZADOS ===\n\n");
    
    // Vetor crescente
    printf("Vetor CRESCENTE: ");
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", vetor_crescente[i]);
    }
    printf("\n\n");
    
    // Vetor decrescente
    printf("Vetor DECRESCENTE: ");
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", vetor_decrescente[i]);
    }
    printf("\n\n");
    
    // Vetor aleatório
    printf("Vetor ALEATORIO: ");
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", vetor_aleatorio[i]);
    }
    printf("\n\n");
    
    // Seção de resumo
    printf("=== RESUMO DOS TESTES ===\n\n");
    printf("1. Teste de insercao com vetor CRESCENTE\n");
    printf("2. Teste de remocao com vetor CRESCENTE\n");
    printf("3. Teste de insercao com vetor DECRESCENTE\n");
    printf("4. Teste de remocao com vetor DECRESCENTE\n");
    printf("5. Teste de insercao com vetor ALEATORIO\n");
    printf("6. Teste de remocao com vetor ALEATORIO\n");
    printf("7. Teste de impressao da arvore\n");
    printf("8. Teste de insercao de valores repetidos\n");
    printf("9. Teste de liberacao da arvore\n\n");
    
    printf("Todos os testes foram executados com sucesso!\n");
    printf("Consulte os arquivos individuais para ver os detalhes de cada teste.\n\n");
    
    // Restaurar stdout
    restaurar_stdout(stdout_original);
    printf("Resumo de todos os testes salvo em %s/%s\n", DIR_RESULTADOS, nome_arquivo);
}

int main()
{
    limpar_tela();
    
    // Criar diretório para resultados se não existir
    criar_diretorio_resultados();
    
    printf("\n\n====================================\n");
    printf("    INICIANDO TESTES DA ARVORE 2-3    \n");
    printf("====================================\n\n");
    printf("Os resultados dos testes serao salvos no diretorio '%s'\n\n", DIR_RESULTADOS);
    
    // Criar os três vetores para teste
    int vetor_crescente[TAM_VETOR];
    int vetor_decrescente[TAM_VETOR];
    int vetor_aleatorio[TAM_VETOR];

    // Preencher os vetores
    preencher_vetor_crescente(vetor_crescente, TAM_VETOR);
    preencher_vetor_decrescente(vetor_decrescente, TAM_VETOR);
    preencher_vetor_aleatorio(vetor_aleatorio, TAM_VETOR);

    // Inicializando as três árvores separadamente
    Arv23 *raiz_crescente = NULL;
    Arv23 *raiz_decrescente = NULL;
    Arv23 *raiz_aleatorio = NULL;

    // Testando operações com árvore vazia
    printf("\n=== TESTE COM ARVORE VAZIA ===\n\n");
    printf("Arvore vazia: ");
    imprime_23_em_ordem(raiz_crescente);
    printf("\n\n");

    pausar_tela();

    // Teste de inserção com vetor crescente
    limpar_tela();
    testar_insercao(&raiz_crescente, vetor_crescente, TAM_VETOR, "CRESCENTE");
    pausar_tela();

    // Teste de impressão da árvore crescente
    limpar_tela();
    testar_impressao(raiz_crescente, "CRESCENTE");
    pausar_tela();

    // Teste de inserção com vetor decrescente
    limpar_tela();
    testar_insercao(&raiz_decrescente, vetor_decrescente, TAM_VETOR, "DECRESCENTE");
    pausar_tela();

    // Teste de impressão da árvore decrescente
    limpar_tela();
    testar_impressao(raiz_decrescente, "DECRESCENTE");
    pausar_tela();

    // Teste de inserção com vetor aleatório
    limpar_tela();
    testar_insercao(&raiz_aleatorio, vetor_aleatorio, TAM_VETOR, "ALEATORIO");
    pausar_tela();

    // Teste de impressão da árvore aleatória
    limpar_tela();
    testar_impressao(raiz_aleatorio, "ALEATORIO");
    pausar_tela();

    // Teste de remoção com vetor crescente
    limpar_tela();
    testar_remocao(&raiz_crescente, vetor_crescente, TAM_VETOR, "CRESCENTE");
    pausar_tela();

    // Teste de remoção com vetor decrescente
    limpar_tela();
    testar_remocao(&raiz_decrescente, vetor_decrescente, TAM_VETOR, "DECRESCENTE");
    pausar_tela();

    // Teste de remoção com vetor aleatório
    limpar_tela();
    testar_remocao(&raiz_aleatorio, vetor_aleatorio, TAM_VETOR, "ALEATORIO");
    pausar_tela();

    // Recriar as árvores para o teste de liberação
    testar_insercao(&raiz_crescente, vetor_crescente, TAM_VETOR, "CRESCENTE");
    testar_insercao(&raiz_decrescente, vetor_decrescente, TAM_VETOR, "DECRESCENTE");
    testar_insercao(&raiz_aleatorio, vetor_aleatorio, TAM_VETOR, "ALEATORIO");
    
    // Teste de liberação das três árvores
    limpar_tela();
    testar_liberacao_tres_arvores(&raiz_crescente, &raiz_decrescente, &raiz_aleatorio);
    pausar_tela();
    
    // Salvar um resumo de todos os testes em um único arquivo
    salvar_resultados_combinados(vetor_crescente, vetor_decrescente, vetor_aleatorio, TAM_VETOR);

    printf("\n");
    printf("TODOS OS TESTES CONCLUIDOS!\n");
    printf("\n\n");
    pausar_tela();

    return 0;
}
