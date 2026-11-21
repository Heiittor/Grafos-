#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

static void *alocar(size_t n, size_t tamanho) {
    void *p = calloc(n, tamanho);
    if (!p) {
        fprintf(stderr, "Sem memoria.\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    if (n <= 0) return NULL;
    GrafoListaPonderado *g = alocar(1, sizeof *g);
    g->n = n;
    g->adj = alocar((size_t)n, sizeof *g->adj);
    g->vertice_ciclo = -1;
    return g;
}

void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso) {
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n) {
        fprintf(stderr, "Vertices invalidos.\n");
        exit(EXIT_FAILURE);
    }
    NoPeso *novo = alocar(1, sizeof *novo);
    *novo = (NoPeso){v, peso, g->adj[u]};
    g->adj[u] = novo;
}

void liberar_grafo_ponderado(GrafoListaPonderado *g) {
    if (!g) return;
    for (int u = 0; u < g->n; u++) {
        NoPeso *p = g->adj[u];
        while (p) {
            NoPeso *proximo = p->prox;
            free(p);
            p = proximo;
        }
    }
    free(g->adj);
    free(g);
}

/* Uma passagem completa de relaxacao. Atualiza pred inclusive na n-esima. */
static int relaxar(GrafoListaPonderado *g, long long *d, int *pred) {
    int ultimo = -1;
    for (int u = 0; u < g->n; u++) {
        if (d[u] == LLONG_MAX) continue;
        for (NoPeso *p = g->adj[u]; p; p = p->prox) {
            if ((p->peso < 0 && d[u] < LLONG_MIN - p->peso) ||
                (p->peso > 0 && d[u] > LLONG_MAX - p->peso)) {
                fprintf(stderr, "Distancia excedeu long long.\n");
                exit(EXIT_FAILURE);
            }
            long long candidato = d[u] + p->peso;
            if (candidato < d[p->destino]) {
                d[p->destino] = candidato;
                pred[p->destino] = u;
                ultimo = p->destino;
            }
        }
    }
    return ultimo;
}

int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    if (!g || !dist || !pred || origem < 0 || origem >= g->n) {
        fprintf(stderr, "Parametros invalidos para Bellman-Ford.\n");
        exit(EXIT_FAILURE);
    }
    long long *d = alocar((size_t)g->n, sizeof *d);
    for (int v = 0; v < g->n; v++) {
        d[v] = LLONG_MAX;
        pred[v] = -1;
    }
    d[origem] = 0;
    for (int passagem = 1; passagem < g->n; passagem++) {
        relaxar(g, d, pred);
    }
    g->vertice_ciclo = relaxar(g, d, pred); /* n-esima passagem: deteccao. */
    for (int v = 0; v < g->n; v++) {
        if (d[v] == LLONG_MAX) dist[v] = INT_MAX;
        else if (d[v] < INT_MIN) dist[v] = INT_MIN;
        else if (d[v] >= INT_MAX) dist[v] = INT_MAX - 1;
        else dist[v] = (int)d[v];
    }
    free(d);
    return g->vertice_ciclo == -1;
}

int reconstruir_ciclo_negativo(int n, int vertice, const int *pred) {
    if (!pred || n <= 0) return 0;
    for (int i = 0; i < n; i++) {
        if (vertice < 0 || vertice >= n) return 0;
        vertice = pred[vertice];
    }
    if (vertice < 0 || vertice >= n) return 0;

    int *ciclo = alocar((size_t)n + 1, sizeof *ciclo);
    int atual = vertice, tamanho = 0;
    do {
        if (atual < 0 || atual >= n || tamanho == n) {
            free(ciclo);
            return 0;
        }
        ciclo[tamanho++] = atual;
        atual = pred[atual];
    } while (atual != vertice);
    ciclo[tamanho] = vertice;
    printf("Ciclo negativo: ");
    for (int i = tamanho; i >= 0; i--) {
        printf("%d%s", ciclo[i], i == 0 ? "\n" : " -> ");
    }
    free(ciclo);
    return 1;
}
