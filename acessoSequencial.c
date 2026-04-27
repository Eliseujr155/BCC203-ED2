#include <stdio.h>
#include <stdlib.h>
#include "acessoSequencial.h"

// Função que varre o arquivo original para montar a tabela de índices na memória RAM
int criarIndicePaginas(const char *nomeArquivo, tipoindice tabela[], int numRegistros) {
    FILE *arq = fopen(nomeArquivo, "rb");
    if (!arq) return 0;
    
    int pos = 0, cont = 0;
    Registro x;
    
    // Lê o registro e, se for o primeiro de uma página, guarda a chave e a posição no índice
    while (fread(&x, sizeof(Registro), 1, arq) == 1 && cont < numRegistros) {
        if (cont % ITENSPAGINA == 0) {
            tabela[pos].chave = x.chave;
            tabela[pos].posicao = pos + 1;
            pos++;
        }
        cont++;
    }
    fclose(arq);
    return pos; // Retorna quantas páginas foram indexadas
}

// Faz a busca em duas etapas: primeiro no índice em RAM, depois no bloco específico no disco
Registro* pesquisaSequencial(const char *nomeArquivo, int chave, tipoindice tabela[], int tam) {
    int i = 0;
    
    // Procura no índice qual página pode conter a nossa chave
    while (i < tam && tabela[i].chave <= chave) i++;

    if (i == 0) return NULL; // A chave é menor que a primeira chave do arquivo

    FILE *arq = fopen(nomeArquivo, "rb");
    Registro *pagina = malloc(ITENSPAGINA * sizeof(Registro));
    
    // Pula direto para o local da página no arquivo binário usando o deslocamento
    long desloc = (long)(tabela[i-1].posicao - 1) * ITENSPAGINA * sizeof(Registro);
    fseek(arq, desloc, SEEK_SET);
    int lidos = fread(pagina, sizeof(Registro), ITENSPAGINA, arq);

    // Agora faz a busca sequencial apenas dentro daqueles 4 registros da página
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
