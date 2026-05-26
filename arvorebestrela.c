#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvorebestrela.h" 

void insereNaPaginaInterna(PaginaEstrela *pagina, int chave, PaginaEstrela *paginaDireita) {
    int k = pagina->conteudo.interna.numChaves;
    while (k > 0) {
        if (chave >= pagina->conteudo.interna.chaves[k - 1]) break; // chave é maior que maior da página
        pagina->conteudo.interna.chaves[k] = pagina->conteudo.interna.chaves[k - 1]; // maior registro é passado pra frente
        pagina->conteudo.interna.filhos[k + 1] = pagina->conteudo.interna.filhos[k]; // filho direito do maior passa pra frente
        k--;   
    }
    pagina->conteudo.interna.chaves[k] = chave; // coloca chave na frente do antigo maior
    pagina->conteudo.interna.filhos[k + 1] = paginaDireita; // página acompanha, ficando na sua direita
    pagina->conteudo.interna.numChaves++; // aumenta qtd de chaves
}

void insereNaPaginaExterna(PaginaEstrela *pagina, Registro reg) {
    // mesma coisa da página interna mas sem filhos
    int k = pagina->conteudo.externa.numRegistros;
    while (k > 0) {
        if (reg.chave >= pagina->conteudo.externa.registros[k - 1].chave) break;
        pagina->conteudo.externa.registros[k] = pagina->conteudo.externa.registros[k - 1];
        k--;
    }
    pagina->conteudo.externa.registros[k] = reg;
    pagina->conteudo.externa.numRegistros++;
}

Registro* pesquisaBEstrela(PaginaEstrela *pagina, int chave, long *comparacoes) {
    int i;
    if(pagina == NULL) return NULL;

    if(pagina->tipo == Interna) {
        i = 1;
        //mesma forma da B, percorre enquanto não chegar no final e chave buscada maior que avaliada
        while (i <= pagina->conteudo.interna.numChaves && chave > pagina->conteudo.interna.chaves[i - 1]) {
            i++;
            (*comparacoes)++; 
        }
        (*comparacoes)++; 
        if (i <= pagina->conteudo.interna.numChaves && chave <= pagina->conteudo.interna.chaves[i-1]) {
            return pesquisaBEstrela(pagina->conteudo.interna.filhos[i-1], chave, comparacoes); // se menor ou igual entra no filho esquerdo
        } else {
            return pesquisaBEstrela(pagina->conteudo.interna.filhos[i], chave, comparacoes); // se não, entra no filho direito
        }
    } 
    else { // página externa
        i = 1;
        // compara com registros da página da mesma forma de antes
        while (i <= pagina->conteudo.externa.numRegistros && chave > pagina->conteudo.externa.registros[i - 1].chave) {
            i++;
            (*comparacoes)++; 
        }
        (*comparacoes)++; 
        if (i <= pagina->conteudo.externa.numRegistros && chave == pagina->conteudo.externa.registros[i - 1].chave) {
            return &pagina->conteudo.externa.registros[i - 1]; // achou
        }
        return NULL; // não achou
    }
}

void insBEstrela(Registro reg, PaginaEstrela *pagina, short *cresceu, Registro *regRetorno, PaginaEstrela **paginaRetorno, long *comparacoes) {
    long i = 1, j;
    PaginaEstrela *paginaTemp;

    if (pagina->tipo == Interna) {
        i = 0;
        while (i < pagina->conteudo.interna.numChaves && reg.chave > pagina->conteudo.interna.chaves[i]) {
            (*comparacoes)++;
            i++;
        }
        (*comparacoes)++;
        
        insBEstrela(reg, pagina->conteudo.interna.filhos[i], cresceu, regRetorno, paginaRetorno, comparacoes);
        if (!*cresceu) return;

        if (pagina->conteudo.interna.numChaves < MM_ESTRELA) {
            insereNaPaginaInterna(pagina, regRetorno->chave, *paginaRetorno);
            *cresceu = FALSO;
            return;
        }

        paginaTemp = (PaginaEstrela *) malloc(sizeof(PaginaEstrela));
        paginaTemp->tipo = Interna;
        paginaTemp->conteudo.interna.numChaves = 0;
        paginaTemp->conteudo.interna.filhos[0] = NULL;

        int chaveInserir = regRetorno->chave;
        PaginaEstrela *ponteiroInserir = *paginaRetorno;

        if (i < ORDEM_ESTRELA + 1) {
            insereNaPaginaInterna(paginaTemp, pagina->conteudo.interna.chaves[MM_ESTRELA - 1], pagina->conteudo.interna.filhos[MM_ESTRELA]);
            pagina->conteudo.interna.numChaves--;
            insereNaPaginaInterna(pagina, chaveInserir, ponteiroInserir);
        } else {
            insereNaPaginaInterna(paginaTemp, chaveInserir, ponteiroInserir);
        }

        for (j = ORDEM_ESTRELA + 1; j < MM_ESTRELA; j++) {
            insereNaPaginaInterna(paginaTemp, pagina->conteudo.interna.chaves[j], pagina->conteudo.interna.filhos[j + 1]);
        }

        pagina->conteudo.interna.numChaves = ORDEM_ESTRELA; 
        paginaTemp->conteudo.interna.filhos[0] = pagina->conteudo.interna.filhos[ORDEM_ESTRELA + 1]; 
        regRetorno->chave = pagina->conteudo.interna.chaves[ORDEM_ESTRELA];
        *paginaRetorno = paginaTemp;
        return; 
    }
    else {
        *regRetorno = reg;
        i = 0;
        while (i < pagina->conteudo.externa.numRegistros && reg.chave > pagina->conteudo.externa.registros[i].chave) {
            (*comparacoes)++;
            i++;
        }

        if (i < pagina->conteudo.externa.numRegistros && reg.chave == pagina->conteudo.externa.registros[i].chave) {
            (*comparacoes)++;
            *cresceu = FALSO; 
            return;
        }

        if (pagina->conteudo.externa.numRegistros < MM_ESTRELA) {
            insereNaPaginaExterna(pagina, reg);
            *cresceu = FALSO; 
            return;
        }

        paginaTemp = (PaginaEstrela *) malloc(sizeof(PaginaEstrela));
        paginaTemp->tipo = Externa;
        paginaTemp->conteudo.externa.numRegistros = 0;

        if (i < ORDEM_ESTRELA + 1) {
            insereNaPaginaExterna(paginaTemp, pagina->conteudo.externa.registros[MM_ESTRELA - 1]);
            pagina->conteudo.externa.numRegistros--;
            insereNaPaginaExterna(pagina, reg);
        } else {
            insereNaPaginaExterna(paginaTemp, reg);
        }

        for (j = ORDEM_ESTRELA + 1; j < MM_ESTRELA; j++) {
            insereNaPaginaExterna(paginaTemp, pagina->conteudo.externa.registros[j]);
        }

        pagina->conteudo.externa.numRegistros = ORDEM_ESTRELA + 1; 
        *regRetorno = pagina->conteudo.externa.registros[ORDEM_ESTRELA]; 
        *paginaRetorno = paginaTemp;
        *cresceu = VERDADEIRO; 
        return;
    }
}

void insereBEstrela(Registro reg, PaginaEstrela **raiz, long *comparacoes) {
    short cresceu = VERDADEIRO;
    Registro regRetorno;
    PaginaEstrela *paginaRetorno;
    PaginaEstrela *paginaTemp;

    if (*raiz == NULL) {
        paginaTemp = (PaginaEstrela *) malloc(sizeof(PaginaEstrela));
        paginaTemp->tipo = Externa;
        paginaTemp->conteudo.externa.numRegistros = 0;
        insereNaPaginaExterna(paginaTemp, reg);
        *raiz = paginaTemp;
        return;
    }

    insBEstrela(reg, *raiz, &cresceu, &regRetorno, &paginaRetorno, comparacoes);

    if (cresceu) {
        paginaTemp = (PaginaEstrela *) malloc(sizeof(PaginaEstrela));
        paginaTemp->tipo = Interna;
        paginaTemp->conteudo.interna.numChaves = 1;
        paginaTemp->conteudo.interna.chaves[0] = regRetorno.chave;
        paginaTemp->conteudo.interna.filhos[0] = *raiz;         
        paginaTemp->conteudo.interna.filhos[1] = paginaRetorno; 
        *raiz = paginaTemp; 
    }
}

void lerArquivoArvoreBEstrela(const char *nomeArquivo, int numRegistros, PaginaEstrela **raiz, long *transferencias, long *comparacoes, double *tempo) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) return;
    
    Registro reg;
    int registrosLidos = 0;
    *transferencias = 0;
    *comparacoes = 0;
    *raiz = NULL;
    
    double inicio = now_seconds();
    while (registrosLidos < numRegistros && fread(&reg, sizeof(Registro), 1, arquivo) == 1) {
        (*transferencias)++;
        insereBEstrela(reg, raiz, comparacoes); 
        registrosLidos++;
    }
    double fim = now_seconds();
    *tempo = (fim - inicio); 
    fclose(arquivo);
}

void pesquisar10AleatoriasBEstrela(const char *nomeArquivo, int numRegistros, PaginaEstrela *raiz) {
    FILE *arquivo;
    Registro reg;
    long comparacoesTotal = 0, transferencias = 0;
    double tempoTotal = 0;
    
    printf("\n=== FASE 2: Pesquisando 10 chaves aleatorias (B*) ===\n");
    srand(time(NULL));
    
    for (int i = 0; i < 10; i++) {
        int posicao = rand() % numRegistros;
        arquivo = fopen(nomeArquivo, "rb");
        fseek(arquivo, posicao * sizeof(Registro), SEEK_SET);
        fread(&reg, sizeof(Registro), 1, arquivo);
        transferencias++;
        fclose(arquivo);
        
        long comparacoes = 0;
        double inicio = now_seconds();
        Registro *resultado = pesquisaBEstrela(raiz, reg.chave, &comparacoes);
        double fim = now_seconds();

        comparacoesTotal += comparacoes;
        tempoTotal += (fim - inicio);
        
        if (resultado != NULL) {
            printf("Chave %d | Encontrada | Comps: %ld | Tempo: %.6f s\n", reg.chave, comparacoes, fim - inicio);
        } else {
            printf("Chave %d | Nao encontrada | Comps: %ld\n", reg.chave, comparacoes);
        }
    }
    
    printf("\n=== RESULTADOS MEDIOS DA PESQUISA (FASE 2) ===\n");
    printf("Media de transferencias p/ pegar chave: %ld\n", transferencias / 10); 
    printf("Media de comparacoes: %ld\n", comparacoesTotal / 10);
    printf("Tempo medio de pesquisa: %.6f s\n", tempoTotal / 10.0);
}

void executarArvoreBEstrela(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves) {
    PaginaEstrela *raiz = NULL;
    long transferencias = 0, comparacoesConstrucao = 0;
    double tempoConstrucao = 0;

    if (imprimirChaves) {
        printf("\nChaves do arquivo:\n");
        FILE *arquivo = fopen(nomeArquivo, "rb");
        Registro reg;
        int contador = 0;
        while (contador < quantidade && fread(&reg, sizeof(Registro), 1, arquivo) == 1) {
            printf("%d ", reg.chave);
            if ((contador + 1) % 10 == 0) printf("\n");
            contador++;
        }
        printf("\n");
        fclose(arquivo);
    }

    printf("Construindo Arvore B*...\n");
    lerArquivoArvoreBEstrela(nomeArquivo, quantidade, &raiz, &transferencias, &comparacoesConstrucao, &tempoConstrucao);

    if (modoTeste) {
        printf("Tempo de construcao: %.6f s | Transf: %ld\n", tempoConstrucao, transferencias);
        pesquisar10AleatoriasBEstrela(nomeArquivo, quantidade, raiz);
        return;
    }

    printf("\n=== FASE 1: Pesquisando chave %d ===\n", chave);
    long comparacoesBusca = 0;
    
    double inicio = now_seconds();
    Registro *resultado = pesquisaBEstrela(raiz, chave, &comparacoesBusca);
    double fim = now_seconds();
    
    double tempoBusca = fim - inicio;

    if (resultado != NULL) {
        printf(">>> CHAVE ENCONTRADA <<<\n");
        printf("Chave: %d | Dado1: %ld\n", resultado->chave, resultado->dado1);
        printf("Dado2: %.50s...\n", resultado->dado2);
        printf("Dado3: %.50s...\n", resultado->dado3);
    } else {
        printf(">>> CHAVE NAO ENCONTRADA <<<\n");
    }

    printf("\n--- ESTATISTICAS ---\n");
    printf("Transferencias de construcao: %ld\n", transferencias);
    printf("Comparacoes na busca: %ld\n", comparacoesBusca);
    printf("Tempo de construcao: %.6f s | Tempo de pesquisa: %.6f s\n", tempoConstrucao, tempoBusca);
}
