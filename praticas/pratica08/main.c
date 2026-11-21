#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "agm.h"

#define TESTE(c) do { if (!(c)) { \
    fprintf(stderr, "FALHA linha %d: %s\n", __LINE__, #c); \
    exit(EXIT_FAILURE); } } while (0)

static void validar_floresta(int n, const Aresta *entrada, int m,
                             const Aresta *saida, int k, long long custo) {
    DSU d = criar_dsu(n);
    long long soma = 0;
    for (int i = 0; i < k; i++) {
        Aresta a = saida[i];
        int existe = 0;
        for (int j = 0; j < m; j++) {
            Aresta b = entrada[j];
            if (a.peso == b.peso &&
                ((a.u == b.u && a.v == b.v) || (a.u == b.v && a.v == b.u))) {
                existe = 1;
            }
        }
        TESTE(existe && unir(&d, a.u, a.v)); /* Sem ciclos. */
        soma += a.peso;
    }
    for (int i = 0; i < m; i++) {
        TESTE(find(&d, entrada[i].u) == find(&d, entrada[i].v));
    }
    TESTE(soma == custo);
    liberar_dsu(&d);
}

static void comparar(int n, const Aresta *arestas, int m,
                     int quantidade, long long esperado) {
    Aresta *k = calloc((size_t)n, sizeof *k);
    Aresta *p = calloc((size_t)n, sizeof *p);
    TESTE(k && p);
    long long ck, cp;
    GrafoListaPonderado *g = criar_grafo(n, arestas, m);
    int nk = kruskal(n, arestas, m, k, &ck);
    int np = prim(g, n - 1, p, &cp);
    TESTE(nk == quantidade && np == quantidade);
    TESTE(ck == esperado && cp == esperado);
    validar_floresta(n, arestas, m, k, nk, ck);
    validar_floresta(n, arestas, m, p, np, cp);
    printf("n=%d: Kruskal=%lld, Prim=%lld, arestas=%d\n", n, ck, cp, nk);
    for (int i = 0; i < nk; i++) {
        printf("  Kruskal: %d -- %d (peso %d)\n", k[i].u, k[i].v, k[i].peso);
    }
    for (int i = 0; i < np; i++) {
        printf("  Prim: %d -- %d (peso %d)\n", p[i].u, p[i].v, p[i].peso);
    }
    liberar_grafo(g);
    free(k);
    free(p);
}

static void testar_dsu(void) {
    DSU d = criar_dsu(8);
    TESTE(unir(&d, 0, 1) && unir(&d, 2, 3) && unir(&d, 0, 2));
    TESTE(unir(&d, 4, 5) && unir(&d, 6, 7) && unir(&d, 4, 6));
    TESTE(unir(&d, 0, 4));
    int raiz = find(&d, 7);
    TESTE(d.pai[7] == raiz && d.rank[raiz] == 3);
    for (int v = 0; v < 8; v++) TESTE(find(&d, v) == raiz);
    TESTE(!unir(&d, 3, 7) && d.rank[raiz] == 3);
    liberar_dsu(&d);
}

int main(void) {
    const Aresta classico[] = {
        {0,1,4}, {0,7,8}, {1,2,8}, {1,7,11}, {2,3,7}, {2,8,2},
        {2,5,4}, {3,4,9}, {3,5,14}, {4,5,10}, {5,6,2}, {6,7,1},
        {6,8,6}, {7,8,7}
    };
    const Aresta especiais[] = {
        {0,0,-100}, {0,1,9}, {0,1,-2}, {1,2,0}, {0,2,0}, {3,4,7}
    };
    const Aresta limites[] = {{0,1,INT_MAX}, {1,2,INT_MAX}};
    const Aresta negativos[] = {{0,1,INT_MIN}, {1,2,-1}, {0,2,INT_MAX}};
    testar_dsu();
    comparar(9, classico, 14, 8, 37);
    comparar(6, especiais, 6, 3, 5); /* Floresta com vertice isolado. */
    comparar(3, limites, 2, 2, 2LL * INT_MAX);
    comparar(3, negativos, 3, 2, (long long)INT_MIN - 1);
    comparar(1, NULL, 0, 0, 0);
    comparar(4, NULL, 0, 0, 0);
    puts("Todos os testes da Pratica 08 passaram.");
    return EXIT_SUCCESS;
}
