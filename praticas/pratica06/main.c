#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

int main() {
    int n = 5;
    GrafoListaPonderado *g = criar_grafo_ponderado(n);

    // Grafo de teste ponderado
    inserir_aresta_ponderada(g, 0, 1, 4);
    inserir_aresta_ponderada(g, 0, 2, 1);
    inserir_aresta_ponderada(g, 2, 1, 2);
    inserir_aresta_ponderada(g, 1, 3, 1);
    inserir_aresta_ponderada(g, 2, 3, 5);
    inserir_aresta_ponderada(g, 3, 4, 3);

    int origem = 0;
    int dist[n], pred[n];

    dijkstra(g, origem, dist, pred);

    printf("=== Algoritmo de Dijkstra ===\n");
    for (int i = 0; i < n; i++) {
        printf("Menor distância da Origem %d ao Vértice %d: %d\n", origem, i, dist[i]);
    }

    printf("\n=== Reconstrução dos Caminhos Mínimos ===\n");
    for (int i = 1; i < n; i++) {
        reconstruir_caminho(origem, i, pred);
    }

    liberar_grafo_ponderado(g);
    return 0;
}// Atualizacao
