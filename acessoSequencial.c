#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "acessoSequencial.h"


int criarIndicePaginas(const char *nomeArquivo, TipoIndice tabela[],  int numRegistros, long *transferencias, double *tempoCriacao) {
    double inicio = now_seconds();
    
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return 0; // Se der erro na leitura, aborta
    
    *transferencias = 0;
    // Truque para arredondar a divisao inteira pra cima
    int numPaginas = (numRegistros + ITENSPAGINA - 1) / ITENSPAGINA;
    TipoItem reg;
    
    for (int i = 0; i < numPaginas; i++) {
        // Pula direto para o inicio de cada pagina no arquivo binario
        long posArquivo = (long)i * ITENSPAGINA * sizeof(TipoItem);
        fseek(arquivo, posArquivo, SEEK_SET);
        
        // Le apenas o primeiro registro da pagina para pegar a chave
        if (fread(&reg, sizeof(TipoItem), 1, arquivo) == 1) {
            tabela[i].posicao = i;           
            tabela[i].chave = reg.chave;     
            (*transferencias)++; // Cada leitura no disco conta como 1 transferencia 
        }
    }
    
    fclose(arquivo);
    double fim = now_seconds();
    *tempoCriacao = (fim - inicio); // Guarda o tempo total da operacao
    
    return numPaginas;
}

/*
  Busca binaria/sequencial APENAS na memoria RAM 
  Nao conta como transferencia de disco, mas conta comparacoes
 */
int buscarPaginaNoIndice(int chave, TipoIndice tabela[], int numPaginas, long *comp) {
    *comp = 0; 
    int paginaAlvo = -1; //não achou nada ainda
    
    for (int i = 0; i < numPaginas; i++) { 
        (*comp)++; //Começa a varrer o índice na RAM e incrementa a contagem de comparações

        if (i == numPaginas - 1 || chave < tabela[i + 1].chave) { //verifica se chegamos na última página ou se a chave procurada é menor que o início da próxima
            if (chave >= tabela[i].chave) { // verifica se não e menor que inicio da pag a atual
                paginaAlvo = tabela[i].posicao; //salva posição
            }
            break; // Achou a pagina possivel, para de procurar
        }
    }
    return paginaAlvo;
}

//Puxa uma pagina inteira do HD/SSD para a memoria principal.
int carregarPagina(const char *nomeArquivo, int numPagina, PaginaAS *paginaAlvo, int numRegistros, long *transferencias) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return 0;
    
    // Posiciona o ponteiro no byte exato onde a pagina comeca
    long posArquivo = (long)numPagina * ITENSPAGINA * sizeof(TipoItem);
    fseek(arquivo, posArquivo, SEEK_SET); // fazendo um salto preciso indo direto pra pagina correta 
    
    paginaAlvo->numItens = 0;
    TipoItem item;
    
    // Le todos os 4 itens que cabem na pagina 
    for (int i = 0; i < ITENSPAGINA; i++) {
        int posRegistro = numPagina * ITENSPAGINA + i;
        if (posRegistro >= numRegistros) break; // Protecao para a ultima pagina que pode estar incompleta evitando ler lixo de memoria 
        
        if (fread(&item, sizeof(TipoItem), 1, arquivo) == 1) { //vai tudp pra paginaAlvo
            paginaAlvo->itens[paginaAlvo->numItens++] = item;
            (*transferencias)++;  // Contabiliza a leitura fisica 
        }
    }
    
    fclose(arquivo);
    return paginaAlvo->numItens > 0;
}

//Procura a chave no array que ja esta carregado na struct PaginaAS 
int buscarNaPagina(int chave, PaginaAS *pag, long *comp, TipoItem *resultado) {
    for (int i = 0; i < pag->numItens; i++) {
        (*comp)++; 
        if (pag->itens[i].chave == chave) {
            *resultado = pag->itens[i]; // Copia o registro todo pra variavel de retorno
            return 1; // Sucesso
        }
    }
    return 0; // sea chave nao estiver aqui
}

/*
  Funcao orquestradora: junta o indice, carrega a pagina e faz a busca.
  Tambem gerencia a alocacao do indice caso ele nao venha pronto.
 */
void lerArquivoSequencial(const char *nomeArquivo, int quantidade, int chave, long *transferencias, long *comp, double *tempo, TipoItem *resultado, int *encontrado, TipoIndice *tabelaIndice, int numPaginas) {
    double inicio = now_seconds();

    //preparando ponteiros locais para trabalhar com o índice
    *transferencias = 0;
    *comp = 0;
    *encontrado = 0;
    
    TipoIndice *indiceLocal = tabelaIndice;
    int paginasLocal = numPaginas;
    int criadoAgora = 0; // Flag para saber se precisamos dar free() depois
    
    // Se nao passaram um indice pronto (Fase 1), a gente cria um agora
    if (indiceLocal == NULL) {
        paginasLocal = (quantidade + ITENSPAGINA - 1) / ITENSPAGINA; 
        indiceLocal = (TipoIndice *)malloc(paginasLocal * sizeof(TipoIndice));
        if (!indiceLocal) return;
        
        long transfIndice = 0;
        double tempoCriacaoIndice = 0;
        paginasLocal = criarIndicePaginas(nomeArquivo, indiceLocal, quantidade, &transfIndice, &tempoCriacaoIndice);
        if (paginasLocal == 0) {
            free(indiceLocal);
            return;
        }
        *transferencias += transfIndice;
        criadoAgora = 1;
    }
    
    // 1. Acha a pagina no indice
    long compIndice = 0;
    int numPaginaAlvo = buscarPaginaNoIndice(chave, indiceLocal, paginasLocal, &compIndice);
    *comp += compIndice;
    
    // 2. Se a pagina for valida, carrega pro disco e procura
    if (numPaginaAlvo >= 0) {
        PaginaAS paginaAlvo;
        long transfPagina = 0;
        
        if (carregarPagina(nomeArquivo, numPaginaAlvo, &paginaAlvo, quantidade, &transfPagina)) {
            *transferencias += transfPagina;
            
            long compPagina = 0;
            *encontrado = buscarNaPagina(chave, &paginaAlvo, &compPagina, resultado);
            *comp += compPagina;
        }
    }
    
    if (criadoAgora) free(indiceLocal); // Limpa a memoria pra nao dar vazamento 
    
    double fim = now_seconds();
    *tempo = ((double)(fim - inicio));
}

//FUNÇÃO DA FASE 2
void pesquisar10AleatoriasSI(const char *nomeArquivo, int quantidade) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return;

    // Sorteia 10 posicoes aleatorias reais do arquivo pra garantir que as chaves existem 
    int chavesTeste[10];
    srand(time(NULL)); // Semente pro gerador de aleatorios
    for (int i = 0; i < 10; i++) {
        int posAleatoria = rand() % quantidade;
        TipoItem itemTemp;
        fseek(arquivo, posAleatoria * sizeof(TipoItem), SEEK_SET);
        fread(&itemTemp, sizeof(TipoItem), 1, arquivo);
        chavesTeste[i] = itemTemp.chave;
    }
    fclose(arquivo);

    // Cria o indice UMA UNICA VEZ pra nao penalizar os testes de pesquisa
    int numPaginas = (quantidade + ITENSPAGINA - 1) / ITENSPAGINA;
    TipoIndice *tabelaIndice = (TipoIndice *)malloc(numPaginas * sizeof(TipoIndice));
    
    long transfIndice = 0;
    double tempoCriacaoIndice = 0;
    numPaginas = criarIndicePaginas(nomeArquivo, tabelaIndice, quantidade, &transfIndice, &tempoCriacaoIndice);
    
    long compTotal = 0, transfTotal = 0;
    double tempoPesquisaTotal = 0.0;

    printf("\n=== FASE 2: Pesquisando 10 chaves aleatorias ===\n");
    for (int i = 0; i < 10; i++) {
        long transf = 0, comp = 0;
        double tempo = 0;
        TipoItem resultado;
        int encontrado = 0;

        lerArquivoSequencial(nomeArquivo, quantidade, chavesTeste[i], &transf, &comp, &tempo, &resultado, &encontrado, tabelaIndice, numPaginas);

        if (encontrado) printf("Chave: %d | Encontrada | transf: %ld | comp: %ld | tempo: %.4f s\n", chavesTeste[i], transf, comp, tempo);
        
        // Acumula os valores para tirar a media depois 
        compTotal += comp;
        transfTotal += transf;
        tempoPesquisaTotal += tempo;
    }

    printf("\n=== RESULTADOS MEDIOS DA PESQUISA (FASE 2) ===\n");
    printf("Media de transferencias: %ld\n", transfTotal / 10);
    printf("Media de comparacoes: %ld\n", compTotal / 10);
    printf("Tempo medio de pesquisa: %.6f s\n", tempoPesquisaTotal / 10.0);
    printf("Tempo total de criacao do indice: %.6f s\n", tempoCriacaoIndice);

    free(tabelaIndice);
}

/*
 * Interface chamada pela main. Trata os argumentos passados pelo console.
 */
void executarSequencial(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves) {
    if (modoTeste) {
        pesquisar10AleatoriasSI(nomeArquivo, quantidade);
        return;
    }

    // Se o usuario passou o [-P] no terminal, imprime as chaves primeiro 
    if (imprimirChaves) {
        printf("\nChaves do arquivo:\n");
        FILE *arquivo = fopen(nomeArquivo, "rb");
        TipoItem item;
        int count = 0;
        // Le e imprime sem carregar tudo na memoria de uma vez
        while (count < quantidade && fread(&item, sizeof(TipoItem), 1, arquivo) == 1) {
            printf("%d ", item.chave);
            if ((count + 1) % 10 == 0) printf("\n"); // Quebra linha 
            count++;
        }
        printf("\n");
        fclose(arquivo);
    }

    printf("\n=== FASE 1: Pesquisando chave %d ===\n", chave);
    long transferencias = 0, comp = 0;
    double tempo = 0;
    TipoItem resultado;
    int encontrado = 0;

    // Executa a busca passando NULL no indice para forcar a criacao dele
    lerArquivoSequencial(nomeArquivo, quantidade, chave, &transferencias, &comp, &tempo, &resultado, &encontrado, NULL, 0);

    // Se achou, printa o registro completo exigido pelo enunciado 
    if (encontrado) {
        printf(">>> CHAVE ENCONTRADA <<<\n");
        printf("Chave: %d | Dado1: %ld\n", resultado.chave, resultado.dado1);
        printf("Dado2: %.50s...\n", resultado.dado2);
        printf("Dado3: %.50s...\n", resultado.dado3);
    } else {
        printf(">>> CHAVE NAO ENCONTRADA <<<\n");
    }
    printf("Transferencias (leitura): %ld | Comparacoes: %ld | Tempo: %.6f s\n", transferencias, comp, tempo);
}
