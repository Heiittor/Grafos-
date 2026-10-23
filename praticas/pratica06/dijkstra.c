#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "dijkstra.h"

// ================= GRAFO PONDERADO =================
GrafoListaPonderado* criar_grafo_ponderado(int n) {
    GrafoListaPonderado *g = (GrafoListaPonderado*)malloc(sizeof(GrafoListaPonderado));
    g->n = n;
    g->adj = (NoPeso**)malloc(n * sizeof(NoPeso*));
    for (int i = 0; i < n; i++) g->adj[i] = NULL;
    return g;
}

void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso) {
    NoPeso *novo = (NoPeso*)malloc(sizeof(NoPeso));
    novo->destino = v;
    novo->peso = peso;
    novo->prox = g->adj[u];
    g->adj[u] = novo;
    
    // Assumindo grafo não direcionado para a prática
    NoPeso *novo2 = (NoPeso*)malloc(sizeof(NoPeso));
    novo2->destino = u;
    novo2->peso = peso;
    novo2->prox = g->adj[v];
    g->adj[v] = novo2;
}

void liberar_grafo_ponderado(GrafoListaPonderado *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) {
        NoPeso *atual = g->adj[i];
        while (atual != NULL) {
            NoPeso *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(g->adj);
    free(g);
}

// ================= MIN-HEAP =================
MinHeap* criar_min_heap(int capacidade) {
    MinHeap *m = (MinHeap*)malloc(sizeof(MinHeap));
    m->capacidade = capacidade;
    m->tamanho = 0;
    m->dados = (NoHeap*)malloc(capacidade * sizeof(NoHeap));
    m->posicao = (int*)malloc(capacidade * sizeof(int));
    return m;
}

static void trocar_no_heap(NoHeap *a, NoHeap *b) {
    NoHeap t = *a;
    *a = *b;
    *b = t;
}

static void min_heapify(MinHeap *m, int idx) {
    int menor = idx;
    int esq = 2 * idx + 1;
    int dir = 2 * idx + 2;

    if (esq < m->tamanho && m->dados[esq].distancia < m->dados[menor].distancia) menor = esq;
    if (dir < m->tamanho && m->dados[dir].distancia < m->dados[menor].distancia) menor = dir;

    if (menor != idx) {
        // Atualiza o mapeamento
        m->posicao[m->dados[menor].vertice] = idx;
        m->posicao[m->dados[idx].vertice] = menor;
        trocar_no_heap(&m->dados[menor], &m->dados[idx]);
        min_heapify(m, menor);
    }
}

int extrair_minimo(MinHeap *m) {
    if (m->tamanho == 0) return -1;

    int raiz = m->dados[0].vertice;
    NoHeap ultimo = m->dados[m->tamanho - 1];

    m->dados[0] = ultimo;
    m->posicao[raiz] = -1; // Vértice extraído
    m->posicao[ultimo.vertice] = 0;
    
    m->tamanho--;
    min_heapify(m, 0);

    return raiz;
}

void diminuir_chave(MinHeap *m, int vertice, int distancia) {
    int i = m->posicao[vertice];
    m->dados[i].distancia = distancia;

    while (i && m->dados[i].distancia < m->dados[(i - 1) / 2].distancia) {
        m->posicao[m->dados[i].vertice] = (i - 1) / 2;
        m->posicao[m->dados[(i - 1) / 2].vertice] = i;
        trocar_no_heap(&m->dados[i], &m->dados[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void liberar_min_heap(MinHeap *m) {
    free(m->dados);
    free(m->posicao);
    free(m);
}

// ================= DIJKSTRA =================
void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    int V = g->n;
    MinHeap *m = criar_min_heap(V);

    for (int v = 0; v < V; v++) {
        dist[v] = INT_MAX;
        pred[v] = -1;
        m->dados[v].vertice = v;
        m->dados[v].distancia = INT_MAX;
        m->posicao[v] = v;
    }

    m->dados[origem].distancia = 0;
    dist[origem] = 0;
    m->tamanho = V;
    diminuir_chave(m, origem, 0);

    while (m->tamanho > 0) {
        int u = extrair_minimo(m);

        NoPeso *atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->destino;
            if (m->posicao[v] != -1 && dist[u] != INT_MAX && atual->peso + dist[u] < dist[v]) {
                dist[v] = dist[u] + atual->peso;
                pred[v] = u;
                diminuir_chave(m, v, dist[v]);
            }
            atual = atual->prox;
        }
    }
    liberar_min_heap(m);
}

// ================= RECONSTRUÇÃO DO CAMINHO =================
void reconstruir_caminho(int origem, int destino, int *pred) {
    if (pred[destino] == -1 && origem != destino) {
        printf("Não há caminho entre %d e %d.\n", origem, destino);
        return;
    }
    
    int caminho[1000];
    int contador = 0;
    int atual = destino;
    
    while (atual != -1) {
        caminho[contador++] = atual;
        atual = pred[atual];
    }
    
    printf("Caminho (%d -> %d): ", origem, destino);
    for (int i = contador - 1; i >= 0; i--) {
        printf("%d", caminho[i]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}