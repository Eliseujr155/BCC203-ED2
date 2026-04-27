#include <stdio.h>
#include <stdlib.h>
#include "acessoSequencial.h"

/* Gera a tabela de indice das paginas lendo o arquivo [cite: 174, 178] */
int criarIndicePaginas(const char *nomeArquivo, tipoindice tabela[], int numRegistros) {
    FILE *arq = fopen(nomeArquivo, "rb");
    if (!arq) return 0;
    
    int pos = 0, cont = 0;
    Registro x;
    
    /* Le o arquivo e guarda a primeira chave de cada pagina no indice [cite: 176, 177] */
    while (fread(&x, sizeof(Registro), 1, arq) == 1 && cont < numRegistros) {
        if (cont % ITENSPAGINA == 0) {
            tabela[pos].chave = x.chave;
            tabela[pos].posicao = pos + 1;
            pos++;
        }
        cont++;
    }
    fclose(arq);
    return pos; /* Retorna o tamanho preenchido da tabela */
}

/* Localiza a pagina no indice e depois pesquisa no disco [cite: 124, 130] */
Registro* pesquisaSequencial(const char *nomeArquivo, int chave, tipoindice tabela[], int tam) {
    int i = 0;
    /* Procura no indice a pagina que pode conter o item [cite: 130] */
    while (i < tam && tabela[i].chave <= chave) i++;

    if (i == 0) return NULL; /* Chave menor que a primeira do arquivo [cite: 133] */

    FILE *arq = fopen(nomeArquivo, "rb");
    Registro *pagina = malloc(ITENSPAGINA * sizeof(Registro));
    
    /* Posiciona o ponteiro no inicio da pagina selecionada [cite: 147, 148] */
    long desloc = (long)(tabela[i-1].posicao - 1) * ITENSPAGINA * sizeof(Registro);
    fseek(arq, desloc, SEEK_SET);
    int lidos = fread(pagina, sizeof(Registro), ITENSPAGINA, arq);

    /* Pesquisa sequencial dentro da pagina carregada [cite: 151, 152] */
    for (int j = 0; j < lidos; j++) {
        if (pagina[j].chave == chave) {
            Registro *res = malloc(sizeof(Registro));
            *res = pagina[j];
            fclose(arq);
            free(pagina);
            return res;
        }
    }

    fclose(arq);
    free(pagina);
    return NULL;
}
