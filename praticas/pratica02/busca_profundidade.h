#ifndef BUSCA_PROFUNDIDADE_H
#define BUSCA_PROFUNDIDADE_H

#include "../pratica01/grafo_lista.h"

typedef struct {
    int *dados;
    int topo;
    int capacidade;
} Pilha;

Pilha* criar_pilha(int capacidade);
void empilhar(Pilha *p, int v);
int desempilhar(Pilha *p);
int pilha_vazia(Pilha *p);
void liberar_pilha(Pilha *p);

void dfs_recursiva(GrafoLista *g, int u, int *visitado, int *d, int *f, int *tempo);
int contar_componentes(GrafoLista *g);
int tem_ciclo(GrafoLista *g);

#endif