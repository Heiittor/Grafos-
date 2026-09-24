#include <stdio.h>
#include <stdlib.h>
#include "../pratica01/grafo_lista.c"  
#include "busca_largura.h"
#include "busca_profundidade.h"

int main() {
    int n = 6;
    GrafoLista *g = criar_grafo_lista(n);

    // Construindo um grafo de teste
    inserir_aresta_lista(g, 0, 1);
    inserir_aresta_lista(g, 0, 2);
    inserir_aresta_lista(g, 1, 3);
    inserir_aresta_lista(g, 2, 4);

    printf("=== Testando Busca em Largura (BFS) ===\n");
    int dist[n], pred[n];
    bfs(g, 0, dist, pred);
    for (int i = 0; i < n; i++) {
        printf("Vértice %d -> Distância: %d, Predecessor: %d\n", i, dist[i], pred[i]);
    }

    printf("\n=== Testando Busca em Profundidade (DFS) ===\n");
    int visitado[n], d[n], f[n];
    int tempo = 0; // A variável que estava faltando!

    // Inicializando os vetores da DFS com zero
    for(int i = 0; i < n; i++) {
        visitado[i] = 0;
        d[i] = 0;
        f[i] = 0;
    }

    dfs_recursiva(g, 0, visitado, d, f, &tempo);
    for (int i = 0; i < n; i++) {
        printf("Vértice %d -> Entrada (d): %d, Saída (f): %d\n", i, d[i], f[i]);
    }

    printf("\n=== Propriedades do Grafo ===\n");
    printf("Número de componentes conexos: %d\n", contar_componentes(g));
    printf("Possui ciclo? %s\n", tem_ciclo(g) ? "Sim" : "Não");
    printf("É bipartido? %s\n", eh_bipartido(g) ? "Sim" : "Não");

    // Adicionando uma aresta para criar ciclo e testar novamente
    inserir_aresta_lista(g, 3, 2);
    printf("\n--- Após adicionar aresta (3-2) ---\n");
    printf("Possui ciclo? %s\n", tem_ciclo(g) ? "Sim" : "Não");

    liberar_grafo_lista(g);
    return 0;
}