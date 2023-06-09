#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <climits>
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
            // Pominiecie komentarzy
            continue;
        }
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos == string::npos) {
            // Pominiecie znakow przypisania
            continue;
        }
        string key = line.substr(0, delimiter_pos);
        string value = line.substr(delimiter_pos + 1);
        // Usuniecie bialych znakow
        key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());
        value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());
        config[key] = value;
    }

    infile.close();
    return config;
}


vector<vector<int>> read_matrix(int n, ifstream& infile) {
    // Odczyt macierzy
    vector<vector<int>> graph(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            infile >> graph[i][j];
        }
        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return graph;
}

void save_to_csv(double elapsed_time, int range, const std::string& filename) {
    // Otwarcie pliku w trybie append
    std::ofstream outfile(filename, std::ios::app); 
    if (!outfile) {
        std::cerr << "Blad podczas otwierania pliku " << filename << std::endl;
        return;
    } // Sparwdzenie czy naglowek jest juz w pliku
    static bool header_written = false; 
    if (!header_written) {
        // Zapisanie naglowka w pierszwej linijce
        outfile << "Zmierzony czas [ms];Ilosc wierzcholkow\n"; 
        header_written = true;
    }
    // Zapis pliku
    outfile << elapsed_time << ";" << range << std::endl; 
}

void dijkstra(const vector<vector<int>>& graph, vector<int>& distances, vector<int>& parents, int source) {
    int n = graph.size();
    vector<bool> visited(n, false);

    for (int i = 0; i < n; i++) {
        distances[i] = INT_MAX;
    }
    
    distances[source] = 0;
    parents[source] = -1;

    for (int i = 0; i < n - 1; i++) {
        int u = -1;
        for (int j = 0; j < n; j++) {
            if (!visited[j] && (u == -1 || distances[j] < distances[u])) {
                u = j;
            }
        }

        if (distances[u] == INT_MAX) {
            break;
        }
        visited[u] = true;

        for (int v = 0; v < n; v++) {
            if (graph[u][v] > 0 && !visited[v]) {
                if (distances[u] + graph[u][v] < distances[v]) {
                    distances[v] = distances[u] + graph[u][v];
                    parents[v] = u;
                }
            }
        }
    }
}

void printShortestPath(const vector<int>& parents, int destination, int source, int weight) {
    vector<int> path;
    cout << "Najkrotsza sciezka do "<<destination + 1<<": ";
    while (destination != -1) {
        path.push_back(destination);
        destination = parents[destination];
    }
    reverse(path.begin(), path.end());
    for (int i = 0; i < path.size(); i++) {
        cout << path[i] + 1;
        if (i < path.size() - 1) {
            cout << " -> ";
        }
    }
    cout<<" | Waga: "<<weight;
    cout << endl;
}

void perform_tests_instances(int range_size, vector<int> ranges, string input_file, unordered_map<string, string> config, int num_tests) {
    for (int i = 0; i < range_size; i++) {
        int nodes = ranges[i];
        ifstream infile(input_file);
        vector<vector<int>> graph = read_matrix(nodes, infile);
        vector<int> distances(nodes, INT_MAX);
        vector<int> parents(nodes);

        // Nodes indexed from 0 in graph
        int source = stoi(config["source_node"]);

        std::cout << "Rozmiar macierzy: " << ranges[i] << "x" << ranges[i] << std::endl;

        double total_elapsed_time = 0.0;

        for (int test = 0; test < num_tests; test++) {
            auto start = std::chrono::high_resolution_clock::now();
            dijkstra(graph, distances, parents, source);
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            total_elapsed_time += elapsed_time.count();
        }

        double average_elapsed_time = total_elapsed_time / num_tests;

        cout << "Sredni czas pomiaru " << num_tests << " tests:\n";
        cout << "Najkrotsza droga od zrodla " << source + 1 << ":\n";
        for (int i = 1; i < nodes; i++) {
            printShortestPath(parents, i, source + 1, distances[i]);
        }
        std::cout << "Czas obliczen (sredni): " << average_elapsed_time << "ms" << std::endl;
        std::cout << std::endl;
        save_to_csv(average_elapsed_time, nodes, "output_instances.csv");
    }
}


void perform_tests_percentages(string input_file, int max_nodes, unordered_map<string, string> config, int num_tests) {
    int nodes = max_nodes;
    ifstream infile(input_file);
    vector<vector<int>> graph = read_matrix(nodes, infile);
    vector<int> distances(nodes, INT_MAX);
    vector<int> parents(nodes);

    // Nodes indexed from 0 in graph
    int source = stoi(config["source_node"]);

    double total_elapsed_time = 0.0;

    for (int test = 0; test < num_tests; test++) {
        auto start = std::chrono::high_resolution_clock::now();
        dijkstra(graph, distances, parents, source);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        total_elapsed_time += elapsed_time.count();
    }

    double average_elapsed_time = total_elapsed_time / num_tests;

    cout << "Sredni czas po " << num_tests << " testach:\n";
    cout << "Najkrotsza droga od zrodla " << source + 1 << ":\n";
    for (int i = 1; i < nodes; i++) {
        printShortestPath(parents, i, source + 1, distances[i]);
    }
    std::cout << "Czas obliczen (srednia): " << average_elapsed_time << "ms" << std::endl;
    std::cout << std::endl;
    save_to_csv(average_elapsed_time, nodes, "output_percentages.csv");
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
    int nodes = stoi(config["nodes"]);
    int num_of_measurements = stoi(config["num_of_measurements"]);

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
        perform_tests_instances(range_size, ranges, input_file, config, num_of_measurements);

    } else if(test_type == "percentages"){

        // ********* POMIARY DLA KOLEJNO USUWANYCH KRAWEDZI **************
        const string input_file_copy = "matrixCopy.txt";
        // Utworzenie kopii, poniewaz usuwanie krawedzi jest operacja zmieniajaca macierz
        vector<vector<int>> matrix = read_matrix(max_nodes, infile);
        saveMatrixToFile(matrix, input_file_copy);
        for(int i = 0; i < range_size_percentages; i++){
            cout<<"Pomiar dla usunietych "<< percentages[i]<<"% "<<"krawedzi (przy zachowaniu spojnosci grafu)"<<endl;
            ifstream infile(input_file_copy);
            perform_tests_percentages(input_file_copy, max_nodes, config, num_of_measurements);
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
