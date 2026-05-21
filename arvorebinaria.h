#ifndef ARVOREBINARIA_H
#define ARVOREBINARIA_H
#include "registro.h"
#include "tempo.h"

typedef struct {
    Registro registro;
    long esquerda;  // -1 se nao existe
    long direita;   // -1 se nao existe
} NoArquivo;

void criarArvoreBinaria(const char *nomeArquivoArvore);
void inserirEmArquivo(const char *nomeArquivoArvore, Registro reg, long *comp);
Registro* buscarEmArquivo(const char *nomeArquivoArvore, int chave, long *comp, long *transferencias);
void lerArquivoBinario(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros, long *transferencias, long *comp, double *tempo);

// Alterado para 10 chaves (Fase 2)
void pesquisar10AleatoriasAB(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros);
void executarArvoreBinaria(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves);

#endif
