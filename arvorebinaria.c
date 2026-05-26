#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvorebinaria.h"

// Cria o arquivo da arvore e joga uma raiz provisoria para inicializar
// Usamos a chave -1 para indicar que a posicao esta vazia
void criarArvoreBinaria(const char *nomeArquivoArvore){
    FILE *arquivo = fopen(nomeArquivoArvore, "wb");
    if (arquivo == NULL) return;

    NoArquivo raizVazia;
    raizVazia.registro.chave = -1;
    // Como a arvore fica no disco, usamos posicoes (offsets) em vez de ponteiros de memoria
    raizVazia.esquerda = -1; 
    raizVazia.direita = -1;

    fwrite(&raizVazia, sizeof(NoArquivo), 1, arquivo);
    fclose(arquivo);
}

// Funcao auxiliar para carregar um no especifico para a RAM usando fseek
NoArquivo lerNo(FILE *arquivo, long posicao){
    NoArquivo no;
    fseek(arquivo, posicao * sizeof(NoArquivo), SEEK_SET); // faz a multiplicação pra achar o no especifico 
    fread(&no, sizeof(NoArquivo), 1, arquivo);
    return no;
}

// Salva o no de volta no arquivo fisico na posicao correspondente
void escreveNo(FILE *arquivo, long posicao, NoArquivo no){
    fseek(arquivo, posicao * sizeof(NoArquivo), SEEK_SET);
    fwrite(&no, sizeof(NoArquivo), 1, arquivo);
}

// Insercao recursiva. 
//  se o arquivo estiver ordenado, a arvore degenera para uma lista e causa 
// Stack Overflow por causa do limite de recursao do sistema operacional
long inserirRecursivo(FILE * arquivo, long posicao, Registro reg, long *comp){
    // Achou um espaco livre (-1), entao cria o novo no aqui.
    if(posicao == -1){
        NoArquivo novoNo;
        novoNo.registro = reg;
        novoNo.esquerda = -1;
        novoNo.direita = -1;

        // Vai pro final do arquivo para salvar sem sobrescrever os nos anteriores
        fseek(arquivo, 0, SEEK_END);
        long novaPosicao = ftell(arquivo) / sizeof(NoArquivo);
        fwrite(&novoNo, sizeof(NoArquivo), 1, arquivo);
        return novaPosicao;
    }

    NoArquivo noAtual = lerNo(arquivo, posicao);
    (*comp)++; // Conta uma comparacao de chave.

    // Logica basica da arvore: maior vai pra direita, menor pra esquerda
    if(reg.chave > noAtual.registro.chave){
        noAtual.direita = inserirRecursivo(arquivo, noAtual.direita, reg, comp);
        escreveNo(arquivo, posicao, noAtual);
    }
    else if(reg.chave < noAtual.registro.chave){
        noAtual.esquerda = inserirRecursivo(arquivo, noAtual.esquerda, reg, comp);
        escreveNo(arquivo, posicao, noAtual);
    }
    return posicao;
}

// Funcao que gerencia a insercao, lidando com o caso inicial de arvore vazia
void inserirEmArquivo(const char *nomeArquivoArvore, Registro reg, long *comp){
    FILE *arquivo = fopen(nomeArquivoArvore, "r+b");
    if(arquivo == NULL) return;

    NoArquivo raiz = lerNo(arquivo, 0);

    // Se for o primeiro registro do arquivo, substitui a raiz provisoria
    if(raiz.registro.chave == -1){
        NoArquivo novoNo;
        novoNo.registro = reg;
        novoNo.esquerda = -1;
        novoNo.direita = -1;
        escreveNo(arquivo, 0, novoNo);
    }
    else {
        inserirRecursivo(arquivo, 0, reg, comp);
    }
    fclose(arquivo);
}

// Rotina de busca. Cada iteracao do while faz um fread no disco, 
// o que torna o I/O mais pesado do que nas Arvores B
Registro* buscarEmArquivo(const char *nomeArquivoArvore, int chave, long *comp, long *transferencias){
    FILE *arq = fopen(nomeArquivoArvore, "rb");
    if (!arq) return NULL;

    NoArquivo no;
    long posicao = 0; // A raiz sempre fica na posicao 0
    *comp = 0;
    *transferencias = 0;

    while (posicao != -1){
        fseek(arq, posicao * sizeof(NoArquivo), SEEK_SET);
        if (fread(&no, sizeof(NoArquivo), 1, arq) != 1){
            fclose(arq);
            return NULL;
        }

        (*transferencias)++; 
        (*comp)++; 

        if (chave == no.registro.chave){
            Registro *regRetorno = malloc(sizeof(Registro));
            *regRetorno = no.registro;
            fclose(arq);
            return regRetorno;
        }

        (*comp)++;
        if (chave < no.registro.chave) posicao = no.esquerda;
        else posicao = no.direita;
    }
    fclose(arq);
    return NULL;                   
}

// Le os dados do arquivo principal e ja monta a arvore, cronometrando o tempo
void lerArquivoBinario(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros, long *transferencias, long *comp, double *tempo){
    FILE *arquivo = fopen(nomeArquivoDados, "rb");
    if(arquivo == NULL) return;

    Registro reg;
    int registrosLidos = 0;
    criarArvoreBinaria(nomeArquivoArvore);
    *transferencias = 0;
    *comp = 0;

    double inicio = now_seconds();
    while(registrosLidos < numRegistros && fread(&reg, sizeof(Registro), 1, arquivo) == 1){
        (*transferencias)++;
        long compInsercao = 0;
        inserirEmArquivo(nomeArquivoArvore, reg, &compInsercao);
        *comp += compInsercao;
        registrosLidos++;
    }
    double fim = now_seconds();
    *tempo = ((double)(fim - inicio));
    fclose(arquivo);
}

// FASE 2: Teste de desempenho com 10 buscas aleatorias
void pesquisar10AleatoriasAB(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros){
    FILE *arquivo;
    Registro reg;
    long transTotal = 0, compTotal = 0;
    double tempoTotalPesquisa = 0;

    printf("\n=== FASE 2: Pesquisando 10 chaves aleatorias (Arvore Binaria) ===\n");
    srand(time(NULL));

    for(int i = 0; i < 10; i++){
        // Busca uma chave que realmente existe no arquivo original para evitar falhas nos testes
        int posicao = rand() % numRegistros;
        arquivo = fopen(nomeArquivoDados, "rb");
        fseek(arquivo, posicao * sizeof(Registro), SEEK_SET);
        fread(&reg, sizeof(Registro), 1, arquivo);
        fclose(arquivo);

        long comp = 0, transf = 0;
        double inicioBusca = now_seconds();
        Registro *resultado = buscarEmArquivo(nomeArquivoArvore, reg.chave, &comp, &transf);
        double fimBusca = now_seconds();

        double tempoBusca = fimBusca - inicioBusca;
        compTotal += comp;
        transTotal += transf;
        tempoTotalPesquisa += tempoBusca;

        if(resultado != NULL){
            printf("Chave %d | Encontrada | transf: %ld | comp: %ld | tempo: %.4f s\n", reg.chave, transf, comp, tempoBusca);
            free(resultado); // Evita memory leak.
        } else {
            printf("Chave %d | Nao encontrada | transf: %ld | comp: %ld | tempo: %.4f s\n", reg.chave, transf, comp, tempoBusca);
        }
    }

    printf("\n=== RESULTADOS MEDIOS DA PESQUISA (FASE 2) ===\n");
    printf("Media de transferencias: %ld\n", transTotal / 10);
    printf("Media de comparacoes: %ld\n", compTotal / 10);
    printf("Tempo medio de pesquisa: %.6f s\n", tempoTotalPesquisa / 10.0);
}

// Interface principal que gerencia o que executar dependendo dos argumentos do terminal
void executarArvoreBinaria(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves) {
    const char *arquivoArvore = "arvore_binaria.bin";
    long transferencias = 0, comp = 0;
    double tempoCriacao = 0, tempoPesquisa = 0;

    // Argumento [-P]: le e imprime as chaves aos poucos para nao gastar muita RAM.
    if (imprimirChaves) {
        printf("\nChaves do arquivo:\n");
        FILE *arquivo = fopen(nomeArquivo, "rb");
        Registro reg;
        int count = 0;
        while (count < quantidade && fread(&reg, sizeof(Registro), 1, arquivo) == 1) {
            printf("%d ", reg.chave);
            if ((count + 1) % 10 == 0) printf("\n");
            count++;
        }
        printf("\n");
        fclose(arquivo);
    }

    printf("Construindo Arvore Binaria...\n");
    lerArquivoBinario(nomeArquivo, arquivoArvore, quantidade, &transferencias, &comp, &tempoCriacao);

    // Se o usuario passou -t, faz os testes da Fase 2 e sai
    if (modoTeste) {
        printf("Tempo de criacao da arvore: %.6f s\n", tempoCriacao);
        pesquisar10AleatoriasAB(nomeArquivo, arquivoArvore, quantidade);
        return; 
    }

    // FASE 1: Fluxo normal pesquisando a chave especifica.
    printf("\n=== FASE 1: Pesquisando chave %d ===\n", chave);
    long compBusca = 0, transfBusca = 0;
    
    double inicio = now_seconds();
    Registro *resultado = buscarEmArquivo(arquivoArvore, chave, &compBusca, &transfBusca);
    double fim = now_seconds();

    tempoPesquisa = fim - inicio;

    if (resultado != NULL) {
        printf(">>> CHAVE ENCONTRADA <<<\n");
        printf("Chave: %d | Dado1: %ld\n", resultado->chave, resultado->dado1);
        printf("Dado2: %.50s...\n", resultado->dado2); 
        printf("Dado3: %.50s...\n", resultado->dado3); 
        free(resultado);
    } else {
        printf(">>> CHAVE NAO ENCONTRADA <<<\n");
    }

    printf("\n--- ESTATISTICAS ---\n");
    printf("Transferencias (leitura): %ld\n", transfBusca);
    printf("Comparacoes entre chaves: %ld\n", compBusca);
    printf("Tempo de criacao: %.6f s | Tempo de pesquisa: %.6f s\n", tempoCriacao, tempoPesquisa);
}
