#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

static void imprimir_ordem(const char *titulo, int *ordem, int tamanho) {
    printf("%s: ", titulo);
    if (ordem == NULL) {
        printf("impossivel (o grafo possui um ciclo)\n");
        return;
    }
    for (int i = 0; i < tamanho; i++) {
        printf("%d", ordem[i]);
        if (i < tamanho - 1) printf(" -> ");
    }
    printf("\n");
}

int main(void) {
    /* ---------- Grafo 1: um DAG válido ----------
     * 5 -> 2
     * 5 -> 0
     * 4 -> 0
     * 4 -> 1
     * 2 -> 3
     * 3 -> 1
     */
    printf("=== Grafo 1: DAG valido ===\n");
    GrafoLista *dag = criar_grafo(6);
    adicionar_aresta(dag, 5, 2);
    adicionar_aresta(dag, 5, 0);
    adicionar_aresta(dag, 4, 0);
    adicionar_aresta(dag, 4, 1);
    adicionar_aresta(dag, 2, 3);
    adicionar_aresta(dag, 3, 1);

    imprimir_grafo(dag);
    printf("\nEh DAG? %s\n\n", eh_dag(dag) ? "Sim" : "Nao");

    int tamanho;
    int *ordem_kahn = ordenacao_topologica_kahn(dag, &tamanho);
    imprimir_ordem("Kahn", ordem_kahn, tamanho);
    free(ordem_kahn);

    int *ordem_dfs = ordenacao_topologica_dfs(dag, &tamanho);
    imprimir_ordem("DFS ", ordem_dfs, tamanho);
    free(ordem_dfs);

    destruir_grafo(dag);

    /* ---------- Grafo 2: possui ciclo (0 -> 1 -> 2 -> 0) ---------- */
    printf("\n=== Grafo 2: possui ciclo ===\n");
    GrafoLista *ciclico = criar_grafo(3);
    adicionar_aresta(ciclico, 0, 1);
    adicionar_aresta(ciclico, 1, 2);
    adicionar_aresta(ciclico, 2, 0);

    imprimir_grafo(ciclico);
    printf("\nEh DAG? %s\n\n", eh_dag(ciclico) ? "Sim" : "Nao");

    ordem_kahn = ordenacao_topologica_kahn(ciclico, &tamanho);
    imprimir_ordem("Kahn", ordem_kahn, tamanho);
    free(ordem_kahn);

    ordem_dfs = ordenacao_topologica_dfs(ciclico, &tamanho);
    imprimir_ordem("DFS ", ordem_dfs, tamanho);
    free(ordem_dfs);

    destruir_grafo(ciclico);

    return 0;
}