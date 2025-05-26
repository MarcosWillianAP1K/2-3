

#pragma once


void limpar_buffer();

void limpar_tela();

void pausar_tela();

void print_amarelo(char *mensagem);

void print_vermelho(char *mensagem);

void print_verde(char *mensagem);

void mensagem_erro(char *mensagem);

void mensagem_sucesso(char *mensagem);

void verificar_alocacao(void *ponteiro);

void verificar_realocacao(void *ponteiro);

char *digitar_string();

short int digitar_short_int();

char digitar_um_caracter();

void corrigir_espacos(char **str);

