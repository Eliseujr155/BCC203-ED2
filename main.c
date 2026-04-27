#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "acessoSequencial.h"
#include "arvoreb.h"
#include "arvorebestrela.h"

int main(int argc, char *argv[]) {
    // Validação básica dos argumentos de entrada
    if (argc < 5) {
        printf("Faltam argumentos. Use: pesquisa <metodo> <quantidade> <situacao> <chave>\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int chaveBusca = atoi(argv[4]);

    // Lógica para decidir qual arquivo binário abrir
    char nomeArquivo[50];
    if (situacao == 1) strcpy(nomeArquivo, "registros_asc.bin");
    else if (situacao == 2) strcpy(nomeArquivo, "registros_desc.bin");
    else strcpy(nomeArquivo, "registros_mix.bin");

    // Switch para disparar o método escolhido pelo usuário
    switch (metodo) {
        case 1: { 
            tipoindice tabela[1000]; // Ajustar tamanho conforme a quantidade
            int tam = criarIndicePaginas(nomeArquivo, tabela, quantidade);
            Registro *r = pesquisaSequencial(nomeArquivo, chaveBusca, tabela, tam);
            if (r) printf("Achou chave %d. Dado1: %ld\n", r->chave, r->dado1);
            else printf("Chave nao encontrada.\n");
            break;
        }
        case 3: {
            // Exemplo simplificado de chamada para Arvore B
            TipoApontador raiz = NULL; 
            // Aqui entraria a função de carregar do arquivo para a árvore
            Registro x; x.chave = chaveBusca;
            Pesquisa(&x, raiz);
            if (x.chave != -1) printf("Chave encontrada na Arvore B.\n");
            break;
        }
        // ... outros casos seguindo a mesma lógica
    }

    return 0;
}
