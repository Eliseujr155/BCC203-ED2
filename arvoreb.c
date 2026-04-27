#include <stdio.h>
#include <stdlib.h>
#include "arvoreb.h"

/* Pesquisa sequencial para encontrar o intervalo na pagina [cite: 298, 300] */
void Pesquisa(Registro *x, TipoApontador Ap) {
    long i = 1;
    if (Ap == NULL) {
        x->chave = -1; /* Sinaliza que nao encontrou */
        return;
    }
    while (i < Ap->n && x->chave > Ap->r[i-1].chave) i++;

    if (x->chave == Ap->r[i-1].chave) {
        *x = Ap->r[i-1]; /* Chave localizada [cite: 301, 302] */
        return;
    }

    /* Ativacao recursiva na subarvore adequada [cite: 307, 308] */
    if (x->chave < Ap->r[i-1].chave) Pesquisa(x, Ap->p[i-1]);
    else Pesquisa(x, Ap->p[i]);
}

/* Funcao auxiliar para inserir ordenadamente na pagina [cite: 382, 394] */
void InsereNaPagina(TipoApontador Ap, Registro Reg, TipoApontador ApDir) {
    int k = Ap->n;
    while (k > 0) {
        if (Reg.chave >= Ap->r[k-1].chave) break;
        Ap->r[k] = Ap->r[k-1];
        Ap->p[k+1] = Ap->p[k];
        k--;
    }
    Ap->r[k] = Reg;
    Ap->p[k+1] = ApDir;
    Ap->n++;
}
