#include <stdio.h>
#include <stdlib.h>
#include "arvorebestrela.h"

// Pesquisa na B* que diferencia a navegação por chaves da busca final na folha
void PesquisaEstrela(Registro *x, TipoApontadorEstrela *Ap) {
    int i;
    TipoApontadorEstrela Pag = *Ap;

    // Se estivermos em um nó interno, comparamos apenas as chaves de índice
    if ((*Ap)->Pt == Interna) {
        i = 1;
        while (i < Pag->UU.U0.ni && x->chave > Pag->UU.U0.ri[i-1]) i++;
        
        // Decide qual subárvore descer
        if (x->chave < Pag->UU.U0.ri[i-1]) PesquisaEstrela(x, &Pag->UU.U0.pi[i-1]);
        else PesquisaEstrela(x, &Pag->UU.U0.pi[i]);
        return;
    }

    // Se o código chegou aqui, estamos na folha (Externa). Agora procuramos o dado real.
    i = 1;
    while (i < Pag->UU.U1.ne && x->chave > Pag->UU.U1.re[i-1].chave) i++;
    
    if (x->chave == Pag->UU.U1.re[i-1].chave) {
        *x = Pag->UU.U1.re[i-1]; // Sucesso
    } else {
        x->chave = -1; // Não existe no arquivo
    }
}
