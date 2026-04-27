#ifndef ARVOREBESTRELA_H
#define ARVOREBESTRELA_H
#include "registro.h"

#define MM 100 

// Define se a página é um nó de índice (só chaves) ou folha (dados completos)
typedef enum {Interna, Externa} TipoIntExt;

typedef struct TipoPaginaEstrela* TipoApontadorEstrela;

typedef struct TipoPaginaEstrela {
    TipoIntExt Pt; // Variável para controle do tipo do nó
    union {
        struct { // Estrutura para os nós de cima (índice)
            int ni;
            int ri[MM]; // Guarda apenas os inteiros das chaves para economizar RAM
            TipoApontadorEstrela pi[MM + 1];
        } U0;
        struct { // Estrutura para os nós folha (onde o registro de 6kb realmente está)
            int ne;
            Registro re[MM];
        } U1;
    } UU;
} TipoPaginaEstrela;

void PesquisaEstrela(Registro *x, TipoApontadorEstrela *Ap);

#endif
