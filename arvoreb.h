#ifndef ARVOREB_H
#define ARVOREB_H
#include "registro.h"

#define M 50      /* Ordem da arvore */
#define MM (2 * M)

/* Estrutura da pagina conforme slide 23 [cite: 268] */
typedef struct TipoPagina* TipoApontador;
typedef struct TipoPagina {
    short n;                /* Numero de itens na pagina [cite: 268] */
    Registro r[MM];         /* Vetor de registros [cite: 269] */
    TipoApontador p[MM + 1]; /* Vetor de filhos [cite: 270] */
} TipoPagina;

void Insere(Registro Reg, TipoApontador *Ap);
void Pesquisa(Registro *x, TipoApontador Ap);

#endif
