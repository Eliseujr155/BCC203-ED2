#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "registro.h"
#include "acessoSequencial.h"
#include "arvorebinaria.h"
#include "arvoreb.h"
#include "arvorebestrela.h"

int main(int argc, char *argv[]) {
    // Verifica se os argumentos mínimos foram passados
    if (argc < 5) {
        printf("Uso: %s <metodo> <quantidade> <situacao> <chave> [-P]\n", argv[0]);
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int chaveProcurada = atoi(argv[4]);
    int mostrarChaves = (argc == 6 && strcmp(argv[5], "-P") == 0);

    // Define o nome do arquivo com base na situação
    char nomeArquivo[50];
    if (situacao == 1) strcpy(nomeArquivo, "registros_asc.bin");
    else if (situacao == 2) strcpy(nomeArquivo, "registros_desc.bin");
    else strcpy(nomeArquivo, "registros_mix.bin");

    // Implementação da chamada dos métodos conforme o parágrafo da Fase 1
    switch (metodo) {
        case 1: { // Acesso Sequencial Indexado
            tipoindice tabela[100]; // MAXTABELA
            int tamIndice = criarIndicePaginas(nomeArquivo, tabela, quantidade);
            Registro *res = pesquisaSequencial(nomeArquivo, chaveProcurada, tabela, tamIndice);
            
            if (res) {
                printf("Chave %d encontrada!\n", res->chave);
                printf("Dados: %ld, %s, %s\n", res->dado1, res->dado2, res->dado3);
                free(res);
            } else printf("Chave nao encontrada.\n");
            break;
        }

        case 2: { // Arvore Binaria Externa
            Registro *res = buscarBinaria("arvore_binaria.bin", chaveProcurada);
            if (res) {
                printf("Chave %d encontrada!\n", res->chave);
                free(res);
            } else printf("Chave nao encontrada.\n");
            break;
        }

        case 3: { // Arvore B
            TipoApontador raiz = NULL;
            // Aqui deve vir a lógica de carga/inicialização da árvore
            Registro x;
            x.chave = chaveProcurada;
            Pesquisa(&x, raiz);
            if (x.chave != -1) printf("Chave encontrada!\n");
            else printf("Chave nao encontrada.\n");
            break;
        }

        case 4: { // Arvore B*
            TipoApontadorEstrela raizEstrela = NULL;
            Registro x;
            x.chave = chaveProcurada;
            PesquisaEstrela(&x, &raizEstrela);
            if (x.chave != -1) printf("Chave encontrada!\n");
            else printf("Chave nao encontrada.\n");
            break;
        }
    }

    return 0;
}
