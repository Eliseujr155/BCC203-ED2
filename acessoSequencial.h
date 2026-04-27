#ifndef ACESSOSEQUENCIAL_H
#define ACESSOSEQUENCIAL_H
#include "registro.h"

#define ITENSPAGINA 4 // Cada página no disco terá 4 registros

// Estrutura do índice que fica na RAM para não precisar ler o arquivo todo
typedef struct {
    int posicao; // Qual é a página no arquivo
    int chave;   // Menor chave daquela página
} tipoindice;

int criarIndicePaginas(const char *nomeArquivo, tipoindice tabela[], int numRegistros);
Registro* pesquisaSequencial(const char *nomeArquivo, int chave, tipoindice tabela[], int tam);

#endif
