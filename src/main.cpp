#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <cstring> // Necessário para memset
#include <chrono>  // Necessário para medir o tempo

using namespace std;

// Número máximo de vértices (o suficiente para ambos os exemplos)
#define MAX_V 6

// Variáveis globais para o grafo
int V; // Número atual de vértices
int capacity[MAX_V][MAX_V];
int original_capacity[MAX_V][MAX_V];
vector<int> adj[MAX_V];

// Função para limpar e preparar o grafo para um novo cenário
void resetGraph(int num_vertices) {
    V = num_vertices;
    for(int i=0; i<MAX_V; ++i) adj[i].clear();
    memset(capacity, 0, sizeof(capacity));
    memset(original_capacity, 0, sizeof(original_capacity));
}

// Adiciona uma aresta direcionada com capacidade
void addEdge(int u, int v, int cap) {
    bool exists = false;
    for(int neighbor : adj[u]) if(neighbor == v) exists = true;
    if(!exists) {
        adj[u].push_back(v);
        adj[v].push_back(u); // Adiciona aresta reversa para o grafo residual
    }
    capacity[u][v] = cap;
    original_capacity[u][v] = cap;
}

// Busca em Largura (BFS) para Edmonds-Karp
bool bfs(int s, int t, vector<int>& parent) {
    fill(parent.begin(), parent.end(), -1);
    parent[s] = -2;
    queue<pair<int, int>> q;
    q.push({s, 2e9});

    while (!q.empty()) {
        int u = q.front().first;
        int flow = q.front().second;
        q.pop();

        for (int v : adj[u]) {
            if (parent[v] == -1 && capacity[u][v] > 0) {
                parent[v] = u;
                int new_flow = min(flow, capacity[u][v]);
                if (v == t)
                    return new_flow;
                q.push({v, new_flow});
            }
        }
    }
    return 0;
}

// Busca em Profundidade (DFS) para Ford-Fulkerson Clássico
int dfs(int u, int t, int flow, vector<int>& parent, vector<bool>& visited) {
    if (u == t) return flow;
    visited[u] = true;
    for (int v : adj[u]) {
        if (!visited[v] && capacity[u][v] > 0) {
            parent[v] = u;
            int new_flow = min(flow, capacity[u][v]);
            int pushed = dfs(v, t, new_flow, parent, visited);
            if (pushed > 0) return pushed;
        }
    }
    return 0;
}

// Função principal do algoritmo de Ford-Fulkerson
int max_flow(int s, int t, bool useBFS) {
    // --- RESET DO ESTADO DO GRAFO ---
    // Restaura as capacidades para o estado original antes de cada execução
    for(int u=0; u<V; ++u) {
        for(int v=0; v<V; ++v) {
           if(capacity[u][v] < original_capacity[u][v]) {
               int flow_passed = original_capacity[u][v] - capacity[u][v];
               capacity[v][u] -= flow_passed;
               capacity[u][v] = original_capacity[u][v];
           }
        }
    }
    // --------------------------------

    int flow = 0;
    vector<int> parent(V);
    int new_flow;
    long long iteracoes = 0;

    while (true) {
        if (useBFS) {
            new_flow = bfs(s, t, parent);
        } else {
            vector<bool> visited(V, false);
            new_flow = dfs(s, t, 2e9, parent, visited);
        }

        if (new_flow == 0) break;

        flow += new_flow;
        iteracoes++;
        int cur = t;
        while (cur != s) {
            int prev = parent[cur];
            capacity[prev][cur] -= new_flow;
            capacity[cur][prev] += new_flow;
            cur = prev;
        }
    }
    cout << "    -> Iterações (caminhos encontrados): " << iteracoes << endl;
    return flow;
}

// Função auxiliar para rodar e medir o tempo
void runAndMeasure(int s, int t, bool useBFS, string methodName) {
    cout << "  Executando " << methodName << "..." << endl;
    auto start = chrono::high_resolution_clock::now();
    int maxFlow = max_flow(s, t, useBFS);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "    -> Fluxo Máximo: " << maxFlow << endl;
    cout << "    -> Tempo: " << duration.count() << " microssegundos\n" << endl;
}

int main() {
    // ==================================================================================
    // CENÁRIO 1: GRAFO NORMAL (EXEMPLO CLÁSSICO)
    // ==================================================================================
    resetGraph(6); // Configura para 6 vértices
    addEdge(0, 1, 16); addEdge(0, 2, 13);
    addEdge(1, 2, 10); addEdge(1, 3, 12);
    addEdge(0, 4, 4);  addEdge(2, 4, 14);
    addEdge(3, 2, 9);  addEdge(4, 3, 7);
    addEdge(3, 5, 20); addEdge(4, 5, 4);
    int s_norm = 0, t_norm = 5;

    cout << "=========================================================" << endl;
    cout << "CENÁRIO 1: GRAFO NORMAL (Exemplo Clássico - 6 nós)" << endl;
    cout << "Fluxo Máximo Esperado: 23" << endl;
    cout << "=========================================================\n" << endl;

    runAndMeasure(s_norm, t_norm, true, "Edmonds-Karp (BFS)");
    runAndMeasure(s_norm, t_norm, false, "Ford-Fulkerson (DFS)");


    // ==================================================================================
    // CENÁRIO 2: CASO PATOLÓGICO (Pior caso para DFS)
    // ==================================================================================
    resetGraph(4); // Configura para 4 vértices
    int M = 100000; // Capacidade grande
    addEdge(0, 1, M); addEdge(0, 2, M);
    addEdge(1, 3, M); addEdge(2, 3, M);
    addEdge(1, 2, 1); // Aresta "ponte" de capacidade 1
    int s_pat = 0, t_pat = 3;

    cout << "\n=========================================================" << endl;
    cout << "CENÁRIO 2: CASO PATOLÓGICO (4 nós, Capacidade M=" << M << ")" << endl;
    cout << "Fluxo Máximo Esperado: " << 2 * M << endl;
    cout << "=========================================================\n" << endl;

    runAndMeasure(s_pat, t_pat, true, "Edmonds-Karp (BFS)");
    
    // ATENÇÃO: Esta execução pode levar alguns segundos se M for muito grande.
    runAndMeasure(s_pat, t_pat, false, "Ford-Fulkerson (DFS)");

    cout << "=========================================================" << endl;
    cout << "Fim dos Testes." << endl;
    cout << "=========================================================" << endl;

    return 0;
}