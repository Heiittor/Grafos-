#include <stdio.h>
#include "grafo_matriz.h"
#include "grafo_lista.h"

int main() {
    int n = 5;

    printf("=== Testando Grafo por Matriz de Adjacência ===\n");
    GrafoMatriz *gm = criar_grafo_matriz(n);
    
    inserir_aresta_matriz(gm, 0, 1);
    inserir_aresta_matriz(gm, 0, 2);
    inserir_aresta_matriz(gm, 1, 3);

    printf("Grau do vértice 0: %d\n", grau_matriz(gm, 0));
    printf("0 e 1 são adjacentes? %s\n", sao_adjacentes_matriz(gm, 0, 1) ? "Sim" : "Não");
    printf("0 e 3 são adjacentes? %s\n", sao_adjacentes_matriz(gm, 0, 3) ? "Sim" : "Não");

    remover_aresta_matriz(gm, 0, 1);
    printf("0 e 1 são adjacentes após remoção? %s\n", sao_adjacentes_matriz(gm, 0, 1) ? "Sim" : "Não");
    
    liberar_grafo_matriz(gm);

    printf("\n=== Testando Grafo por Lista de Adjacência ===\n");
    GrafoLista *gl = criar_grafo_lista(n);
    
    inserir_aresta_lista(gl, 0, 1);
    inserir_aresta_lista(gl, 0, 2);
    inserir_aresta_lista(gl, 1, 3);

    printf("Grau do vértice 0: %d\n", grau_lista(gl, 0));
    printf("0 e 1 são adjacentes? %s\n", sao_adjacentes_lista(gl, 0, 1) ? "Sim" : "Não");
    printf("0 e 3 são adjacentes? %s\n", sao_adjacentes_lista(gl, 0, 3) ? "Sim" : "Não");

    remover_aresta_lista(gl, 0, 1);
    printf("0 e 1 são adjacentes após remoção? %s\n", sao_adjacentes_lista(gl, 0, 1) ? "Sim" : "Não");

    liberar_grafo_lista(gl);

    return 0;
}