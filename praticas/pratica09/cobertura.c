#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cobertura.h"

static void *alocar(size_t n, size_t tamanho) {
    void *p = calloc(n ? n : 1, tamanho);
    if (!p) {
        fprintf(stderr, "Sem memoria.\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

Grafo *criar_grafo(int n) {
    if (n < 0) return NULL;
    Grafo *g = alocar(1, sizeof *g);
    g->n = n;
    g->adj = alocar((size_t)n * (size_t)n, sizeof *g->adj);
    return g;
}

void inserir_aresta(Grafo *g, int u, int v) {
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n) {
        fprintf(stderr, "Vertices invalidos.\n");
        exit(EXIT_FAILURE);
    }
    g->adj[(size_t)u * g->n + v] = 1;
    g->adj[(size_t)v * g->n + u] = 1;
}

int tem_aresta(const Grafo *g, int u, int v) {
    return g->adj[(size_t)u * g->n + v] != 0;
}

void liberar_grafo(Grafo *g) {
    if (!g) return;
    free(g->adj);
    free(g);
}

int eh_cobertura(const Grafo *g, const int *selecionados) {
    if (!g || (!selecionados && g->n > 0)) return 0;
    for (int u = 0; u < g->n; u++) {
        for (int v = u; v < g->n; v++) {
            if (tem_aresta(g, u, v) && !selecionados[u] && !selecionados[v]) return 0;
        }
    }
    return 1;
}

int cobertura_gulosa(const Grafo *g, int *selecionados) {
    if (!g || (!selecionados && g->n > 0)) return -1;
    if (g->n > 0) memset(selecionados, 0, (size_t)g->n * sizeof *selecionados);
    int quantidade = 0;
    while (1) {
        int melhor = -1, maior_grau = 0;
        for (int u = 0; u < g->n; u++) {
            if (selecionados[u]) continue;
            int grau = 0;
            for (int v = 0; v < g->n; v++) {
                if (!selecionados[v] && tem_aresta(g, u, v)) grau++;
            }
            if (grau > maior_grau) {
                maior_grau = grau;
                melhor = u; /* Empate: menor indice. */
            }
        }
        if (melhor == -1) break;
        selecionados[melhor] = 1;
        quantidade++;
    }
    return quantidade;
}

typedef struct {
    const Grafo *g;
    int *atual, *melhor, *marcados;
    int limite;
} Busca;

/* Arestas descobertas sem extremos em comum exigem vertices distintos. */
static int limite_inferior(Busca *b) {
    memset(b->marcados, 0, (size_t)b->g->n * sizeof *b->marcados);
    int limite = 0;
    for (int u = 0; u < b->g->n; u++) {
        for (int v = u; v < b->g->n; v++) {
            if (!b->atual[u] && !b->atual[v] &&
                !b->marcados[u] && !b->marcados[v] && tem_aresta(b->g, u, v)) {
                b->marcados[u] = b->marcados[v] = 1;
                limite++;
            }
        }
    }
    return limite;
}

static void buscar(Busca *b, int quantidade) {
    if (quantidade >= b->limite) return;
    int u = -1, v = -1;
    for (int i = 0; i < b->g->n && u == -1; i++) {
        for (int j = i; j < b->g->n; j++) {
            if (!b->atual[i] && !b->atual[j] && tem_aresta(b->g, i, j)) {
                u = i;
                v = j;
                break;
            }
        }
    }
    if (u == -1) {
        b->limite = quantidade;
        memcpy(b->melhor, b->atual, (size_t)b->g->n * sizeof *b->melhor);
        return;
    }
    if (quantidade + limite_inferior(b) >= b->limite) return;
    /* Toda cobertura precisa conter u ou v. Um laco obriga selecionar u. */
    b->atual[u] = 1;
    buscar(b, quantidade + 1);
    b->atual[u] = 0;
    if (u != v) {
        b->atual[v] = 1;
        buscar(b, quantidade + 1);
        b->atual[v] = 0;
    }
}

int cobertura_minima_bt(const Grafo *g, int *selecionados) {
    if (!g || (!selecionados && g->n > 0)) return -1;
    if (g->n == 0) return 0;
    Busca b = {g, alocar((size_t)g->n, sizeof(int)), selecionados,
               alocar((size_t)g->n, sizeof(int)), 0};
    b.limite = cobertura_gulosa(g, selecionados); /* Limite superior inicial. */
    buscar(&b, 0);
    free(b.atual);
    free(b.marcados);
    return b.limite;
}
