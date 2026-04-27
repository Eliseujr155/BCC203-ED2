#ifndef ARVOREBESTRELA_H
#define ARVOREBESTRELA_H
#include "registro.h"

#define MM 100 /* Ordem para paginas internas */

/* Tipos de pagina conforme slide 54 [cite: 730] */
typedef enum {Interna, Externa} TipoIntExt;

typedef struct TipoPaginaEstrela* TipoApontadorEstrela;
typedef struct TipoPaginaEstrela {
    TipoIntExt Pt; /* Indica se e interna ou externa [cite: 737] */
    union {
        struct { /* Pagina de indice: apenas chaves [cite: 741, 742] */
            int ni;
            int ri[MM];
            TipoApontadorEstrela pi[MM + 1];
        } U0;
        struct { /* Pagina de dados: registros completos [cite: 745, 746] */
            int ne;
            Registro re[MM];
        } U1;
    } UU;
} TipoPaginaEstrela;

void PesquisaEstrela(Registro *x, TipoApontadorEstrela *Ap);

#endif
