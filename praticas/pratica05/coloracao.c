#include <stdio.h>
#include <stdlib.h>
#include "coloracao.h"

// Estrutura auxiliar para o Welsh-Powell (armazenar vértice e seu grau)
typedef struct {
    int vertice;
    int grau;
} VerticeGrau;

// Função de comparação para o qsort (ordem decrescente de grau)
static int comparar_grau(const void *a, const void *b) {
    VerticeGrau *va = (VerticeGrau*)a;
    VerticeGrau *vb = (VerticeGrau*)b;
    return vb->grau - va->grau;
}

// Algoritmo central de coloração gulosa aplicado a uma ordem específica de vértices
static int* aplicar_coloracao(GrafoLista *g, int *ordem, int *num_cores) {
    int *cor = (int*)malloc(g->n * sizeof(int));
    for (int i = 0; i < g->n; i++) cor[i] = -1;

    int *cor_disponivel = (int*)malloc(g->n * sizeof(int));
    *num_cores = 0;

    for (int i = 0; i < g->n; i++) {
        int u = ordem[i];

        // 1. Marca todas as cores como disponíveis
        for (int c = 0; c < g->n; c++) cor_disponivel[c] = 1;

        // 2. Verifica as cores dos vizinhos e as marca como indisponíveis
        No *atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->destino;
            if (cor[v] != -1) {
                cor_disponivel[cor[v]] = 0;
            }
            atual = atual->prox;
        }

        // 3. Encontra a primeira cor disponível
        int cr;
        for (cr = 0; cr < g->n; cr++) {
            if (cor_disponivel[cr]) break;
        }

        // 4. Atribui a cor ao vértice
        cor[u] = cr;
        if (cr + 1 > *num_cores) {
            *num_cores = cr + 1;
        }
    }

    free(cor_disponivel);
    return cor;
}

int* coloracao_gulosa(GrafoLista *g, int *num_cores) {
    if (g == NULL) return NULL;
    
    int *ordem = (int*)malloc(g->n * sizeof(int));
    for (int i = 0; i < g->n; i++) ordem[i] = i; // Ordem natural (0 a n-1)

    int *resultado = aplicar_coloracao(g, ordem, num_cores);
    free(ordem);
    return resultado;
}

int* coloracao_welsh_powell(GrafoLista *g, int *num_cores) {
    if (g == NULL) return NULL;

    VerticeGrau *vg = (VerticeGrau*)malloc(g->n * sizeof(VerticeGrau));
    
    // Calcula o grau de todos os vértices
    for (int i = 0; i < g->n; i++) {
        vg[i].vertice = i;
        vg[i].grau = 0;
        No *atual = g->adj[i];
        while (atual != NULL) {
            vg[i].grau++;
            atual = atual->prox;
        }
    }

    // Ordena os vértices por grau decrescente
    qsort(vg, g->n, sizeof(VerticeGrau), comparar_grau);

    // Cria o array de ordem baseado na ordenação
    int *ordem = (int*)malloc(g->n * sizeof(int));
    for (int i = 0; i < g->n; i++) {
        ordem[i] = vg[i].vertice;
    }

    int *resultado = aplicar_coloracao(g, ordem, num_cores);

    free(vg);
    free(ordem);
    return resultado;
}

int eh_bipartido(GrafoLista *g) {
    if (g == NULL) return 1;

    int *cor = (int*)malloc(g->n * sizeof(int));
    for (int i = 0; i < g->n; i++) cor[i] = -1;

    int *fila = (int*)malloc(g->n * sizeof(int));
    int inicio = 0, fim = 0;

    for (int i = 0; i < g->n; i++) {
        if (cor[i] == -1) {
            cor[i] = 0;
            fila[fim++] = i;

            while (inicio < fim) {
                int u = fila[inicio++];
                No *atual = g->adj[u];
                while (atual != NULL) {
                    int v = atual->destino;
                    if (cor[v] == -1) {
                        cor[v] = 1 - cor[u]; // Alterna entre 0 e 1
                        fila[fim++] = v;
                    } else if (cor[v] == cor[u]) {
                        free(cor);
                        free(fila);
                        return 0; // Achou dois vizinhos com a mesma cor
                    }
                    atual = atual->prox;
                }
            }
        }
    }

    free(cor);
    free(fila);
    return 1;
}