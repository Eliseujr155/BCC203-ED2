#ifndef ARVOREB_H
#define ARVOREB_H
#include "registro.h"

#define M 50      // Ordem da árvore
#define MM (2 * M)

typedef struct TipoPagina* TipoApontador;

// Estrutura do nó da Árvore B. Cada nó (página) armazena vários registros.
typedef struct TipoPagina {
    short n;                // Quantos registros estão ocupados na página
    Registro r[MM];         // O array de registros físicos
    TipoApontador p[MM + 1]; // Os ponteiros para as subárvores filhas
} TipoPagina;

void Insere(Registro Reg, TipoApontador *Ap);
void Pesquisa(Registro *x, TipoApontador Ap);

#endif
