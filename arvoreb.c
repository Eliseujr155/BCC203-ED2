#include <stdio.h>
#include <stdlib.h>
#include "arvoreb.h"

// Pesquisa recursiva clássica de árvore B. Varre a página e decide para qual filho descer.
void Pesquisa(Registro *x, TipoApontador Ap) {
    long i = 1;
    if (Ap == NULL) {
        x->chave = -1; // Se chegou no fim e não achou, invalida a chave de retorno
        return;
    }
    
    // Caminha dentro da página enquanto a chave procurada for maior
    while (i < Ap->n && x->chave > Ap->r[i-1].chave) i++;

    // Se achou a chave exata dentro desta página, retorna o registro completo
    if (x->chave == Ap->r[i-1].chave) {
        *x = Ap->r[i-1];
        return;
    }

    // Se não achou aqui, desce para o filho à esquerda ou à direita do intervalo
    if (x->chave < Ap->r[i-1].chave) Pesquisa(x, Ap->p[i-1]);
    else Pesquisa(x, Ap->p[i]);
}

// Função para enfiar um novo registro na página mantendo a ordenação das chaves
void InsereNaPagina(TipoApontador Ap, Registro Reg, TipoApontador ApDir) {
    int k = Ap->n;
    // Puxa os registros maiores para a direita para abrir espaço
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
