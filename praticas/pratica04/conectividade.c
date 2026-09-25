#include <stdio.h>
#include <stdlib.h>
#include "conectividade.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static void dfs_articulacoes_pontes(GrafoLista *g, int u, int *visitado, int *descoberta, int *low, int *pai, int *articulacao, int *tempo) {
    int filhos = 0;
    visitado[u] = 1;
    descoberta[u] = low[u] = ++(*tempo);

    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;

        if (!visitado[v]) {
            filhos++;
            pai[v] = u;
            dfs_articulacoes_pontes(g, v, visitado, descoberta, low, pai, articulacao, tempo);

            low[u] = MIN(low[u], low[v]);

            // Detecção de articulação
            if (pai[u] == -1 && filhos > 1) {
                articulacao[u] = 1;
            }
            if (pai[u] != -1 && low[v] >= descoberta[u]) {
                articulacao[u] = 1;
            }

            // Detecção de ponte
            if (low[v] > descoberta[u]) {
                printf("  -> Ponte detectada entre os vértices %d e %d\n", u, v);
            }
        } else if (v != pai[u]) {
            low[u] = MIN(low[u], descoberta[v]);
        }
        atual = atual->prox;
    }
}

void detectar_articulacoes_e_pontes(GrafoLista *g) {
    if (g == NULL) return;

    int *visitado = (int*) calloc(g->n, sizeof(int));
    int *descoberta = (int*) calloc(g->n, sizeof(int));
    int *low = (int*) calloc(g->n, sizeof(int));
    int *pai = (int*) malloc(g->n * sizeof(int));
    int *articulacao = (int*) calloc(g->n, sizeof(int));
    int tempo = 0;

    for (int i = 0; i < g->n; i++) pai[i] = -1;

    printf("\n=== Analisando Pontes ===\n");
    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            dfs_articulacoes_pontes(g, i, visitado, descoberta, low, pai, articulacao, &tempo);
        }
    }

    printf("\n=== Analisando Articulações ===\n");
    int encontrou_art = 0;
    for (int i = 0; i < g->n; i++) {
        if (articulacao[i]) {
            printf("  -> Vértice de corte (articulação) detectado: %d\n", i);
            encontrou_art = 1;
        }
    }
    if (!encontrou_art) printf("  -> Nenhuma articulação encontrada.\n");

    free(visitado);
    free(descoberta);
    free(low);
    free(pai);
    free(articulacao);
}