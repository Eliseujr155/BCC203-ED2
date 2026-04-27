#ifndef REGISTRO_H
#define REGISTRO_H

/* Estrutura do registro definida conforme a especificacao do TP */
typedef struct {
    int chave;          /* Campo chave de pesquisa */
    long int dado1;     /* Valor inteiro longo */
    char dado2[1000];   /* Cadeia de 1000 caracteres */
    char dado3[5000];   /* Cadeia de 5000 caracteres */
} Registro;

#endif
