#ifndef ACESSOSEQUENCIAL_H
#define ACESSOSEQUENCIAL_H
#include "tempo.h"

//fixa 4 como numero de registro por pagina
#define ITENSPAGINA 4 
#define MAXTABELA 100


// estrutura de feinição do formato, vai guardar o numero da pagina e a menor chave 
typedef struct {
    int posicao;      
    int chave;        
} TipoIndice;

// tipoitem redefine a extrutura do registro pois tem que ser identico pro sizeof 
// dar os mesmos 6kb garantindo que não de erro na leitura do fread 
typedef struct {
    int chave;
    long int dado1;
    char dado2[1000];
    char dado3[5000];
} TipoItem;

//estrutura  representa um bloco de dads trazido do disco para a memória principal
typedef struct {
    TipoItem itens[ITENSPAGINA];
    int numItens;  
} PaginaAS;

int criarIndicePaginas(const char *nomeArquivo, TipoIndice tabela[], int numRegistros, long *transferencias, double *tempoCriacao);
int buscarPaginaNoIndice(int chave, TipoIndice tabela[], int numPaginas, long *comp);
int carregarPagina(const char *nomeArquivo, int numPagina, PaginaAS *paginaAlvo, int numRegistros, long *transferencias);
int buscarNaPagina(int chave, PaginaAS *pag, long *comp, TipoItem *resultado);

void lerArquivoSequencial(const char *nomeArquivo, int quantidade, int chave, long *transferencias, long *comp, double *tempo, TipoItem *resultado, int *encontrado, TipoIndice *tabelaIndice, int numPaginas);

void pesquisar10AleatoriasSI(const char *nomeArquivo, int quantidade);
void executarSequencial(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves);

#endif
