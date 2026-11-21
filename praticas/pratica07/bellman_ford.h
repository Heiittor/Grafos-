#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

/* Grafo direcionado: cada chamada insere somente origem -> destino. */
typedef struct NoPeso {
    int destino, peso;
    struct NoPeso *prox;
} NoPeso;

typedef struct {
    int n;
    NoPeso **adj;
    int vertice_ciclo; /* Ultimo vertice relaxado na n-esima passagem, ou -1. */
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo_ponderado(int n);
void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

/* Retorna 1 sem ciclo negativo alcancavel; 0 se existir tal ciclo.
   dist/pred possuem n elementos. INT_MAX significa inalcancavel.
   Usa long long internamente; distancias finitas fora da faixa int sao
   saturadas em INT_MIN ou INT_MAX-1. Com ciclo, dist nao e caminho minimo. */
int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/* Use g->vertice_ciclo apos retorno 0. Retrocede n vezes por pred,
   imprime as arestas no sentido correto e repete o primeiro vertice.
   Retorna 1 ao imprimir; 0 se os predecessores nao contiverem um ciclo. */
int reconstruir_ciclo_negativo(int n, int vertice, const int *pred);

#endif
