#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    if (n <= 0) return NULL;

    GrafoListaPonderado *g = malloc(sizeof *g);
    if (!g) return NULL;

    g->n = n;
    g->adj = calloc((size_t)n, sizeof *g->adj);
    if (!g->adj) {
        free(g);
        return NULL;
    }
    return g;
}

void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso) {
    if (!g || u < 0 || u >= g->n || v < 0 || v >= g->n || peso < 0) {
        fprintf(stderr, "Aresta invalida: vertices fora do grafo ou peso negativo.\n");
        return;
    }

    NoPeso *ida = malloc(sizeof *ida);
    NoPeso *volta = malloc(sizeof *volta);
    if (!ida || !volta) {
        free(ida);
        free(volta);
        fprintf(stderr, "Sem memoria para inserir aresta.\n");
        return;
    }

    *ida = (NoPeso){v, peso, g->adj[u]};
    g->adj[u] = ida;
    *volta = (NoPeso){u, peso, g->adj[v]};
    g->adj[v] = volta;
}

void liberar_grafo_ponderado(GrafoListaPonderado *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) {
        NoPeso *atual = g->adj[i];
        while (atual) {
            NoPeso *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(g->adj);
    free(g);
}

MinHeap *criar_min_heap(int capacidade) {
    if (capacidade <= 0) return NULL;

    MinHeap *m = malloc(sizeof *m);
    if (!m) return NULL;

    m->capacidade = capacidade;
    m->tamanho = 0;
    m->dados = malloc((size_t)capacidade * sizeof *m->dados);
    m->posicao = malloc((size_t)capacidade * sizeof *m->posicao);
    if (!m->dados || !m->posicao) {
        liberar_min_heap(m);
        return NULL;
    }
    for (int v = 0; v < capacidade; v++) m->posicao[v] = -1;
    return m;
}

/* Toda troca mantem o mapeamento sincronizado, sem buscar vertices no heap. */
static void trocar_no_heap(MinHeap *m, int a, int b) {
    NoHeap temporario = m->dados[a];
    m->dados[a] = m->dados[b];
    m->dados[b] = temporario;
    m->posicao[m->dados[a].vertice] = a;
    m->posicao[m->dados[b].vertice] = b;
}

static void subir_no_heap(MinHeap *m, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (m->dados[pai].distancia <= m->dados[i].distancia) break;
        trocar_no_heap(m, i, pai);
        i = pai;
    }
}

int inserir_min_heap(MinHeap *m, int vertice, int distancia) {
    if (!m || vertice < 0 || vertice >= m->capacidade ||
        m->tamanho == m->capacidade || m->posicao[vertice] != -1) return 0;

    int i = m->tamanho++;
    m->dados[i] = (NoHeap){vertice, distancia};
    m->posicao[vertice] = i;
    subir_no_heap(m, i);
    return 1;
}

static void min_heapify(MinHeap *m, int i) {
    /* i < tamanho / 2 garante que existe ao menos um filho a esquerda. */
    while (i < m->tamanho / 2) {
        int menor = 2 * i + 1;
        int direita = menor + 1;
        if (direita < m->tamanho &&
            m->dados[direita].distancia < m->dados[menor].distancia) {
            menor = direita;
        }
        if (m->dados[i].distancia <= m->dados[menor].distancia) break;
        trocar_no_heap(m, i, menor);
        i = menor;
    }
}

int extrair_minimo(MinHeap *m) {
    if (!m || m->tamanho == 0) return -1;

    int vertice = m->dados[0].vertice;
    m->posicao[vertice] = -1;
    m->tamanho--;
    if (m->tamanho > 0) {
        m->dados[0] = m->dados[m->tamanho];
        m->posicao[m->dados[0].vertice] = 0;
        min_heapify(m, 0);
    }
    return vertice;
}

void diminuir_chave(MinHeap *m, int vertice, int distancia) {
    if (!m || vertice < 0 || vertice >= m->capacidade) return;

    int i = m->posicao[vertice];
    if (i == -1 || distancia >= m->dados[i].distancia) return;
    m->dados[i].distancia = distancia;
    subir_no_heap(m, i);
}

void liberar_min_heap(MinHeap *m) {
    if (!m) return;
    free(m->dados);
    free(m->posicao);
    free(m);
}

void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    if (!g || !dist || !pred) return;
    for (int v = 0; v < g->n; v++) {
        dist[v] = INT_MAX;
        pred[v] = -1;
    }
    if (origem < 0 || origem >= g->n) return;

    MinHeap *m = criar_min_heap(g->n);
    if (!m) {
        fprintf(stderr, "Sem memoria para executar Dijkstra.\n");
        return;
    }
    for (int v = 0; v < g->n; v++) inserir_min_heap(m, v, INT_MAX);
    dist[origem] = 0;
    diminuir_chave(m, origem, 0);

    while (m->tamanho > 0) {
        int u = extrair_minimo(m);
        if (dist[u] == INT_MAX) break; /* Os restantes sao inalcancaveis. */

        for (NoPeso *aresta = g->adj[u]; aresta; aresta = aresta->prox) {
            int v = aresta->destino;
            /* Verifica o limite antes da soma para evitar overflow de int. */
            if (m->posicao[v] != -1 && aresta->peso <= INT_MAX - dist[u]) {
                int alternativa = dist[u] + aresta->peso;
                if (alternativa < dist[v]) {
                    dist[v] = alternativa;
                    pred[v] = u;
                    diminuir_chave(m, v, alternativa);
                }
            }
        }
    }
    liberar_min_heap(m);
}

void reconstruir_caminho(int origem, int destino, int *pred) {
    if (!pred || origem < 0 || destino < 0) return;

    size_t tamanho = 1;
    int atual = destino;
    while (atual != origem) {
        atual = pred[atual];
        if (atual == -1) {
            printf("Nao ha caminho entre %d e %d.\n", origem, destino);
            return;
        }
        tamanho++;
    }

    int *caminho = malloc(tamanho * sizeof *caminho);
    if (!caminho) {
        fprintf(stderr, "Sem memoria para reconstruir caminho.\n");
        return;
    }
    atual = destino;
    for (size_t i = tamanho; i > 0; i--) {
        caminho[i - 1] = atual;
        if (i > 1) atual = pred[atual];
    }

    printf("Caminho (%d -> %d): ", origem, destino);
    for (size_t i = 0; i < tamanho; i++) {
        if (i > 0) printf(" -> ");
        printf("%d", caminho[i]);
    }
    printf("\n");
    free(caminho);
}
