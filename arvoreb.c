#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvoreb.h"

Registro* pesquisa(Pagina *pagina, int chave, long *comp) {
    int i;
    while (pagina != NULL) {
        i = 0;
        while (i < pagina->n && chave > pagina->registro[i].chave) {
            i++;
            (*comp)++;
        }
        (*comp)++;
        if (i < pagina->n && chave == pagina->registro[i].chave) {
            return &pagina->registro[i]; //achou
        }
        pagina = pagina->filhos[i]; //filho correspondente continua no while
    }
    return NULL; // chave não existe na arvore
}

void InsereNaPagina(Pagina *ap, Registro Reg, Pagina *apDir){
    int k = ap->n;
    while (k > 0) {
        if (Reg.chave >= ap->registro[k-1].chave) break; 
        ap->registro[k] = ap->registro[k - 1]; //maior registro é passado pra frente
        ap->filhos[k + 1] = ap->filhos[k]; // filho direito do maior registro é passado pra frente
        k--; // decrementa pra verificar posição anterior
    }
    ap->registro[k] = Reg; //colocado na frente do registro em que foi maior
    ap->filhos[k + 1] = apDir; //filho acompanha
    ap->n++;
}

void Ins(Registro reg, Pagina *ap, short *cresceu, Registro *regRetorno, Pagina **apRetorno, long *comp){
    int i = 0, j;
    Pagina *apTemp;

    if (ap == NULL){
        *cresceu = TRUE;
        *regRetorno = reg;
        *apRetorno = NULL;
        return;
    }

    while (i < ap->n && reg.chave > ap->registro[i].chave){ //compara registros
        (*comp)++;
        i++;
    }

    if (i < ap->n && reg.chave == ap->registro[i].chave) {
        (*comp)++;
        *cresceu = FALSE;
        return; //se chave já existe na arvore, apenas retorna sem adicionar nada
    }

    Ins(reg, ap->filhos[i], cresceu, regRetorno, apRetorno, comp); //entra no filho do registro correspondente a sua iteração

    if (!*cresceu) return; //garante que a função termine caso cresceu falso na chamada recursiva

    if(ap->n < MM){
        InsereNaPagina(ap, *regRetorno, *apRetorno); //caso ainda caiba na página apenas insere o registro nela
        *cresceu = FALSE;
        return;
    }

    apTemp = (Pagina *) malloc(sizeof(Pagina)); //cria página temporária
    apTemp->n = 0;
    apTemp->filhos[0] = NULL;

    if (i < ORDEM + 1){
        InsereNaPagina(apTemp, ap->registro[MM - 1], ap->filhos[MM]); //move maior registro pra nova página para liberar espaço na página original
        ap->n--; //diminui n da página original pois um registro foi removido
        InsereNaPagina(ap, *regRetorno, *apRetorno); //coloca o novo registro na página original
    } else {
        InsereNaPagina(apTemp, *regRetorno, *apRetorno); //pega o novo registro e coloca na página nova
    }

    for (j = ORDEM + 1; j < MM; j++) {
        InsereNaPagina(apTemp, ap->registro[j], ap->filhos[j + 1]); //move todos os registro maiores que m+1 na nova página
    }

    ap->n = ORDEM; //corta página pela metade
    apTemp->filhos[0] = ap->filhos[ORDEM + 1]; //ponteiro direito do registro que sobe vira o primeiro ponteiro da nova página
    *regRetorno = ap->registro[ORDEM]; //aponta que registro do meio deve subir pro nó pai
    *apRetorno  = apTemp; //aponta página que foi gerada
}

void Insere(Registro reg, Pagina **ap, long *comp) {
    short cresceu;
    Registro regRetorno;
    Pagina *apRetorno;
    Pagina *apTemp;

    Ins(reg, *ap, &cresceu, &regRetorno, &apRetorno, comp);
// caso a variável cresceu voltar como verdadeira, o ultimo split feito está retornando um reg retorno que não tem pra onde ir
// deve ser criado nova raiz
    if (cresceu) {
        apTemp = (Pagina *) malloc(sizeof(Pagina));
        apTemp->n = 1;
        apTemp->registro[0] = regRetorno;
        apTemp->filhos[0] = *ap;
        apTemp->filhos[1] = apRetorno;
        *ap = apTemp;
    }
}

void lerArquivoArvoreB(const char *nomeArquivo, int numRegistros, Pagina **raiz, long *transferencias, long *comp, double *tempo) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) return;
    
    Registro reg;
    // inicializa estatisticas
    int registrosLidos = 0;
    *transferencias = 0;
    *comp = 0;
    *raiz = NULL;
    
    double inicio = now_seconds();
    // lê registros enquanto for menor que número pedido
    while (registrosLidos < numRegistros && fread(&reg, sizeof(Registro), 1, arquivo) == 1) {
        (*transferencias)++;
        Insere(reg, raiz, comp); // insere na arvore cada registro lido
        registrosLidos++;
    }
    double fim = now_seconds();
    *tempo = ((double)(fim - inicio));
    fclose(arquivo);
}

void pesquisar10AleatoriasB(const char *nomeArquivo, int numRegistros, Pagina *raiz) {
    FILE *arquivo;
    Registro reg;
    long compTotal = 0, transferencias = 0;
    double tempoTotal = 0;
    
    printf("\n=== FASE 2: Pesquisando 10 chaves aleatorias (Arvore B) ===\n");
    srand(time(NULL));
    
    for (int i = 0; i < 10; i++) {
        int posicao = rand() % numRegistros;
        arquivo = fopen(nomeArquivo, "rb");
        fseek(arquivo, posicao * sizeof(Registro), SEEK_SET);
        fread(&reg, sizeof(Registro), 1, arquivo);
        transferencias++; 
        fclose(arquivo);
        
        long comp = 0;
        double inicio = now_seconds();
        Registro *resultado = pesquisa(raiz, reg.chave, &comp); //busca chave na arvore
        double fim = now_seconds();
        
        compTotal += comp;
        tempoTotal += (fim - inicio);
        
        if (resultado != NULL) {
            printf("Chave %d | Encontrada | comps: %ld | tempo: %.6f s\n", reg.chave, comp, fim - inicio);
        } else {
            printf("Chave %d | Nao encontrada | comps: %ld | tempo: %.6f s\n", reg.chave, comp, fim - inicio);
        }
    }
    
    printf("\n=== RESULTADOS MEDIOS DA PESQUISA (FASE 2) ===\n");
    printf("Media de transferencias p/ pegar chave: %ld\n", transferencias / 10); 
    printf("Media de comparacoes: %ld\n", compTotal / 10);
    printf("Tempo medio de pesquisa: %.6f s\n", tempoTotal / 10.0);
}

void executarArvoreB(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves) {
    Pagina *raiz = NULL;
    long transferencias = 0, compConstrucao = 0;
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

    printf("Construindo Arvore B...\n");
    lerArquivoArvoreB(nomeArquivo, quantidade, &raiz, &transferencias, &compConstrucao, &tempoConstrucao);

    if (modoTeste) {
        printf("Tempo de construcao da arvore: %.6f s | Transf: %ld\n", tempoConstrucao, transferencias);
        pesquisar10AleatoriasB(nomeArquivo, quantidade, raiz);
        return;
    }

    printf("\n=== FASE 1: Pesquisando chave %d ===\n", chave);
    long compBusca = 0;
    
    double inicio = now_seconds();
    Registro *resultado = pesquisa(raiz, chave, &compBusca);
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
    printf("Comparacoes na busca: %ld\n", compBusca);
    printf("Tempo de construcao: %.6f s | Tempo de pesquisa: %.6f s\n", tempoConstrucao, tempoBusca);
}
