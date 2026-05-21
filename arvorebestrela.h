#ifndef ARVOREBESTRELA_H
#define ARVOREBESTRELA_H
#include "registro.h"
#include "tempo.h"

#define VERDADEIRO 1
#define FALSO 0
#define ORDEM_ESTRELA 50 
#define MM_ESTRELA (2 * ORDEM_ESTRELA)

typedef enum { Interna, Externa } TipoPaginaEstrela;

typedef struct PaginaEstrela {
    TipoPaginaEstrela tipo;
    union {
        struct {
            int numChaves;
            int chaves[MM_ESTRELA]; 
            struct PaginaEstrela *filhos[MM_ESTRELA + 1]; 
        } interna;
        struct {
            int numRegistros; 
            Registro registros[MM_ESTRELA]; 
        } externa;
    } conteudo;
} PaginaEstrela;

Registro* pesquisaBEstrela(PaginaEstrela *pagina, int chave, long *comparacoes);
void insereBEstrela(Registro reg, PaginaEstrela **raiz, long *comparacoes);
void lerArquivoArvoreBEstrela(const char *nomeArquivo, int numRegistros, PaginaEstrela **raiz, long *transferencias, long *comparacoes, double *tempo);

// Alterado para 10 chaves (Fase 2)
void pesquisar10AleatoriasBEstrela(const char *nomeArquivo, int numRegistros, PaginaEstrela *raiz);
void executarArvoreBEstrela(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves);

#endif
