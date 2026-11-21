#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agm.h"

static void exigir(int condicao) {
    if (!condicao) {
        fprintf(stderr, "Parametros invalidos ou memoria insuficiente.\n");
        exit(EXIT_FAILURE);
    }
}

static void *alocar(size_t n, size_t tamanho) {
    void *p = calloc(n ? n : 1, tamanho);
    exigir(p != NULL);
    return p;
}

DSU criar_dsu(int n) {
    exigir(n > 0);
    DSU d = {alocar((size_t)n, sizeof(int)),
             alocar((size_t)n, sizeof(int)), n};
    for (int v = 0; v < n; v++) d.pai[v] = v;
    return d;
}

void liberar_dsu(DSU *d) {
    if (!d) return;
    free(d->pai);
    free(d->rank);
    *d = (DSU){NULL, NULL, 0};
}

int find(DSU *d, int x) {
    exigir(d && x >= 0 && x < d->n);
    int raiz = x;
    while (d->pai[raiz] != raiz) raiz = d->pai[raiz];
    while (x != raiz) {
        int proximo = d->pai[x];
        d->pai[x] = raiz; /* Compressao completa do caminho. */
        x = proximo;
    }
    return raiz;
}

int unir(DSU *d, int x, int y) {
    int a = find(d, x), b = find(d, y);
    if (a == b) return 0;
    if (d->rank[a] < d->rank[b]) d->pai[a] = b;
    else {
        d->pai[b] = a;
        if (d->rank[a] == d->rank[b]) d->rank[a]++;
    }
    return 1;
}

static void validar_arestas(int n, const Aresta *a, int m) {
    exigir(n > 0 && m >= 0 && (m == 0 || a));
    for (int i = 0; i < m; i++) {
        exigir(a[i].u >= 0 && a[i].u < n && a[i].v >= 0 && a[i].v < n);
    }
}

GrafoListaPonderado *criar_grafo(int n, const Aresta *arestas, int m) {
    validar_arestas(n, arestas, m);
    GrafoListaPonderado *g = alocar(1, sizeof *g);
    g->n = n;
    g->adj = alocar((size_t)n, sizeof *g->adj);
    for (int i = 0; i < m; i++) {
        int u = arestas[i].u, v = arestas[i].v, peso = arestas[i].peso;
        NoPeso *ida = alocar(1, sizeof *ida), *volta = alocar(1, sizeof *volta);
        *ida = (NoPeso){v, peso, g->adj[u]};
        g->adj[u] = ida;
        *volta = (NoPeso){u, peso, g->adj[v]};
        g->adj[v] = volta;
    }
    return g;
}

void liberar_grafo(GrafoListaPonderado *g) {
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

static int comparar(const void *a, const void *b) {
    const Aresta *x = a, *y = b;
    return (x->peso > y->peso) - (x->peso < y->peso);
}

int kruskal(int n, const Aresta *arestas, int m,
            Aresta *resultado, long long *custo) {
    validar_arestas(n, arestas, m);
    exigir(custo && (n == 1 || resultado));
    Aresta *ordenadas = alocar((size_t)m, sizeof *ordenadas);
    if (m > 0) memcpy(ordenadas, arestas, (size_t)m * sizeof *ordenadas);
    qsort(ordenadas, (size_t)m, sizeof *ordenadas, comparar);
    DSU d = criar_dsu(n);
    int quantidade = 0;
    *custo = 0;
    for (int i = 0; i < m && quantidade < n - 1; i++) {
        Aresta a = ordenadas[i];
        if (unir(&d, a.u, a.v)) {
            resultado[quantidade++] = a;
            *custo += a.peso;
        }
    }
    liberar_dsu(&d);
    free(ordenadas);
    return quantidade;
}

typedef struct {
    int vertice;
    long long chave; /* LLONG_MAX e infinito, inclusive com peso INT_MAX. */
} NoHeap;

typedef struct {
    NoHeap *dados;
    int *posicao, tamanho;
} MinHeap;

static void trocar(MinHeap *h, int i, int j) {
    NoHeap temp = h->dados[i];
    h->dados[i] = h->dados[j];
    h->dados[j] = temp;
    h->posicao[h->dados[i].vertice] = i;
    h->posicao[h->dados[j].vertice] = j;
}

static void diminuir_chave(MinHeap *h, int v, long long chave) {
    int i = h->posicao[v];
    h->dados[i].chave = chave;
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->dados[pai].chave <= h->dados[i].chave) break;
        trocar(h, i, pai);
        i = pai;
    }
}

static int extrair_minimo(MinHeap *h) {
    int v = h->dados[0].vertice;
    h->posicao[v] = -1;
    if (--h->tamanho == 0) return v;
    h->dados[0] = h->dados[h->tamanho];
    h->posicao[h->dados[0].vertice] = 0;
    int i = 0;
    while (i < h->tamanho / 2) {
        int filho = 2 * i + 1;
        if (filho + 1 < h->tamanho &&
            h->dados[filho + 1].chave < h->dados[filho].chave) filho++;
        if (h->dados[i].chave <= h->dados[filho].chave) break;
        trocar(h, i, filho);
        i = filho;
    }
    return v;
}

int prim(GrafoListaPonderado *g, int origem,
         Aresta *resultado, long long *custo) {
    exigir(g && origem >= 0 && origem < g->n && custo &&
            (g->n == 1 || resultado));
    MinHeap h = {alocar((size_t)g->n, sizeof(NoHeap)),
                 alocar((size_t)g->n, sizeof(int)), g->n};
    int *pai = alocar((size_t)g->n, sizeof *pai);
    long long *chave = alocar((size_t)g->n, sizeof *chave);
    for (int v = 0; v < g->n; v++) {
        pai[v] = -1;
        chave[v] = LLONG_MAX;
        h.dados[v] = (NoHeap){v, LLONG_MAX};
        h.posicao[v] = v;
    }
    chave[origem] = 0;
    diminuir_chave(&h, origem, 0);
    *custo = 0;
    int quantidade = 0;
    while (h.tamanho > 0) {
        int u = extrair_minimo(&h);
        if (pai[u] != -1) {
            resultado[quantidade++] = (Aresta){pai[u], u, (int)chave[u]};
            *custo += chave[u];
        } /* Sem pai: raiz da primeira ou de uma nova componente. */
        for (NoPeso *p = g->adj[u]; p; p = p->prox) {
            int v = p->destino;
            if (h.posicao[v] != -1 && p->peso < chave[v]) {
                pai[v] = u;
                chave[v] = p->peso;
                diminuir_chave(&h, v, chave[v]);
            }
        }
    }
    free(pai);
    free(chave);
    free(h.dados);
    free(h.posicao);
    return quantidade;
}
