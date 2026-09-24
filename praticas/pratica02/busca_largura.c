#include <stdio.h>
#include <stdlib.h>
#include "busca_largura.h"

Fila* criar_fila(int capacidade) {
    Fila *f = (Fila*) malloc(sizeof(Fila));
    if (f == NULL) return NULL;
    f->dados = (int*) malloc(capacidade * sizeof(int));
    f->capacidade = capacidade;
    f->inicio = 0;
    f->fim = 0;
    f->tamanho = 0;
    return f;
}

void enfileirar(Fila *f, int v) {
    if (f == NULL || f->tamanho == f->capacidade) return;
    f->dados[f->fim] = v;
    f->fim = (f->fim + 1) % f->capacidade;
    f->tamanho++;
}

int desenfileirar(Fila *f) {
    if (f == NULL || f->tamanho == 0) return -1;
    int v = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return v;
}

int fila_vazia(Fila *f) {
    if (f == NULL) return 1;
    return f->tamanho == 0;
}

void liberar_fila(Fila *f) {
    if (f == NULL) return;
    free(f->dados);
    free(f);
}

void bfs(GrafoLista *g, int origem, int *dist, int *pred) {
    if (g == NULL || origem < 0 || origem >= g->n) return;

    for (int i = 0; i < g->n; i++) {
        dist[i] = -1;
        pred[i] = -1;
    }

    Fila *f = criar_fila(g->n);
    dist[origem] = 0;
    enfileirar(f, origem);

    while (!fila_vazia(f)) {
        int u = desenfileirar(f);
        No *atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->destino;
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                pred[v] = u;
                enfileirar(f, v);
            }
            atual = atual->prox;
        }
    }

    liberar_fila(f);
}

int eh_bipartido(GrafoLista *g) {
    if (g == NULL) return 1;

    int *cor = (int*) malloc(g->n * sizeof(int));
    for (int i = 0; i < g->n; i++) {
        cor[i] = -1;
    }

    Fila *f = criar_fila(g->n);

    for (int i = 0; i < g->n; i++) {
        if (cor[i] == -1) {
            cor[i] = 0;
            enfileirar(f, i);

            while (!fila_vazia(f)) {
                int u = desenfileirar(f);
                No *atual = g->adj[u];
                while (atual != NULL) {
                    int v = atual->destino;
                    if (cor[v] == -1) {
                        cor[v] = 1 - cor[u];
                        enfileirar(f, v);
                    } else if (cor[v] == cor[u]) {
                        free(cor);
                        liberar_fila(f);
                        return 0;
                    }
                    atual = atual->prox;
                }
            }
        }
    }

    free(cor);
    liberar_fila(f);
    return 1;
}