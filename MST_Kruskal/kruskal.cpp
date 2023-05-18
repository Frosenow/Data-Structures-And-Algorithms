#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "matrix_interface.h"


using namespace std;

// Przetwarzanie pliku konfiguracyjnego 
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

// Maksymalna ilosc wierzcholkow
const int MAX_VERTICES = 1000; 

// Tablica rodzicow wierzcholkow
int parent[MAX_VERTICES]; 

// Tablica maksymalnej glebokosci drzewa z danego wierzcholka
int rank_[MAX_VERTICES]; 

struct Edge {
    // Wierzcholki polaczone krawedzia z okreslona waga 
    int u, v, weight; 
};

// Tablica przechowująca krawędzie grafu
Edge edges[MAX_VERTICES*MAX_VERTICES]; 
// Poczatkowa wartosc krawedzi w grafie
int num_edges = 0; 

// Znajdowanie rodzica wierzcholka 
int find_set(int v) {
    if (v == parent[v]) {
        return v;
    }
    return parent[v] = find_set(parent[v]);
}

// Laczenie dwoch zbiorow, do ktorych naleza wierzcholki
void union_sets(int u, int v) {
    u = find_set(u);
    v = find_set(v);
    // Zbior z mniejsza ranga dolaczony do zbioru z wieksza ranga 
    if (u != v) {
        if (rank_[u] < rank_[v])
            swap(u, v);
        parent[v] = u;
        if (rank_[u] == rank_[v])
            rank_[u]++;
    }
}

// Porownanie wag krawedzi 
bool cmp(Edge a, Edge b) {
    return a.weight < b.weight;
}

void kruskal(int n) {
    for (int i = 0; i < n; i++) {
        parent[i] = i;
        rank_[i] = 0;
    }

    sort(edges, edges+num_edges, cmp);

    // waga MST
    int mst_weight = 0; 
    for (int i = 0; i < num_edges; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        int weight = edges[i].weight;
        if (find_set(u) != find_set(v)) {
            union_sets(u, v);
            mst_weight += weight;

            cout << "Waga krawedzi: " << edges[i].weight << "\tDroga: " << u + 1<< " --> " << v + 1<< endl;
        }
    }

    cout << "Waga MST: " << mst_weight << endl;
}


vector<vector<int>> read_matrix(int n, ifstream& infile) {
    // Odczyt macierzy
    vector<vector<int>> graph(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            infile >> graph[i][j];
            // std::cout<<graph[i][j]<<" "; 
            if (graph[i][j] > 0) {
                edges[num_edges].u = i;
                edges[num_edges].v = j;
                edges[num_edges].weight = graph[i][j];
                num_edges++;
            }
        }
        // Tworzenie podmacierzy jesli macierz wejsciowa jest wieksza niz zadeklarowana macierz (n x n)
        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        // std::cout<<std::endl;
    }
    return graph;
}

void save_to_csv(long elapsed_time, int range, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::app); // Open the file in append mode
    if (!outfile) {
        std::cerr << "Blad podczas otwierania pliku " << filename << std::endl;
        return;
    }
    static bool header_written = false; // keep track if the header has been written already
    if (!header_written) {
        outfile << "Zmierzony czas [ms];Ilosc wierzcholkow\n"; // Write the header on the first line
        header_written = true;
    }
    outfile << elapsed_time << ";" << range << std::endl; // Write the data in CSV format
}


void perform_tests_instances(int range_size, vector<int> ranges, string input_file){
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
        save_to_csv(elapsed_time.count(), nodes, "output_instances.csv");
    }
}

void perform_tests_percentages(string input_file, int max_nodes){
        int nodes = max_nodes;
        ifstream infile(input_file);
        read_matrix(nodes, infile);
        auto start = std::chrono::high_resolution_clock::now();
        kruskal(nodes);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Czas obliczen: " << elapsed_time.count() << "ms" << std::endl;
        std::cout<< std::endl; 
        save_to_csv(elapsed_time.count(), nodes, "output_percentages.csv");
}

int main() {
    // Rozpoczecie programu
    std::cout << "Nacisnij [ENTER] aby rozpoczac..." << std::endl;
    getchar();
    
    // Odczyt pliku konfiguracyjnego 
    unordered_map<string, string> config = parse_config_file("config.ini");

    string input_file = config["input_file"];
    string test_type = config["test"];
    int max_nodes = stoi(config["max_nodes"]);

    vector<int> ranges;
    istringstream iss(config["ranges"]);
    for (string range; getline(iss, range, ','); ) {
        ranges.push_back(stoi(range));
    }

    vector<int> percentages;
    istringstream fss(config["percentages"]);
    for (string percent; getline(fss, percent, ','); ) {
        percentages.push_back(stoi(percent));
    }

    int range_size = stoi(config["range_size"]);
    int range_size_percentages = stoi(config["range_size_percentages"]);

    // Funkcja do testowania z plikiem matrixTest.txt
    // int nodes = stoi(config["nodes"]);
    // read_matrix(nodes, infile);
    // kruskal(nodes);
    char choice;
    cout<<"Wygenerowac macierz? [T/N]"<<endl; 
    cin>> choice; 
    if(choice == 'T') {
        vector<vector<int>> matrix = generateMatrix(max_nodes);
        saveMatrixToFile(matrix, input_file);
    } else if(choice == 'N') {
        cout<<"Macierz nie zostala wygenerowana, szukanie pliku lokalnego..."<<endl; 
    } else {
        cout<<"Bledny wybor..."<<endl;
        std::cout << "Nacisnij [ENTER] aby zakonczyc..." << std::endl;
        getchar();  
        return 0;
    }

    ifstream infile(input_file);

    if(infile.good()){
        cout<<"Plik "<< input_file << " został znaleziony\n";
    } else {
        cout<<"Plik "<< input_file << " nie zostal znaleziony\n";
        std::cout << "Nacisnij [ENTER] aby zakonczyc..." << std::endl;
        getchar(); 
        return 0; 
    }
    if(test_type == "instances"){

        // ********* POMIARY DLA KOLEJNYCH ROZMIAROW INSTANCJI **************
        // Odczyt danych wejsciowych
        perform_tests_instances(range_size, ranges, input_file);

    } else if(test_type == "percentages"){

        // ********* POMIARY DLA KOLEJNO USUWANYCH KRAWEDZI **************
        const string input_file_copy = "matrixCopy.txt";
        // Utworzenie kopii, poniewaz usuwanie krawedzi jest operacja zmieniajaca macierz
        vector<vector<int>> matrix = read_matrix(max_nodes, infile);
        saveMatrixToFile(matrix, input_file_copy);
        for(int i = 0; i < range_size_percentages; i++){
            cout<<"Pomiar dla usunietych "<< percentages[i]<<"% "<<"krawedzi (przy zachowaniu spojnosci grafu)"<<endl;
            ifstream infile(input_file_copy);
            perform_tests_percentages(input_file_copy, max_nodes);
            removeEdges(matrix, percentages[i]);
            saveMatrixToFile(matrix, input_file_copy);
        }

    } else { 
        std::cout<<"Wybrano niepoprawny typ pomiary w pliku konfiguraycjnym [TYP POMIARU: instances || percentages]";
    }
    // Zakonczenie programu
    std::cout << "Nacisnij [ENTER] aby zakonczyc..." << std::endl;
    getchar(); 
    return 0;
}
