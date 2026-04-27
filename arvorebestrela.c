#include <stdio.h>
#include <stdlib.h>
#include "arvorebestrela.h"

/* Pesquisa que sempre desce ate a folha [cite: 754, 774] */
void PesquisaEstrela(Registro *x, TipoApontadorEstrela *Ap) {
    int i;
    TipoApontadorEstrela Pag = *Ap;

    if ((*Ap)->Pt == Interna) {
        i = 1;
        /* Busca na pagina interna para decidir o caminho [cite: 777] */
        while (i < Pag->UU.U0.ni && x->chave > Pag->UU.U0.ri[i-1]) i++;
        if (x->chave < Pag->UU.U0.ri[i-1]) PesquisaEstrela(x, &Pag->UU.U0.pi[i-1]);
        else PesquisaEstrela(x, &Pag->UU.U0.pi[i]);
        return;
    }

    /* Chegou na pagina folha: busca o registro real [cite: 785, 787] */
    i = 1;
    while (i < Pag->UU.U1.ne && x->chave > Pag->UU.U1.re[i-1].chave) i++;
    if (x->chave == Pag->UU.U1.re[i-1].chave) {
        *x = Pag->UU.U1.re[i-1];
    } else {
        x->chave = -1; /* Nao encontrado */
    }
}
