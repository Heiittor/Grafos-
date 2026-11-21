#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tps.h"

static void *alocar(size_t n, size_t tamanho) {
    void *p = calloc(n ? n : 1, tamanho);
    if (!p) {
        fprintf(stderr, "Sem memoria.\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

int peso_tsp(const TSP *t, int u, int v) {
    return t->pesos[(size_t)u * t->n + v];
}

TSP *criar_tsp(int n, const int *pesos) {
    if (n <= 0 || !pesos) return NULL;
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            int peso = pesos[(size_t)u * n + v];
            if (peso < 0 || (u == v && peso != 0) ||
                peso != pesos[(size_t)v * n + u]) return NULL;
        }
    }
    TSP *t = alocar(1, sizeof *t);
    t->n = n;
    t->pesos = alocar((size_t)n * (size_t)n, sizeof *t->pesos);
    memcpy(t->pesos, pesos, (size_t)n * (size_t)n * sizeof *pesos);
    return t;
}

void liberar_tsp(TSP *t) {
    if (!t) return;
    free(t->pesos);
    free(t);
}

TSP *grafo_para_tsp(const Grafo *g) {
    if (!g || g->n == 0) return NULL;
    int n = g->n;
    int *pesos = alocar((size_t)n * (size_t)n, sizeof *pesos);
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            pesos[(size_t)u * n + v] = u == v ? 0 : (tem_aresta(g, u, v) ? 1 : 2);
        }
    }
    TSP *t = criar_tsp(n, pesos);
    free(pesos);
    return t;
}

long long custo_tour(const TSP *t, const int *rota) {
    if (!t || !rota || rota[t->n] != rota[0]) return -1;
    int *usado = alocar((size_t)t->n, sizeof *usado);
    long long custo = 0;
    for (int i = 0; i < t->n; i++) {
        int u = rota[i], v = rota[i + 1];
        if (u < 0 || u >= t->n || v < 0 || v >= t->n || usado[u]) {
            free(usado);
            return -1;
        }
        usado[u] = 1;
        custo += peso_tsp(t, u, v);
    }
    free(usado);
    return custo;
}

typedef struct {
    const TSP *t;
    int *atual, *melhor;
    long long custo;
    unsigned long long folhas;
} BuscaTSP;

static void trocar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* Sem poda por custo: cada permutacao e visitada, conforme a atividade. */
static void permutar(BuscaTSP *b, int posicao, long long parcial) {
    int n = b->t->n;
    if (posicao == n) {
        if (b->folhas < ULLONG_MAX) b->folhas++;
        long long custo = parcial + peso_tsp(b->t, b->atual[n - 1], b->atual[0]);
        if (custo < b->custo) {
            b->custo = custo;
            memcpy(b->melhor, b->atual, (size_t)n * sizeof *b->melhor);
            b->melhor[n] = b->atual[0];
        }
        return;
    }
    for (int i = posicao; i < n; i++) {
        trocar(&b->atual[posicao], &b->atual[i]);
        long long custo = parcial +
            peso_tsp(b->t, b->atual[posicao - 1], b->atual[posicao]);
        permutar(b, posicao + 1, custo);
        trocar(&b->atual[posicao], &b->atual[i]);
    }
}

static int valido(const TSP *t, int origem, const int *rota) {
    return t && rota && origem >= 0 && origem < t->n;
}

long long tsp_forca_bruta(const TSP *t, int origem, int *rota,
                          unsigned long long *permutacoes) {
    if (permutacoes) *permutacoes = 0;
    if (!valido(t, origem, rota)) return -1;
    BuscaTSP b = {t, alocar((size_t)t->n, sizeof(int)), rota, LLONG_MAX, 0};
    b.atual[0] = origem;
    int posicao = 1;
    for (int v = 0; v < t->n; v++) {
        if (v != origem) b.atual[posicao++] = v;
    }
    permutar(&b, 1, 0);
    if (permutacoes) *permutacoes = b.folhas;
    free(b.atual);
    return b.custo;
}

long long tsp_vizinho_mais_proximo(const TSP *t, int origem, int *rota) {
    if (!valido(t, origem, rota)) return -1;
    int *usado = alocar((size_t)t->n, sizeof *usado);
    rota[0] = origem;
    usado[origem] = 1;
    for (int i = 1; i < t->n; i++) {
        int escolhido = -1;
        long long menor = LLONG_MAX;
        for (int v = 0; v < t->n; v++) {
            if (!usado[v] && peso_tsp(t, rota[i - 1], v) < menor) {
                escolhido = v;
                menor = peso_tsp(t, rota[i - 1], v);
            }
        }
        rota[i] = escolhido;
        usado[escolhido] = 1;
    }
    rota[t->n] = origem;
    free(usado);
    return custo_tour(t, rota);
}

long long tsp_insercao(const TSP *t, int origem, int *rota) {
    if (!valido(t, origem, rota)) return -1;
    int *usado = alocar((size_t)t->n, sizeof *usado);
    int *proximidade = alocar((size_t)t->n, sizeof *proximidade);
    rota[0] = rota[1] = origem;
    usado[origem] = 1;
    for (int v = 0; v < t->n; v++) proximidade[v] = peso_tsp(t, origem, v);
    for (int tamanho = 1; tamanho < t->n; tamanho++) {
        int escolhido = -1;
        long long menor = LLONG_MAX;
        /* Vertice externo mais proximo de qualquer vertice do ciclo. */
        for (int v = 0; v < t->n; v++) {
            if (!usado[v] && proximidade[v] < menor) {
                menor = proximidade[v];
                escolhido = v;
            }
        }
        int posicao = 0;
        long long acrescimo = LLONG_MAX;
        for (int i = 0; i < tamanho; i++) {
            long long delta = (long long)peso_tsp(t, rota[i], escolhido) +
                peso_tsp(t, escolhido, rota[i + 1]) - peso_tsp(t, rota[i], rota[i + 1]);
            if (delta < acrescimo) {
                acrescimo = delta;
                posicao = i + 1;
            }
        }
        for (int i = tamanho + 1; i > posicao; i--) rota[i] = rota[i - 1];
        rota[posicao] = escolhido;
        usado[escolhido] = 1;
        for (int v = 0; v < t->n; v++) {
            int peso = peso_tsp(t, escolhido, v);
            if (peso < proximidade[v]) proximidade[v] = peso;
        }
    }
    free(usado);
    free(proximidade);
    return custo_tour(t, rota);
}
