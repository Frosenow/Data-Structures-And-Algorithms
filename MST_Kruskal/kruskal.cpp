#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <chrono>


using namespace std;

unordered_map<string, string> parse_config_file(const string& filename) {
    ifstream infile(filename);
    if (!infile.is_open()) {
        throw runtime_error("Error: cannot open file " + filename);
    }

    unordered_map<string, string> config;
    string line;
    while (getline(infile, line)) {
        if (line.empty() || line[0] == ';') {
            // skip comments and empty lines
            continue;
        }
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos == string::npos) {
            // skip lines without delimiter
            continue;
        }
        string key = line.substr(0, delimiter_pos);
        string value = line.substr(delimiter_pos + 1);
        // trim whitespace
        key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());
        value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());
        config[key] = value;
    }

    infile.close();
    return config;
}

const int MAXN = 1000; // Maksymalna ilosc wierzcholkow

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

void read_matrix(int n, ifstream& infile) {
    // Odczyt macierzy
    vector<vector<int>> graph(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            infile >> graph[i][j];
            std::cout<<graph[i][j]<<" "; 
            if (graph[i][j] > 0) {
                edges[num_edges].u = i;
                edges[num_edges].v = j;
                edges[num_edges].weight = graph[i][j];
                num_edges++;
            }
        }
        // Tworzenie podmacierzy jesli macierz wejsciowa jest wieksza niz zadeklarowana macierz (n x n)
        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cout<<std::endl;
    }
}

void perform_tests(int range_size, vector<int> ranges, string input_file){
    for(int i = 0; i < range_size; i++){
        int nodes = ranges[i];
        ifstream infile(input_file);
        std::cout << "Rozmiar macierzy: " << ranges[i] << "x" << ranges[i] << std::endl; 
        read_matrix(nodes, infile);
         auto start = std::chrono::high_resolution_clock::now();
        kruskal(nodes);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Czas obliczen: " << elapsed_time.count() << "ms" << std::endl;
        std::cout<< std::endl; 
    }
}

int main() {
    
    // Odczyt pliku konfiguracyjnego 
    unordered_map<string, string> config = parse_config_file("config.ini");

    int max_nodes = stoi(config["max_nodes"]);
    string input_file = config["input_file"];

    vector<int> ranges;
    istringstream iss(config["ranges"]);
    for (string range; getline(iss, range, ','); ) {
        ranges.push_back(stoi(range));
    }
    int range_size = stoi(config["range_size"]);

    // Odczyt danych wejsciowych
    ifstream infile(input_file);

    int nodes = stoi(config["nodes"]);
    read_matrix(nodes, infile);
    kruskal(nodes);

    // perform_tests(range_size, ranges, input_file);

    return 0;
}
