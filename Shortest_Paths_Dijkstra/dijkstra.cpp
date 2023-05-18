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

void save_to_csv(long elapsed_time, int range, const std::string& filename) {
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

    // Funkcja do testowania z plikiem matrixDijkstra.txt
    // vector<vector<int>> matrix = generateDirectedMatrix(max_nodes);
    // saveMatrixToFile(matrix, "matrixDijkstra.txt");
    ifstream infile("matrixDijkstraTest.txt");
    // vector<vector<int>> graph = read_matrix(max_nodes, infile);
    int n = 5; 
     vector<vector<int>> graph(n, vector<int>(n, 0));  // initialize graph with 0 weights
    graph[0][1] = 3;
    graph[0][4] = 5;
    graph[1][2] = 2;
    graph[2][1] = 7;
    graph[2][3] = 1;
    graph[3][0] = 4;
    graph[4][3] = 6;

    vector<int> distances(n, INT_MAX);
    vector<int> parents(n);
    
    // Nodes indexed from 0 in graph
    int source = 0;
    dijkstra(graph, distances, parents, source);
    // printShortestPath(parents, 2);

    cout << "Najkrotsza droga od zrodla " << source + 1 << ":\n";
    for (int i = 1; i < n; i++) {
        printShortestPath(parents, i, source + 1, distances[i]);
    }

    // Zakonczenie programu
    std::cout << "Nacisnij [ENTER] aby zakonczyc..." << std::endl;
    getchar(); 
    return 0;
}
