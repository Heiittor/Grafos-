#include <stdio.h>
#include <stdlib.h>
#include "planaridade.h"

// Conta o total de arestas de um grafo não direcionado
static int contar_arestas(GrafoLista *g) {
    int arestas = 0;
    for (int i = 0; i < g->n; i++) {
        No *atual = g->adj[i];
        while (atual != NULL) {
            arestas++;
            atual = atual->prox;
        }
    }
    return arestas / 2; // Como é não direcionado, divide por 2
}

int eh_planar_euler(GrafoLista *g) {
    if (g == NULL) return 0;
    
    int m = contar_arestas(g);
    int n = g->n;

    // Para grafos com menos de 3 vértices, a fórmula não se aplica diretamente (são sempre planares)
    if (n < 3) return 1;

    // Condição de Euler: m <= 3n - 6
    if (m <= 3 * n - 6) {
        return 1;
    }
    return 0;
}

int heuristica_kuratowski(GrafoLista *g) {
    if (g == NULL || g->n > 10) return 1; // Só analisa força bruta para n <= 10

    int n = g->n;
    int *graus = (int*) calloc(n, sizeof(int));
    int vertices_grau_alto = 0;

    // Conta os graus de todos os vértices
    for (int i = 0; i < n; i++) {
        No *atual = g->adj[i];
        while (atual != NULL) {
            graus[i]++;
            atual = atual->prox;
        }
        if (graus[i] >= 4) vertices_grau_alto++;
    }

    free(graus);

    // Heurística básica: Se existem 5 ou mais vértices com grau >= 4, é muito provável conter um K5
    if (vertices_grau_alto >= 5) {
        return 0; // Não é planar (suspeita de K5)
    }

    // Heurística básica para K3,3: Exige pelo menos 6 vértices com grau >= 3
    // Como a verificação real de K3,3 exige busca de isomorfismo complexa, 
    // confiamos no teste de Euler como filtro principal para essa prática.
    
    return 1;
}