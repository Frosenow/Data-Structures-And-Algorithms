#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

const int MAXN = 1000; // maksymalna liczba wierzchołków

int parent[MAXN]; // tablica przechowująca rodziców wierzchołków
int rank_[MAXN]; // tablica przechowująca rozmiary zbiorów wierzchołków

struct Edge {
    int u, v, weight; // wierzchołki połączone krawędzią oraz jej waga
};

Edge edges[MAXN*MAXN]; // tablica przechowująca krawędzie grafu
int num_edges = 0; // liczba krawędzi w grafie

int find_set(int v) {
    if (v == parent[v]) {
        return v;
    }
    return parent[v] = find_set(parent[v]);
}

void union_sets(int u, int v) {
    u = find_set(u);
    v = find_set(v);
    if (u != v) {
        if (rank_[u] < rank_[v])
            swap(u, v);
        parent[v] = u;
        if (rank_[u] == rank_[v])
            rank_[u]++;
    }
}

bool cmp(Edge a, Edge b) {
    return a.weight < b.weight;
}

void kruskal(int n) {
    for (int i = 0; i < n; i++) {
        parent[i] = i;
        rank_[i] = 0;
    }

    sort(edges, edges+num_edges, cmp);

    int mst_weight = 0; // waga MST
    for (int i = 0; i < num_edges; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        int weight = edges[i].weight;
        if (find_set(u) != find_set(v)) {
            union_sets(u, v);
            mst_weight += weight;
        }
    }

    cout << "Waga MST: " << mst_weight << endl;
}

void print_mst_edges(int n) {
    
}

int main() {
    ifstream infile("matrixTest.txt");
    int nodes;
    nodes = 6;

    // Odczyt macierzy
    vector<vector<int>> graph(nodes, vector<int>(nodes));
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            infile >> graph[i][j];
            // std::cout<<graph[i][j]<<" "; 
            if (graph[i][j] > 0) {
                edges[num_edges].u = i;
                edges[num_edges].v = j;
                edges[num_edges].weight = graph[i][j];
                num_edges++;
            }
        }
        // Tworzenie podmacierzy jesli macierz wejsciowa jest wieksza niz zadeklarowana macierz (nodes x nodes)
        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        // std::cout<<std::endl;
    }

    kruskal(nodes);
    return 0;
}
