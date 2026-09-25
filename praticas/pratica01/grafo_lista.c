#include <stdio.h>
#include <stdlib.h>
#include "grafo_lista.h"

GrafoLista* criar_grafo_lista(int n) {
    GrafoLista *g = (GrafoLista*) malloc(sizeof(GrafoLista));
    if (g == NULL) return NULL;

    g->n = n;
    g->adj = (No**) malloc(n * sizeof(No*));
    for (int i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
    return g;
}

// Função auxiliar para inserir nó na cabeça da lista
static void inserir_no_cabeca(No **cabeca, int destino) {
    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) return;
    novo->destino = destino;
    novo->prox = *cabeca;
    *cabeca = novo;
}

void inserir_aresta_lista(GrafoLista *g, int u, int v) {
    if (g == NULL || u < 0 || u >= g->n || v < 0 || v >= g->n) return;
    inserir_no_cabeca(&g->adj[u], v);
    inserir_no_cabeca(&g->adj[v], u);
}

// Função auxiliar para remover nó específico da lista
static void remover_no(No **cabeca, int destino) {
    No *atual = *cabeca;
    No *anterior = NULL;

    while (atual != NULL && atual->destino != destino) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) return; // Elemento não encontrado

    if (anterior == NULL) {
        *cabeca = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }
    free(atual);
}

void remover_aresta_lista(GrafoLista *g, int u, int v) {
    if (g == NULL || u < 0 || u >= g->n || v < 0 || v >= g->n) return;
    remover_no(&g->adj[u], v);
    remover_no(&g->adj[v], u);
}

int grau_lista(GrafoLista *g, int u) {
    if (g == NULL || u < 0 || u >= g->n) return 0;
    int grau = 0;
    No *atual = g->adj[u];
    while (atual != NULL) {
        grau++;
        atual = atual->prox;
    }
    return grau;
}

int sao_adjacentes_lista(GrafoLista *g, int u, int v) {
    if (g == NULL || u < 0 || u >= g->n || v < 0 || v >= g->n) return 0;
    No *atual = g->adj[u];
    while (atual != NULL) {
        if (atual->destino == v) return 1;
        atual = atual->prox;
    }
    return 0;
}

void liberar_grafo_lista(GrafoLista *g) {
    if (g == NULL) return;
    for (int i = 0; i < g->n; i++) {
        No *atual = g->adj[i];
        while (atual != NULL) {
            No *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(g->adj);
    free(g);
}