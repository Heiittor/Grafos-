#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

#define TESTE(c) do { if (!(c)) { \
    fprintf(stderr, "FALHA linha %d: %s\n", __LINE__, #c); \
    exit(EXIT_FAILURE); } } while (0)

/* Confere se o ciclo testemunhado usa arestas reais e tem custo negativo. */
static void verificar_ciclo(GrafoListaPonderado *g, const int *pred) {
    int v = g->vertice_ciclo;
    for (int i = 0; i < g->n; i++) {
        TESTE(v >= 0 && v < g->n);
        v = pred[v];
    }
    int inicio = v, passos = 0;
    long long custo = 0;
    do {
        TESTE(v >= 0 && v < g->n);
        int u = pred[v], menor = INT_MAX, existe = 0;
        TESTE(u >= 0 && u < g->n);
        for (NoPeso *p = g->adj[u]; p; p = p->prox) {
            if (p->destino == v && (!existe || p->peso < menor)) {
                menor = p->peso;
                existe = 1;
            }
        }
        TESTE(existe);
        custo += menor;
        v = u;
        TESTE(++passos <= g->n);
    } while (v != inicio);
    TESTE(custo < 0);
    TESTE(reconstruir_ciclo_negativo(g->n, g->vertice_ciclo, pred));
}

static void testar_sem_ciclo(void) {
    GrafoListaPonderado *g = criar_grafo_ponderado(6);
    const int esperado[] = {0, 2, 7, 4, -2, INT_MAX};
    int dist[6], pred[6];
    inserir_aresta_ponderada(g, 0, 1, 6);
    inserir_aresta_ponderada(g, 0, 2, 7);
    inserir_aresta_ponderada(g, 1, 2, 8);
    inserir_aresta_ponderada(g, 1, 3, 5);
    inserir_aresta_ponderada(g, 1, 4, -4);
    inserir_aresta_ponderada(g, 2, 3, -3);
    inserir_aresta_ponderada(g, 2, 4, 9);
    inserir_aresta_ponderada(g, 3, 1, -2);
    inserir_aresta_ponderada(g, 4, 0, 2);
    inserir_aresta_ponderada(g, 4, 3, 7);
    TESTE(bellman_ford(g, 0, dist, pred) == 1);
    for (int v = 0; v < 6; v++) {
        TESTE(dist[v] == esperado[v]);
        if (dist[v] == INT_MAX) printf("0 -> %d: inalcancavel\n", v);
        else printf("0 -> %d: %d\n", v, dist[v]);
    }
    TESTE(pred[0] == -1 && pred[5] == -1);
    TESTE(pred[1] == 3 && pred[2] == 0 && pred[3] == 2 && pred[4] == 1);
    TESTE(!reconstruir_ciclo_negativo(g->n, 4, pred));
    liberar_grafo_ponderado(g);
}

static void testar_ciclos(void) {
    int dist[5], pred[5];
    GrafoListaPonderado *g = criar_grafo_ponderado(5);
    inserir_aresta_ponderada(g, 0, 1, 1);
    inserir_aresta_ponderada(g, 1, 2, -3);
    inserir_aresta_ponderada(g, 2, 1, 1);
    inserir_aresta_ponderada(g, 2, 3, 2);
    inserir_aresta_ponderada(g, 3, 4, 2); /* Testemunha pode estar fora do ciclo. */
    TESTE(bellman_ford(g, 0, dist, pred) == 0);
    verificar_ciclo(g, pred);
    liberar_grafo_ponderado(g);

    g = criar_grafo_ponderado(5);
    inserir_aresta_ponderada(g, 0, 1, 0);
    inserir_aresta_ponderada(g, 3, 4, -2);
    inserir_aresta_ponderada(g, 4, 3, 1); /* Ciclo inicialmente inalcancavel. */
    TESTE(bellman_ford(g, 0, dist, pred) == 1);
    TESTE(dist[3] == INT_MAX && dist[4] == INT_MAX);
    TESTE(bellman_ford(g, 3, dist, pred) == 0);
    verificar_ciclo(g, pred);
    liberar_grafo_ponderado(g);
}

static void testar_limites(void) {
    int dist[3], pred[3];
    GrafoListaPonderado *g = criar_grafo_ponderado(3);
    inserir_aresta_ponderada(g, 2, 1, INT_MIN);
    inserir_aresta_ponderada(g, 1, 0, -1);
    TESTE(bellman_ford(g, 2, dist, pred) == 1);
    TESTE(dist[0] == INT_MIN && pred[0] == 1); /* Saturacao documentada. */
    inserir_aresta_ponderada(g, 0, 1, 0);
    TESTE(bellman_ford(g, 2, dist, pred) == 0); /* Nao perde ciclo por saturacao. */
    verificar_ciclo(g, pred);
    liberar_grafo_ponderado(g);

    g = criar_grafo_ponderado(1);
    TESTE(bellman_ford(g, 0, dist, pred) == 1 && dist[0] == 0);
    inserir_aresta_ponderada(g, 0, 0, -1);
    TESTE(bellman_ford(g, 0, dist, pred) == 0);
    verificar_ciclo(g, pred);
    liberar_grafo_ponderado(g);
}

int main(void) {
    testar_sem_ciclo();
    testar_ciclos();
    testar_limites();
    puts("Todos os testes da Pratica 07 passaram.");
    return EXIT_SUCCESS;
}
