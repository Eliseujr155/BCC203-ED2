#ifndef ARVOREBINARIA_H
#define ARVOREBINARIA_H
#include "registro.h"

 // No da arvore adaptado para armazenar enderecos de disco  
    Registro registro;
    long esquerda;  // Posicao no arquivo (-1 para NULL) 
    long direita;   // Posicao no arquivo (-1 para NULL) 
} NoArquivo;

void inserirBinaria(const char *nomeArq, Registro reg);
Registro* buscarBinaria(const char *nomeArq, int chave);

#endif
