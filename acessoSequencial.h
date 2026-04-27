#ifndef ACESSOSEQUENCIAL_H
#define ACESSOSEQUENCIAL_H
#include "registro.h"

#define ITENSPAGINA 4 /* Tamanho da pagina conforme slide 11 */

/* Estrutura para entrada da tabela de indice das paginas [cite: 116] */
typedef struct {
    int posicao; /* Endereco da pagina no disco */
    int chave;   /* Chave do primeiro item da pagina */
} tipoindice;

/* Funcoes principais do metodo */
int criarIndicePaginas(const char *nomeArquivo, tipoindice tabela[], int numRegistros);
Registro* pesquisaSequencial(const char *nomeArquivo, int chave, tipoindice tabela[], int tam);

#endif
