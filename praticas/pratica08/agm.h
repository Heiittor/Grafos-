#ifndef AGM_H
#define AGM_H

typedef struct {
    int *pai, *rank;
    int n;
} DSU;

typedef struct {
    int u, v, peso;
} Aresta;

typedef struct NoPeso {
    int destino, peso;
    struct NoPeso *prox;
} NoPeso;

typedef struct {
    int n;
    NoPeso **adj;
} GrafoListaPonderado;

DSU criar_dsu(int n);
void liberar_dsu(DSU *d);
int find(DSU *d, int x);
int unir(DSU *d, int x, int y);

GrafoListaPonderado *criar_grafo(int n, const Aresta *arestas, int m);
void liberar_grafo(GrafoListaPonderado *g);

/* Grafo nao direcionado. resultado tem espaco para n-1 arestas.
   Retornam o numero selecionado; custo usa long long.
   Se o grafo for desconectado, retornam uma floresta geradora minima.
   Kruskal nao altera o vetor de entrada; O(E log E).
   Prim usa lista de adjacencia e heap indexado; O((V+E) log V). */
int kruskal(int n, const Aresta *arestas, int m,
            Aresta *resultado, long long *custo);
int prim(GrafoListaPonderado *g, int origem,
         Aresta *resultado, long long *custo);

#endif
