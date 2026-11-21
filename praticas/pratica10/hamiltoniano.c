#include <stdio.h>
#include <stdlib.h>
#include "hamiltoniano.h"

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
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n || u == v) {
        fprintf(stderr, "Aresta invalida para grafo simples.\n");
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

/* Constroi permutacoes sem repetir vertices; poda prefixos sem aresta. */
static int buscar(const Grafo *g, int *caminho, int *usado,
                  int tamanho, int fechar) {
    if (tamanho == g->n) {
        return !fechar || tem_aresta(g, caminho[tamanho - 1], caminho[0]);
    }
    for (int v = 0; v < g->n; v++) {
        if (!usado[v] && tem_aresta(g, caminho[tamanho - 1], v)) {
            usado[v] = 1;
            caminho[tamanho] = v;
            if (buscar(g, caminho, usado, tamanho + 1, fechar)) return 1;
            usado[v] = 0;
        }
    }
    return 0;
}

static int resolver(const Grafo *g, int *caminho, int fechar) {
    if (!g || !caminho || g->n == 0 || (fechar && g->n < 3)) return 0;
    int *usado = alocar((size_t)g->n, sizeof *usado);
    int encontrado = 0;
    for (int inicio = 0; inicio < (fechar ? 1 : g->n); inicio++) {
        caminho[0] = inicio;
        usado[inicio] = 1;
        encontrado = buscar(g, caminho, usado, 1, fechar);
        if (encontrado) break;
        usado[inicio] = 0;
    }
    if (encontrado && fechar) caminho[g->n] = caminho[0];
    free(usado);
    return encontrado;
}

int encontrar_caminho_hamiltoniano_bt(const Grafo *g, int *caminho) {
    return resolver(g, caminho, 0);
}

int encontrar_ciclo_hamiltoniano_bt(const Grafo *g, int *ciclo) {
    return resolver(g, ciclo, 1);
}
