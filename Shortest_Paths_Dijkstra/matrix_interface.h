#ifndef MYHEADER_H
#define MYHEADER_H

#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <queue>

// Prototypy funkcji 
std::vector<std::vector<int>> generateMatrix(int n);
std::vector<std::vector<int>> generateDirectedMatrix(int n);
void printMatrix(std::vector<std::vector<int>> matrix);
void saveMatrixToFile(std::vector<std::vector<int>> matrix, std::string filename);
bool isConnected(std::vector<std::vector<int>>& matrix);
void removeEdges(std::vector<std::vector<int>>& matrix, double percentage);

#endif // MYHEADER_H

