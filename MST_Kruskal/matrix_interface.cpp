#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <queue>
#include "matrix_interface.h"


using namespace std;

// Funkcja generująca macierz kwadratową o wymiarach n x n
vector<vector<int>> generateMatrix(int n) {
    // Tworzenie macierzy o wymiarach n x n i wypełnienie zerami
    vector<vector<int>> matrix(n, vector<int>(n, 0));

    // Tworzenie generatora liczb losowych
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 10);

    // Wypełnianie macierzy losowymi wagami krawędzi
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int weight = dist(gen);
            matrix[i][j] = weight;
            matrix[j][i] = weight;
        }
    }

    return matrix;
}

// Funkcja wyświetlająca macierz
void printMatrix(vector<vector<int>> matrix) {
    for (auto row : matrix) {
        for (auto element : row) {
            cout << element << " ";
        }
        cout << endl;
    }
}

// Funkcja zapisująca macierz do pliku
void saveMatrixToFile(vector<vector<int>> matrix, string filename) {
    ofstream outputFile(filename);

    if (outputFile.is_open()) {
        for (auto row : matrix) {
            for (auto element : row) {
                outputFile << element << " ";
            }
            outputFile << endl;
        }

        outputFile.close();
        cout << "Plik " << filename << " zostal zapisany." << endl;
    } else {
        cout << "Blad: Nie mozna otworzyc pliku " << filename << " do zapisu." << endl;
    }
}

// Funkcja sprawdzajaca spojnosc grafu
bool isConnected(vector<vector<int>>& matrix) {
    int n = matrix.size();
    
    // Wykorzystanie algorytmu BFS do sprawdzenia czy wszystkie krawedzie sa osiagalne 
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(0);
    visited[0] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v = 0; v < n; v++) {
            if (matrix[u][v] != 0 && !visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    // Sprawdzenie czy wszystkie wierzcholki byly odwiedzone
    for (bool v : visited) {
        if (!v) {
            return false;
        }
    }
    return true;
}

void removeEdges(vector<vector<int>>& matrix, double percentage) {
    // Oblicz calkowita ilosc krawedzi w grafie 
    int numEdges = 0;
    for (auto row : matrix) {
        for (auto element : row) {
            if (element != 0) {
                numEdges++;
            }
        }
    }

    // Oblicz ilosc krawedzi do usniecia (wyrazona w procentach)
    int numEdgesToRemove = static_cast<int>((percentage / 100.0) * numEdges);

    // Usuwanie krawedzi losowa z zachowaniem spojnosci grafu
    while (numEdgesToRemove > 0) {
        int i = rand() % matrix.size();
        int j = rand() % matrix.size();
        if (matrix[i][j] != 0) {
            matrix[i][j] = 0;
            numEdgesToRemove--;
            if (isConnected(matrix)) {
                cout << "Usunieto krawedz (" << i << ", " << j << ")" << endl;
            } else {
                matrix[i][j] = 1;
            }
        }
    }
}

// int main() {
//     int n;
//     cout << "Podaj rozmiar macierzy: ";
//     cin >> n;

//     double percentToRemove;
//     cout << "Podaj ile procent krawedzi chcesz usunac [WARTOSCI 0 - 100, BEZ ZNAKU]: ";
//     cin >> percentToRemove;

//     vector<vector<int>> matrix = generateMatrix(n);
//     printMatrix(matrix);

//     int numEdgesToRemove = static_cast<int>((percentToRemove / 100.0) * n * (n - 1) / 2);
//     cout << "Usuwanie " << numEdgesToRemove << " Krawedzi aby graf byl spojny..." << endl;

//     removeEdges(matrix, percentToRemove);
//     cout << "Po usunieciu " << percentToRemove << "% krawedzi:" << endl;
//     printMatrix(matrix);

//     string filename;
//     cout << "Podaj nazwe pliku do ktorego ma zostac zapisana macierz wag: ";
//     cin >> filename;

//     saveMatrixToFile(matrix, filename);
//     cout << "Macierz zapisana do pliku " << filename << endl;

//     return 0;
// }

