#ifndef DIJKSTRA_H
#define DIJKSTRA_H

/* Grafo nao direcionado, com vertices de 0 a n - 1 e pesos nao negativos. */
typedef struct NoPeso {
    int destino;
    int peso;
    struct NoPeso *prox;
} NoPeso;

typedef struct {
    int n;
    NoPeso **adj;
} GrafoListaPonderado;

GrafoListaPonderado* criar_grafo_ponderado(int n);
void inserir_aresta_ponderada(GrafoListaPonderado *g, int origem, int destino, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

/* Estruturas exigidas pela Issue #11. */
typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao; /* vertice -> indice no heap; -1 indica vertice ausente */
} MinHeap;

/* Criacao retorna NULL para capacidade invalida ou falha de alocacao. */
MinHeap* criar_min_heap(int capacidade);
/* Insere em O(log V); retorna 1 em caso de sucesso e 0 em caso de erro. */
int inserir_min_heap(MinHeap *minHeap, int vertice, int distancia);
/* Retorna o vertice minimo em O(log V), ou -1 se o heap estiver vazio. */
int extrair_minimo(MinHeap *minHeap);
/* O(log V); ignora vertices ausentes e tentativas de aumentar a chave. */
void diminuir_chave(MinHeap *minHeap, int vertice, int distancia);
void liberar_min_heap(MinHeap *minHeap);

/* dist e pred devem ter n elementos. INT_MAX representa distancia infinita
   ou nao representavel por int; -1 representa ausencia de predecessor.
   Custos finitos representaveis: 0 a INT_MAX - 1. */
void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred);
/* Imprime o caminho usando pred produzido por Dijkstra. origem e destino
   devem ser indices validos nesse vetor. Nao ha limite fixo de comprimento. */
void reconstruir_caminho(int origem, int destino, int *pred);

#endif
