#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hamiltoniano.h"
#include "tps.h"

#define TESTE(c) do { if (!(c)) { \
    fprintf(stderr, "FALHA linha %d: %s\n", __LINE__, #c); \
    exit(EXIT_FAILURE); } } while (0)

static void validar_hamiltoniano(const Grafo *g, const int *rota, int ciclo) {
    int *visto = calloc((size_t)g->n, sizeof *visto);
    TESTE(visto != NULL);
    for (int i = 0; i < g->n; i++) {
        int v = rota[i];
        TESTE(v >= 0 && v < g->n && !visto[v]);
        visto[v] = 1;
        if (i > 0) TESTE(tem_aresta(g, rota[i - 1], v));
    }
    if (ciclo) {
        TESTE(rota[g->n] == rota[0]);
        TESTE(tem_aresta(g, rota[g->n - 1], rota[0]));
    }
    free(visto);
}

static void testar_hamiltoniano(void) {
    int rota[5];
    Grafo *g = criar_grafo(0);
    TESTE(!encontrar_caminho_hamiltoniano_bt(g, rota));
    liberar_grafo(g);
    g = criar_grafo(1);
    TESTE(encontrar_caminho_hamiltoniano_bt(g, rota) && rota[0] == 0);
    TESTE(!encontrar_ciclo_hamiltoniano_bt(g, rota));
    liberar_grafo(g);
    g = criar_grafo(3);
    inserir_aresta(g, 0, 1);
    inserir_aresta(g, 0, 2);
    TESTE(encontrar_caminho_hamiltoniano_bt(g, rota));
    TESTE(rota[0] != 0); /* Precisa tentar outras origens. */
    validar_hamiltoniano(g, rota, 0);
    TESTE(!encontrar_ciclo_hamiltoniano_bt(g, rota));
    liberar_grafo(g);
    g = criar_grafo(4);
    for (int v = 1; v < 4; v++) inserir_aresta(g, 0, v);
    TESTE(!encontrar_caminho_hamiltoniano_bt(g, rota)); /* Estrela. */
    TESTE(!encontrar_ciclo_hamiltoniano_bt(g, rota));
    liberar_grafo(g);
    puts("OK: buscas Hamiltonianas, varias origens e ausencia de solucao.");
}

static void conferir_tsp(const TSP *t, int origem,
                         long long otimo, long long vizinho, long long insercao) {
    int *rota = calloc((size_t)t->n + 1, sizeof *rota);
    TESTE(rota != NULL);
    unsigned long long folhas, fatorial = 1;
    for (int i = 2; i < t->n; i++) fatorial *= (unsigned int)i;
    TESTE(tsp_forca_bruta(t, origem, rota, &folhas) == otimo);
    TESTE(rota[0] == origem && custo_tour(t, rota) == otimo && folhas == fatorial);
    TESTE(tsp_vizinho_mais_proximo(t, origem, rota) == vizinho);
    TESTE(rota[0] == origem && custo_tour(t, rota) == vizinho && vizinho >= otimo);
    TESTE(tsp_insercao(t, origem, rota) == insercao);
    TESTE(rota[0] == origem && custo_tour(t, rota) == insercao && insercao >= otimo);
    free(rota);
}

static void testar_tsp(void) {
    const int classico[] = {0,10,15,20, 10,0,35,25, 15,35,0,30, 20,25,30,0};
    const int armadilha[] = {0,1,2,2, 1,0,2,2, 2,2,0,100, 2,2,100,0};
    const int unico[] = {0};
    const int grandes[] = {0,INT_MAX,INT_MAX, INT_MAX,0,INT_MAX, INT_MAX,INT_MAX,0};
    const int zeros[9] = {0};
    const int invalido[] = {0,1,2,0}; /* Assimetrico. */
    TSP *t = criar_tsp(4, classico);
    TESTE(t != NULL);
    conferir_tsp(t, 2, 80, 80, 80);
    int repetida[] = {0, 1, 1, 3, 0};
    TESTE(custo_tour(t, repetida) == -1);
    liberar_tsp(t);
    t = criar_tsp(4, armadilha);
    conferir_tsp(t, 0, 8, 105, 8); /* Heuristica nao garante otimalidade. */
    liberar_tsp(t);
    t = criar_tsp(1, unico);
    conferir_tsp(t, 0, 0, 0, 0);
    liberar_tsp(t);
    t = criar_tsp(3, grandes);
    conferir_tsp(t, 0, 3LL * INT_MAX, 3LL * INT_MAX, 3LL * INT_MAX);
    liberar_tsp(t);
    t = criar_tsp(3, zeros);
    conferir_tsp(t, 0, 0, 0, 0);
    liberar_tsp(t);
    TESTE(criar_tsp(2, invalido) == NULL);
    puts("OK: TSP de custo conhecido, heuristica subotima, pesos zero e custos grandes.");
}

static void testar_reducao(void) {
    /* Todos os grafos simples de ordem 4: custo TSP = 4 sse ha ciclo. */
    for (unsigned int mascara = 0; mascara < 64; mascara++) {
        Grafo *g = criar_grafo(4);
        unsigned int bit = 0;
        for (int u = 0; u < 4; u++) {
            for (int v = u + 1; v < 4; v++, bit++) {
                if (mascara & (1u << bit)) inserir_aresta(g, u, v);
            }
        }
        int ciclo[5], rota[5];
        int tem_ciclo = encontrar_ciclo_hamiltoniano_bt(g, ciclo);
        if (tem_ciclo) validar_hamiltoniano(g, ciclo, 1);
        TSP *t = grafo_para_tsp(g);
        TESTE(t != NULL);
        for (int u = 0; u < 4; u++) {
            for (int v = 0; v < 4; v++) {
                TESTE(peso_tsp(t, u, v) == (u == v ? 0 : (tem_aresta(g,u,v) ? 1 : 2)));
            }
        }
        unsigned long long folhas;
        long long otimo = tsp_forca_bruta(t, 0, rota, &folhas);
        TESTE(folhas == 6 && custo_tour(t, rota) == otimo);
        TESTE((otimo == 4) == tem_ciclo);
        long long cn = tsp_vizinho_mais_proximo(t, 0, rota);
        TESTE(cn >= otimo && custo_tour(t, rota) == cn);
        long long ci = tsp_insercao(t, 0, rota);
        TESTE(ci >= otimo && custo_tour(t, rota) == ci);
        liberar_tsp(t);
        liberar_grafo(g);
    }
    puts("OK: reducao Hamiltoniano -> TSP em todos os 64 grafos de ordem 4.");
}

static double medir(const TSP *t, int algoritmo, int *rota,
                     long long *custo, unsigned long long *folhas) {
    clock_t inicio = clock(), fim;
    unsigned long repeticoes = 0;
    do {
        if (algoritmo == 0) *custo = tsp_forca_bruta(t, 0, rota, folhas);
        else if (algoritmo == 1) *custo = tsp_vizinho_mais_proximo(t, 0, rota);
        else *custo = tsp_insercao(t, 0, rota);
        repeticoes++;
        fim = clock();
    } while ((double)(fim - inicio) / CLOCKS_PER_SEC < 0.02);
    return 1e3 * (double)(fim - inicio) / CLOCKS_PER_SEC / repeticoes;
}

static void experimento(void) {
    const int tamanhos[] = {5, 8, 10, 12};
    puts("\nn | permutacoes | otimo | exato(ms) | vizinho | tempo(ms) | insercao | tempo(ms)");
    fflush(stdout);
    for (int k = 0; k < 4; k++) {
        int n = tamanhos[k];
        int *pesos = calloc((size_t)n * n, sizeof *pesos);
        int *rota = calloc((size_t)n + 1, sizeof *rota);
        TESTE(pesos && rota);
        unsigned int estado = 2026u + (unsigned int)n;
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                estado = 1664525u * estado + 1013904223u;
                pesos[u*n+v] = pesos[v*n+u] = 1 + (int)(estado % 100);
            }
        }
        TSP *t = criar_tsp(n, pesos);
        TESTE(t != NULL);
        unsigned long long folhas = 0, fatorial = 1;
        for (int i = 2; i < n; i++) fatorial *= (unsigned int)i;
        long long exato, vizinho, insercao;
        double te = medir(t, 0, rota, &exato, &folhas);
        TESTE(custo_tour(t, rota) == exato && folhas == fatorial);
        double tv = medir(t, 1, rota, &vizinho, &folhas);
        TESTE(custo_tour(t, rota) == vizinho && vizinho >= exato);
        double ti = medir(t, 2, rota, &insercao, &folhas);
        TESTE(custo_tour(t, rota) == insercao && insercao >= exato);
        printf("%2d | %11llu | %5lld | %9.3f | %7lld | %9.6f | %8lld | %9.6f\n",
               n, folhas, exato, te, vizinho, tv, insercao, ti);
        fflush(stdout);
        liberar_tsp(t);
        free(pesos);
        free(rota);
    }
    puts("Tempos medios de CPU por chamada; cada medicao dura ao menos 20 ms.");
    puts("Instancias simetricas reproduziveis, semente 2026+n, pesos de 1 a 100.");
}

int main(void) {
    testar_hamiltoniano();
    testar_tsp();
    testar_reducao();
    experimento();
    puts("Todos os testes da Pratica 10 passaram.");
    return EXIT_SUCCESS;
}
