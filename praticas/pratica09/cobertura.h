#ifndef COBERTURA_H
#define COBERTURA_H

/* Grafo nao direcionado; matriz booleana n x n, inclusive lacos. */
typedef struct {
    int n;
    unsigned char *adj;
} Grafo;

Grafo *criar_grafo(int n);
void inserir_aresta(Grafo *g, int u, int v);
void liberar_grafo(Grafo *g);
int tem_aresta(const Grafo *g, int u, int v);

/* selecionados e um vetor de n inteiros (0/1). Os algoritmos preenchem
   esse vetor e retornam sua cardinalidade, sem modificar o grafo.
   O algoritmo exato ramifica nos extremos de uma aresta descoberta e poda
   pelo melhor tamanho conhecido e por um limite inferior de emparelhamento. */
int cobertura_minima_bt(const Grafo *g, int *selecionados);
int cobertura_gulosa(const Grafo *g, int *selecionados);
int eh_cobertura(const Grafo *g, const int *selecionados);

#endif
