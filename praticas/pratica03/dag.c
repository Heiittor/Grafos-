#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

/* ================= Funções básicas do grafo ================= */

GrafoLista *criar_grafo(int num_vertices) {
    GrafoLista *g = malloc(sizeof(GrafoLista));
    g->num_vertices = num_vertices;
    g->adj = malloc(num_vertices * sizeof(No *));
    for (int i = 0; i < num_vertices; i++) {
        g->adj[i] = NULL;
    }
    return g;
}

void destruir_grafo(GrafoLista *g) {
    if (!g) return;
    for (int v = 0; v < g->num_vertices; v++) {
        No *atual = g->adj[v];
        while (atual != NULL) {
            No *tmp = atual;
            atual = atual->prox;
            free(tmp);
        }
    }
    free(g->adj);
    free(g);
}

void adicionar_aresta(GrafoLista *g, int origem, int destino) {
    No *novo = malloc(sizeof(No));
    novo->vertice = destino;
    novo->prox = g->adj[origem];
    g->adj[origem] = novo;
}

void imprimir_grafo(GrafoLista *g) {
    for (int v = 0; v < g->num_vertices; v++) {
        printf("%d ->", v);
        for (No *atual = g->adj[v]; atual != NULL; atual = atual->prox) {
            printf(" %d", atual->vertice);
        }
        printf("\n");
    }
}

/* ================= Detecção de ciclo (eh_dag) ================= */

/* Cores usadas na DFS para detectar ciclo em grafo direcionado:
 * BRANCO = não visitado
 * CINZA  = em processamento (está na pilha de recursão atual)
 * PRETO  = totalmente processado
 * Se durante a DFS encontramos um vizinho CINZA, existe um ciclo. */
typedef enum { BRANCO, CINZA, PRETO } Cor;

static int dfs_detecta_ciclo(GrafoLista *g, int v, Cor *cor) {
    cor[v] = CINZA;

    for (No *atual = g->adj[v]; atual != NULL; atual = atual->prox) {
        int u = atual->vertice;
        if (cor[u] == CINZA) {
            return 1; /* aresta de retorno -> ciclo */
        }
        if (cor[u] == BRANCO && dfs_detecta_ciclo(g, u, cor)) {
            return 1;
        }
    }

    cor[v] = PRETO;
    return 0;
}

int eh_dag(GrafoLista *g) {
    Cor *cor = malloc(g->num_vertices * sizeof(Cor));
    for (int i = 0; i < g->num_vertices; i++) {
        cor[i] = BRANCO;
    }

    int resultado = 1; /* assume DAG até provar o contrário */
    for (int i = 0; i < g->num_vertices; i++) {
        if (cor[i] == BRANCO) {
            if (dfs_detecta_ciclo(g, i, cor)) {
                resultado = 0;
                break;
            }
        }
    }

    free(cor);
    return resultado;
}

/* ================= Algoritmo de Kahn (BFS) ================= */

int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho) {
    int n = g->num_vertices;
    int *grau_entrada = calloc(n, sizeof(int));

    /* Calcula o grau de entrada de cada vértice */
    for (int v = 0; v < n; v++) {
        for (No *atual = g->adj[v]; atual != NULL; atual = atual->prox) {
            grau_entrada[atual->vertice]++;
        }
    }

    /* Fila (implementada com vetor) com todos os vértices de grau 0 */
    int *fila = malloc(n * sizeof(int));
    int inicio = 0, fim = 0;
    for (int v = 0; v < n; v++) {
        if (grau_entrada[v] == 0) {
            fila[fim++] = v;
        }
    }

    int *ordem = malloc(n * sizeof(int));
    int count = 0;

    while (inicio < fim) {
        int v = fila[inicio++];
        ordem[count++] = v;

        for (No *atual = g->adj[v]; atual != NULL; atual = atual->prox) {
            int u = atual->vertice;
            grau_entrada[u]--;
            if (grau_entrada[u] == 0) {
                fila[fim++] = u;
            }
        }
    }

    free(grau_entrada);
    free(fila);

    if (count != n) {
        /* Nem todos os vértices entraram na fila -> existe ciclo */
        free(ordem);
        *tamanho = 0;
        return NULL;
    }

    *tamanho = count;
    return ordem;
}

/* ================= Ordenação topológica via DFS ================= */

static void dfs_topologica(GrafoLista *g, int v, int *visitado,
                            int *pilha, int *topo) {
    visitado[v] = 1;

    for (No *atual = g->adj[v]; atual != NULL; atual = atual->prox) {
        int u = atual->vertice;
        if (!visitado[u]) {
            dfs_topologica(g, u, visitado, pilha, topo);
        }
    }

    /* Empilha o vértice na SAÍDA da DFS (quando ele termina de ser processado) */
    pilha[(*topo)++] = v;
}

int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho) {
    if (!eh_dag(g)) {
        *tamanho = 0;
        return NULL;
    }

    int n = g->num_vertices;
    int *visitado = calloc(n, sizeof(int));
    int *pilha = malloc(n * sizeof(int));
    int topo = 0;

    for (int v = 0; v < n; v++) {
        if (!visitado[v]) {
            dfs_topologica(g, v, visitado, pilha, &topo);
        }
    }

    free(visitado);

    /* A pilha está na ordem inversa: o topo (última posição) veio primeiro.
     * A ordenação topológica correta é a pilha lida de trás para frente. */
    int *ordem = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        ordem[i] = pilha[n - 1 - i];
    }
    free(pilha);

    *tamanho = n;
    return ordem;
}