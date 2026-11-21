#ifndef HAMILTONIANO_H
#define HAMILTONIANO_H

/* Grafo simples nao direcionado, representado por matriz de adjacencia. */
typedef struct {
    int n;
    unsigned char *adj;
} Grafo;

Grafo *criar_grafo(int n);
void inserir_aresta(Grafo *g, int u, int v);
int tem_aresta(const Grafo *g, int u, int v);
void liberar_grafo(Grafo *g);

/* Retornam 1 se encontrarem solucao, 0 caso contrario.
   caminho tem n posicoes; ciclo tem n+1 (repete o primeiro vertice).
   Caminho tenta todas as origens; ciclo fixa 0 e exige n >= 3. */
int encontrar_caminho_hamiltoniano_bt(const Grafo *g, int *caminho);
int encontrar_ciclo_hamiltoniano_bt(const Grafo *g, int *ciclo);

#endif
