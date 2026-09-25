#include <stdio.h>
#include <stdlib.h>

// TRUQUE DO CI: Incluindo o .c da prática 01 diretamente
#include "../pratica01/grafo_lista.c" 
#include "coloracao.h"

int main() {
    int n = 6;
    GrafoLista *g = criar_grafo_lista(n);

    // Construindo um grafo de teste (Estrela com um triângulo na ponta)
    // O triângulo garante que o número cromático será pelo menos 3 (não bipartido)
    inserir_aresta_lista(g, 0, 1);
    inserir_aresta_lista(g, 0, 2);
    inserir_aresta_lista(g, 0, 3);
    inserir_aresta_lista(g, 0, 4);
    inserir_aresta_lista(g, 0, 5);
    inserir_aresta_lista(g, 1, 2); // Aresta que cria o ciclo ímpar 0-1-2

    int num_cores_gulosa = 0;
    int *cores_gulosa = coloracao_gulosa(g, &num_cores_gulosa);

    printf("=== Coloração Gulosa (Ordem Arbitrária) ===\n");
    printf("Total de cores utilizadas: %d\n", num_cores_gulosa);
    for (int i = 0; i < n; i++) {
        printf("  -> Vértice %d pintado com a Cor %d\n", i, cores_gulosa[i]);
    }

    int num_cores_wp = 0;
    int *cores_wp = coloracao_welsh_powell(g, &num_cores_wp);

    printf("\n=== Coloração Welsh-Powell (Ordenado por Grau) ===\n");
    printf("Total de cores utilizadas: %d\n", num_cores_wp);
    for (int i = 0; i < n; i++) {
        printf("  -> Vértice %d pintado com a Cor %d\n", i, cores_wp[i]);
    }

    printf("\n=== Verificação de Bipartição ===\n");
    if (eh_bipartido(g)) {
        printf("  -> O grafo É bipartido (Número cromático = 2).\n");
    } else {
        printf("  -> O grafo NÃO é bipartido (Requer mais de 2 cores).\n");
    }

    free(cores_gulosa);
    free(cores_wp);
    liberar_grafo_lista(g);
    return 0;
}