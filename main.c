#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acessoSequencial.h"
#include "arvorebinaria.h"
#include "arvoreb.h"
#include "arvorebestrela.h"
#include "registro.h"

int main(int argc, char *argv[]) {

    // verifica se os argumentos minimos foram informados
    if (argc < 4) {
        printf("uso da Fase 1: %s <metodo> <quantidade> <situacao> <chave> [-P]\n", argv[0]);
        printf("uso da Fase 2: %s <metodo> <quantidade> <situacao> -t\n", argv[0]);
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);

    // verifica se esta no modo teste (Fase 2), onde pesquisa 10 chaves
    int modoTeste = (argc == 5 && (strcmp(argv[4], "-t") == 0 || strcmp(argv[4], "-T") == 0));

    int chave = 0;
    int imprimirChaves = 0; 

    // se nao for modo teste (Fase 1), deve existir a chave alvo
    if (!modoTeste) {
        if (argc < 5) {
            printf("faltando parametro <chave> ou flag -t\n");
            return 1;
        }

        chave = atoi(argv[4]);

        // verifica flag opcional de impressao
        if (argc == 6) {
            if (strcmp(argv[5], "[-P]") == 0 || strcmp(argv[5], "[-p]") == 0 || strcmp(argv[5], "-P") == 0) {
                imprimirChaves = 1;
            }
        }
    }

    char nomeArquivo[50];
    if (situacao == 1) strcpy(nomeArquivo, "registros_asc.bin");
    else if (situacao == 2) strcpy(nomeArquivo, "registros_desc.bin");
    else if (situacao == 3) strcpy(nomeArquivo, "registros_mix.bin");
    else {
        printf("situacao invalida\n");
        return 1;
    }

    printf("\n--- Teste com %d registros no arquivo %s ---\n", quantidade, nomeArquivo);

    switch (metodo) {
        case 1:
            executarSequencial(nomeArquivo, quantidade, chave, modoTeste, imprimirChaves);
            break;
        case 2:
            executarArvoreBinaria(nomeArquivo, quantidade, chave, modoTeste, imprimirChaves);
            break;
        case 3:
            executarArvoreB(nomeArquivo, quantidade, chave, modoTeste, imprimirChaves);
            break;
        case 4:
            executarArvoreBEstrela(nomeArquivo, quantidade, chave, modoTeste, imprimirChaves);
            break;
        default:
            printf("metodo invalido\n");
            return 1;
    }

    return 0;
}
