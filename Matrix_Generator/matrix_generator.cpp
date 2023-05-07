#include <iostream>
#include <vector>
#include <random>
#include <fstream>

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

// Funkcja testująca
int main() {
    int n;
    cout << "Podaj wymiar macierzy: ";
    cin >> n;

    vector<vector<int>> matrix = generateMatrix(n);
    printMatrix(matrix);
    saveMatrixToFile(matrix, "matrix.txt");

    return 0;
}
