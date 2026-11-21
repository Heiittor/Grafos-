#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

/* As verificacoes continuam ativas mesmo se o programa usar -DNDEBUG. */
#define VERIFICAR(condicao) verificar((condicao), #condicao, __LINE__)

static void verificar(int condicao, const char *expressao, int linha) {
    if (!condicao) {
        fprintf(stderr, "FALHA na linha %d: %s\n", linha, expressao);
        exit(EXIT_FAILURE);
    }
}

static void verificar_heap(MinHeap *m) {
    for (int i = 0; i < m->tamanho; i++) {
        VERIFICAR(m->posicao[m->dados[i].vertice] == i);
        if (i > 0) {
            VERIFICAR(m->dados[(i - 1) / 2].distancia <= m->dados[i].distancia);
        }
    }
    for (int v = 0; v < m->capacidade; v++) {
        int i = m->posicao[v];
        VERIFICAR(i == -1 || (i >= 0 && i < m->tamanho));
        if (i != -1) VERIFICAR(m->dados[i].vertice == v);
    }
}

static void testar_heap(void) {
    const int chaves[] = {40, 10, 50, 30, 20, 60};
    const int ordem[] = {5, 1, 4, 3, 0, 2};
    MinHeap *m = criar_min_heap(6);
    VERIFICAR(m != NULL);
    VERIFICAR(extrair_minimo(m) == -1);
    verificar_heap(m);

    for (int v = 0; v < 6; v++) {
        VERIFICAR(inserir_min_heap(m, v, chaves[v]));
        verificar_heap(m);
    }
    VERIFICAR(!inserir_min_heap(m, 0, 0));
    VERIFICAR(!inserir_min_heap(m, -1, 0));
    VERIFICAR(!inserir_min_heap(m, 6, 0));
    diminuir_chave(m, 5, 1);
    diminuir_chave(m, 1, 100); /* Aumento nao deve alterar a chave 10. */
    verificar_heap(m);

    for (int i = 0; i < 6; i++) {
        VERIFICAR(extrair_minimo(m) == ordem[i]);
        VERIFICAR(m->posicao[ordem[i]] == -1);
        diminuir_chave(m, ordem[i], 0); /* Vertice extraido: nenhuma alteracao. */
        verificar_heap(m);
    }
    VERIFICAR(extrair_minimo(m) == -1);
    VERIFICAR(inserir_min_heap(m, 2, 7));
    VERIFICAR(!inserir_min_heap(m, 2, 3)); /* Duplicata com espaco disponivel. */
    VERIFICAR(extrair_minimo(m) == 2);
    VERIFICAR(m->posicao[2] == -1); /* Regressao da ultima extracao. */
    liberar_min_heap(m);
    VERIFICAR(criar_min_heap(0) == NULL);
    printf("OK: insercao, extracao, decrease-key e mapeamento do Min-Heap.\n");
}

static void testar_exemplo(void) {
    const int esperado[] = {0, 3, 1, 4, 7};
    const int predecessores[] = {-1, 2, 0, 1, 3};
    const int esperado_origem_3[] = {4, 1, 3, 0, 3};
    int dist[5], pred[5];
    GrafoListaPonderado *g = criar_grafo_ponderado(5);
    VERIFICAR(g != NULL);
    inserir_aresta_ponderada(g, 0, 1, 4);
    inserir_aresta_ponderada(g, 0, 2, 1);
    inserir_aresta_ponderada(g, 2, 1, 2);
    inserir_aresta_ponderada(g, 1, 3, 1);
    inserir_aresta_ponderada(g, 2, 3, 5);
    inserir_aresta_ponderada(g, 3, 4, 3);

    dijkstra(g, 0, dist, pred);
    printf("\n=== Exemplo: origem 0, distancias esperadas 0, 3, 1, 4, 7 ===\n");
    for (int v = 0; v < 5; v++) {
        VERIFICAR(dist[v] == esperado[v]);
        VERIFICAR(pred[v] == predecessores[v]);
        printf("Distancia de 0 a %d: %d\n", v, dist[v]);
        reconstruir_caminho(0, v, pred);
    }
    dijkstra(g, 3, dist, pred);
    for (int v = 0; v < 5; v++) VERIFICAR(dist[v] == esperado_origem_3[v]);
    VERIFICAR(pred[3] == -1);
    reconstruir_caminho(3, 0, pred);
    liberar_grafo_ponderado(g);
    printf("OK: caminhos minimos e origem diferente de zero.\n");
}

static void testar_casos_especiais(void) {
    const int esperado[] = {0, 0, 0, 2, INT_MAX, INT_MAX};
    int dist[6], pred[6];
    GrafoListaPonderado *g = criar_grafo_ponderado(6);
    VERIFICAR(g != NULL);
    inserir_aresta_ponderada(g, 0, 1, 9);
    inserir_aresta_ponderada(g, 0, 1, 0); /* Arestas paralelas. */
    inserir_aresta_ponderada(g, 1, 2, 0);
    inserir_aresta_ponderada(g, 2, 0, 0); /* Ciclo com custo zero. */
    inserir_aresta_ponderada(g, 2, 2, 0); /* Laco. */
    inserir_aresta_ponderada(g, 2, 3, 2);
    inserir_aresta_ponderada(g, 1, 3, 2); /* Empate entre caminhos minimos. */
    inserir_aresta_ponderada(g, 4, 5, 1); /* Componente desconectado. */

    dijkstra(g, 0, dist, pred);
    for (int v = 0; v < 6; v++) VERIFICAR(dist[v] == esperado[v]);
    VERIFICAR(pred[0] == -1 && pred[4] == -1 && pred[5] == -1);
    /* Verifica que os predecessores dos alcancaveis chegam a origem sem ciclo. */
    for (int v = 1; v <= 3; v++) {
        int atual = v;
        int passos = 0;
        while (atual != 0 && passos < g->n) {
            VERIFICAR(atual >= 0 && atual < g->n);
            atual = pred[atual];
            passos++;
        }
        VERIFICAR(atual == 0);
    }
    reconstruir_caminho(0, 3, pred);
    reconstruir_caminho(0, 4, pred);
    liberar_grafo_ponderado(g);
    printf("OK: pesos zero, empates, laco, arestas paralelas e desconexao.\n");
}

static void testar_limite_inteiro(void) {
    int dist[5], pred[5];
    GrafoListaPonderado *g = criar_grafo_ponderado(5);
    VERIFICAR(g != NULL);
    inserir_aresta_ponderada(g, 0, 1, INT_MAX - 5);
    inserir_aresta_ponderada(g, 1, 2, 10); /* Soma excede INT_MAX. */
    inserir_aresta_ponderada(g, 0, 2, INT_MAX - 1);
    inserir_aresta_ponderada(g, 1, 3, 5); /* Soma igual a sentinela. */
    inserir_aresta_ponderada(g, 1, 4, 10); /* Apenas rota nao representavel. */
    dijkstra(g, 0, dist, pred);
    VERIFICAR(dist[1] == INT_MAX - 5 && pred[1] == 0);
    VERIFICAR(dist[2] == INT_MAX - 1 && pred[2] == 0);
    VERIFICAR(dist[3] == INT_MAX && pred[3] == -1);
    VERIFICAR(dist[4] == INT_MAX && pred[4] == -1);
    liberar_grafo_ponderado(g);
    printf("OK: distancias grandes sem overflow de int.\n");
}

static void testar_vertice_unico(void) {
    int dist[1], pred[1];
    GrafoListaPonderado *g = criar_grafo_ponderado(1);
    VERIFICAR(g != NULL);
    dijkstra(g, 0, dist, pred);
    VERIFICAR(dist[0] == 0 && pred[0] == -1);
    reconstruir_caminho(0, 0, pred);
    liberar_grafo_ponderado(g);
    VERIFICAR(criar_grafo_ponderado(0) == NULL);
    printf("OK: grafo com um vertice e caminho da origem para ela mesma.\n");
}

int main(void) {
    testar_heap();
    testar_exemplo();
    testar_casos_especiais();
    testar_limite_inteiro();
    testar_vertice_unico();
    printf("\nTodos os testes da Pratica 06 passaram.\n");
    return EXIT_SUCCESS;
}
