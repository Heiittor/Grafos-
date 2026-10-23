#ifndef DIJKSTRA_H
#define DIJKSTRA_H

// --- Estrutura do Grafo Ponderado ---
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

// --- Estruturas Obrigatórias do Min-Heap ---
typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao; // mapeia vértice -> índice no heap para O(log V)
} MinHeap;

// --- Funções do Min-Heap e Dijkstra ---
MinHeap* criar_min_heap(int capacidade);
int extrair_minimo(MinHeap *minHeap);
void diminuir_chave(MinHeap *minHeap, int vertice, int distancia);
void liberar_min_heap(MinHeap *minHeap);

void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred);
void reconstruir_caminho(int origem, int destino, int *pred);

#endif