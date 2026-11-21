#ifndef TPS_H
#define TPS_H

#include "hamiltoniano.h"

/* Nome tps.h exigido pela Issue #16; funcoes preservam o prefixo tsp.
   Instancia completa, simetrica, custos int nao negativos, diagonal zero. */
typedef struct {
    int n;
    int *pesos; /* Matriz linear n x n. */
} TSP;

TSP *criar_tsp(int n, const int *pesos);
void liberar_tsp(TSP *t);
int peso_tsp(const TSP *t, int u, int v);

/* Converte arestas em custo 1 e nao arestas em custo 2; diagonal zero.
   Para n >= 3, custo otimo n equivale a existir ciclo Hamiltoniano. */
TSP *grafo_para_tsp(const Grafo *g);

/* Rotas possuem n+1 posicoes e repetem a origem no final.
   Custos sao long long. Retornam -1 para parametros invalidos.
   Forca bruta enumera todas as (n-1)! permutacoes com origem fixa.
   permutacoes pode ser NULL; caso contrario recebe o numero de folhas. */
long long tsp_forca_bruta(const TSP *t, int origem, int *rota,
                          unsigned long long *permutacoes);
long long tsp_vizinho_mais_proximo(const TSP *t, int origem, int *rota);
long long tsp_insercao(const TSP *t, int origem, int *rota);
/* Verifica fechamento e visita unica de todos os vertices; -1 se invalida. */
long long custo_tour(const TSP *t, const int *rota);

#endif
