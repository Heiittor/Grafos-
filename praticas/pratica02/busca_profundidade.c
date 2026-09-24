#include <stdio.h>
#include <stdlib.h>
#include "busca_profundidade.h"

Pilha* criar_pilha(int capacidade) {
    Pilha *p = (Pilha*) malloc(sizeof(Pilha));
    if (p == NULL) return NULL;
    p->dados = (int*) malloc(capacidade * sizeof(int));
    p->capacidade = capacidade;
    p->topo = -1;
    return p;
}

void empilhar(Pilha *p, int v) {
    if (p == NULL || p->topo == p->capacidade - 1) return;
    p->topo++;
    p->dados[p->topo] = v;
}

int desempilhar(Pilha *p) {
    if (p == NULL || p->topo == -1) return -1;
    int v = p->dados[p->topo];
    p->topo--;
    return v;
}

int pilha_vazia(Pilha *p) {
    if (p == NULL) return 1;
    return p->topo == -1;
}

void liberar_pilha(Pilha *p) {
    if (p == NULL) return;
    free(p->dados);
    free(p);
}

void dfs_recursiva(GrafoLista *g, int u, int *visitado, int *d, int *f, int *tempo) {
    if (g == NULL || u < 0 || u >= g->n) return;

    visitado[u] = 1;
    (*tempo)++;
    if (d != NULL) d[u] = *tempo;

    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        if (!visitado[v]) {
            dfs_recursiva(g, v, visitado, d, f, tempo);
        }
        atual = atual->prox;
    }

    (*tempo)++;
    if (f != NULL) f[u] = *tempo;
}

int contar_componentes(GrafoLista *g) {
    if (g == NULL) return 0;

    int *visitado = (int*) calloc(g->n, sizeof(int));
    int componentes = 0;
    int tempo = 0;

    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            componentes++;
            dfs_recursiva(g, i, visitado, NULL, NULL, &tempo);
        }
    }

    free(visitado);
    return componentes;
}

static int tem_ciclo_dfs(GrafoLista *g, int u, int pai, int *visitado) {
    visitado[u] = 1;
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        if (!visitado[v]) {
            if (tem_ciclo_dfs(g, v, u, visitado)) return 1;
        } else if (v != pai) {
            return 1;
        }
        atual = atual->prox;
    }
    return 0;
}

int tem_ciclo(GrafoLista *g) {
    if (g == NULL) return 0;

    int *visitado = (int*) calloc(g->n, sizeof(int));

    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            if (tem_ciclo_dfs(g, i, -1, visitado)) {
                free(visitado);
                return 1;
            }
        }
    }

    free(visitado);
    return 0;
}