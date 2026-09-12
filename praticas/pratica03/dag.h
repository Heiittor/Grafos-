#ifndef DAG_H
#define DAG_H

/*
 * Representação de um digrafo por LISTA DE ADJACÊNCIA.
 * Cada vértice é um índice de 0 a num_vertices-1.
 * adj[v] é uma lista encadeada com os vértices para os quais v tem aresta.
 */

typedef struct No {
    int vertice;
    struct No *prox;
} No;

typedef struct {
    int num_vertices;
    No **adj; /* vetor de listas de adjacência, tamanho num_vertices */
} GrafoLista;

/* ---------- Funções básicas do grafo ---------- */
GrafoLista *criar_grafo(int num_vertices);
void destruir_grafo(GrafoLista *g);
void adicionar_aresta(GrafoLista *g, int origem, int destino);
void imprimir_grafo(GrafoLista *g);

/* ---------- Funções da prática ---------- */

/* Retorna 1 se o grafo é acíclico (DAG), 0 caso contrário. */
int eh_dag(GrafoLista *g);

/*
 * Ordenação topológica pelo algoritmo de Kahn (BFS com grau de entrada).
 * Preenche *tamanho com o número de elementos do vetor retornado.
 * Retorna NULL (e *tamanho = 0) se o grafo tiver ciclo.
 * O vetor retornado é alocado com malloc; quem chama deve dar free().
 */
int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho);

/*
 * Ordenação topológica via DFS (empilha cada vértice na saída da DFS
 * e depois inverte a pilha).
 * Mesma convenção de retorno/erro da função acima.
 */
int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho);

#endif