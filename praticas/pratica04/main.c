#include <stdio.h>
#include <stdlib.h>

// TRUQUE DO CI: Incluindo o .c da prática 01 diretamente para o GitHub Actions compilar tudo junto
#include "../pratica01/grafo_lista.c" 

#include "conectividade.h"
#include "planaridade.h"

int main() {
    int n = 6;
    GrafoLista *g = criar_grafo_lista(n);

    // Construindo um grafo que possui ponte e articulação
    inserir_aresta_lista(g, 0, 1);
    inserir_aresta_lista(g, 1, 2);
    inserir_aresta_lista(g, 2, 0); // Ciclo 0-1-2
    
    inserir_aresta_lista(g, 2, 3); // Ponte (vértice 2 é articulação)
    
    inserir_aresta_lista(g, 3, 4);
    inserir_aresta_lista(g, 4, 5);
    inserir_aresta_lista(g, 5, 3); // Ciclo 3-4-5

    detectar_articulacoes_e_pontes(g);

    printf("\n=== Teste de Planaridade ===\n");
    if (!eh_planar_euler(g)) {
        printf("  -> Grafo NÃO é planar pela fórmula de Euler (m > 3n - 6).\n");
    } else {
        printf("  -> Grafo atende à restrição de Euler (m <= 3n - 6).\n");
        if (g->n <= 10) {
            if (!heuristica_kuratowski(g)) {
                printf("  -> Grafo NÃO é planar pela heurística de Kuratowski (Contém provável K5 ou K3,3).\n");
            } else {
                printf("  -> Grafo é planar (Heurística de Kuratowski passou).\n");
            }
        }
    }

    liberar_grafo_lista(g);
    return 0;
}