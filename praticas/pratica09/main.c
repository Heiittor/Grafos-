#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cobertura.h"

#define TESTE(c) do { if (!(c)) { \
    fprintf(stderr, "FALHA linha %d: %s\n", __LINE__, #c); \
    exit(EXIT_FAILURE); } } while (0)

static void conferir(const Grafo *g, int esperado, int esperado_gulosa) {
    int *exata = calloc((size_t)g->n + 1, sizeof *exata);
    int *gulosa = calloc((size_t)g->n + 1, sizeof *gulosa);
    TESTE(exata && gulosa);
    int ne = cobertura_minima_bt(g, exata), ng = cobertura_gulosa(g, gulosa);
    TESTE(ne == esperado && ng == esperado_gulosa);
    TESTE(eh_cobertura(g, exata) && eh_cobertura(g, gulosa));
    int soma = 0;
    for (int v = 0; v < g->n; v++) soma += exata[v];
    TESTE(soma == ne);
    free(exata);
    free(gulosa);
}

/* Oraculo independente: enumera todos os subconjuntos de vertices. */
static int oraculo(const Grafo *g) {
    int melhor = g->n, selecionados[4];
    for (unsigned int mascara = 0; mascara < (1u << g->n); mascara++) {
        int tamanho = 0;
        for (int v = 0; v < g->n; v++) {
            selecionados[v] = (mascara >> v) & 1u;
            tamanho += selecionados[v];
        }
        if (tamanho < melhor && eh_cobertura(g, selecionados)) melhor = tamanho;
    }
    return melhor;
}

static void testar(void) {
    Grafo *g = criar_grafo(0);
    conferir(g, 0, 0);
    liberar_grafo(g);
    g = criar_grafo(1);
    conferir(g, 0, 0);
    inserir_aresta(g, 0, 0);
    conferir(g, 1, 1);
    liberar_grafo(g);
    g = criar_grafo(7);
    for (int v = 1; v <= 3; v++) {
        inserir_aresta(g, 0, v);
        inserir_aresta(g, v, v + 3);
    }
    conferir(g, 3, 4); /* Contraexemplo: a gulosa pode ser subotima. */
    liberar_grafo(g);

    for (unsigned int mascara = 0; mascara < 64; mascara++) {
        g = criar_grafo(4);
        unsigned int bit = 0;
        for (int u = 0; u < 4; u++) {
            for (int v = u + 1; v < 4; v++, bit++) {
                if (mascara & (1u << bit)) inserir_aresta(g, u, v);
            }
        }
        int exata[4], gulosa[4];
        int ne = cobertura_minima_bt(g, exata), ng = cobertura_gulosa(g, gulosa);
        TESTE(ne == oraculo(g) && ng >= ne);
        TESTE(eh_cobertura(g, exata) && eh_cobertura(g, gulosa));
        liberar_grafo(g);
    }
    puts("OK: vazio, laco, contraexemplo guloso e todos os 64 grafos simples de ordem 4.");
}

static double medir(const Grafo *g, int *saida,
                     int (*algoritmo)(const Grafo *, int *), int *tamanho) {
    clock_t inicio = clock(), fim;
    unsigned long repeticoes = 0;
    do {
        *tamanho = algoritmo(g, saida);
        repeticoes++;
        fim = clock();
    } while ((double)(fim - inicio) / CLOCKS_PER_SEC < 0.02);
    return 1e6 * (double)(fim - inicio) / CLOCKS_PER_SEC / repeticoes;
}

static void experimento(void) {
    const int tamanhos[] = {5, 8, 10, 12, 15};
    puts("\nn | exata | gulosa | excesso | BT (us) | gulosa (us)");
    for (int i = 0; i < 5; i++) {
        int n = tamanhos[i], ne, ng;
        Grafo *g = criar_grafo(n);
        unsigned int estado = 2026u + (unsigned int)n;
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                estado = 1664525u * estado + 1013904223u;
                if (estado % 100 < 35) inserir_aresta(g, u, v);
            }
        }
        int *exata = calloc((size_t)n, sizeof *exata);
        int *gulosa = calloc((size_t)n, sizeof *gulosa);
        TESTE(exata && gulosa);
        double te = medir(g, exata, cobertura_minima_bt, &ne);
        double tg = medir(g, gulosa, cobertura_gulosa, &ng);
        TESTE(eh_cobertura(g, exata) && eh_cobertura(g, gulosa) && ng >= ne);
        printf("%2d | %5d | %6d | %7d | %7.3f | %10.3f\n", n, ne, ng, ng-ne, te, tg);
        free(exata);
        free(gulosa);
        liberar_grafo(g);
    }
    puts("Tempos medios de CPU por chamada, repetidos por pelo menos 20 ms.");
    puts("Grafos reproduziveis (semente 2026+n, probabilidade de aresta 35%).");
}

int main(void) {
    testar();
    experimento();
    puts("Todos os testes da Pratica 09 passaram.");
    return EXIT_SUCCESS;
}
