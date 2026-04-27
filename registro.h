#ifndef REGISTRO_H
#define REGISTRO_H

// Estrutura principal do registro seguindo os tamanhos pedidos no trabalho
typedef struct {
    int chave;          // Chave usada na pesquisa
    long int dado1;     
    char dado2[1000];   // Strings grandes para simular arquivo pesado
    char dado3[5000]; 
} Registro;

#endif
