#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "registro.h"

#define MAX_REGISTROS 1000000 // 1 milhão de registros para cobrir todos os testes

// Função para preencher lixo nas strings para deixá-las pesadas (6KB)
void preencherLixo(Registro *reg) {
    reg->dado1 = rand();
    memset(reg->dado2, 'A' + (rand() % 26), sizeof(reg->dado2) - 1);
    reg->dado2[sizeof(reg->dado2) - 1] = '\0';
    memset(reg->dado3, 'B' + (rand() % 26), sizeof(reg->dado3) - 1);
    reg->dado3[sizeof(reg->dado3) - 1] = '\0';
}

// Embaralha um vetor de inteiros (Fisher-Yates) para criar a situação Mista
void embaralhar(int *vetor, int tamanho) {
    for (int i = tamanho - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = temp;
    }
}

int main() {
    srand(time(NULL));
    Registro reg;

    printf("Gerando registros_asc.bin (1.000.000 registros crescentes)...\n");
    FILE *fAsc = fopen("registros_asc.bin", "wb");
    for (int i = 1; i <= MAX_REGISTROS; i++) {
        reg.chave = i;
        preencherLixo(&reg);
        fwrite(&reg, sizeof(Registro), 1, fAsc);
    }
    fclose(fAsc);

    printf("Gerando registros_desc.bin (1.000.000 registros decrescentes)...\n");
    FILE *fDesc = fopen("registros_desc.bin", "wb");
    for (int i = MAX_REGISTROS; i >= 1; i--) {
        reg.chave = i;
        preencherLixo(&reg);
        fwrite(&reg, sizeof(Registro), 1, fDesc);
    }
    fclose(fDesc);

    printf("Gerando registros_mix.bin (1.000.000 registros desordenados)...\n");
    // Aloca vetor de chaves para embaralhar primeiro
    int *chavesMix = malloc(MAX_REGISTROS * sizeof(int));
    for (int i = 0; i < MAX_REGISTROS; i++) chavesMix[i] = i + 1;
    embaralhar(chavesMix, MAX_REGISTROS);

    FILE *fMix = fopen("registros_mix.bin", "wb");
    for (int i = 0; i < MAX_REGISTROS; i++) {
        reg.chave = chavesMix[i];
        preencherLixo(&reg);
        fwrite(&reg, sizeof(Registro), 1, fMix);
    }
    fclose(fMix);
    free(chavesMix);

    printf("Concluido! Arquivos gerados com sucesso.\n");
    return 0;
}
