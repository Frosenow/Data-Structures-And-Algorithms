#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <unordered_map>
#include <vector>

std::vector<int> read_data(std::string filename){
    std::ifstream file(filename); 
    std::vector<int> data;
    int number; 
    char comma;
    if(file.is_open()){
        while(file >> number >> comma){
            data.push_back(number);
        }
        file.close(); 
    } else { 
        std::cerr << "Error: Nie mozna otworzyc pliku: " << filename << '\n';
    }
    std::cout<< "Odczyt zakonczony..." << "\n";
    return data; 
}

std::unordered_map<std::string, std::string> parse_config(std::string filename){
    std::ifstream config_file(filename);
    std::string line; 
    std::string current_section; 
    std::unordered_map<std::string, std::string> data;

    if (config_file.is_open()) {
        std::string line;
        while (std::getline(config_file, line)) {
            // Usuwanie bialych znakow
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            // Ignoruj puste linie i komentarze 
            if(line.empty() || line[0] == ';'){
                continue; 
            }

            // Sprawdz czy naglowek 
            if(line[0] == '[' && line.back() == ']'){
                current_section = line.substr(1, line.size() - 2);
                continue; 
            }

            // Stworz klucz i odpowiadajaca mu wartosc 
            std::istringstream iss(line);
            std::string key, value; 
            std::getline(iss, key, '=');
            std::getline(iss, value);

            // Usuwanie bialych znakow
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1); 

            data[current_section + '.' + key] = value; 
        }
        config_file.close();
        return data; 
    } else {
        std::cerr << "Error: Could not open file " << filename << "\n"; 
    }
}

std::vector<int> get_instances(std::string config){
    std::vector<int> instances; 
    std::string instance; 
    std::stringstream ss(config);
    while(std::getline(ss, instance, ',')){
        instances.push_back(std::stoi(instance));
    }
    return instances; 
}

int main(){
    // Oczyt pliku sterującego i jego przetworzenie
    std::unordered_map<std::string, std::string> config = parse_config("config.ini"); 
    
    // Uzyskanie tablicy instancji problemu 
    std::vector<int> instances = get_instances(config["SortingAlgorithm1Measurements.instances"]);

    // Odczyt danych z pliku 
    std::vector<int> data = read_data(config["Data.input_file"]);

    // Przetwarzanie danych za pomoca wybranego algorytmu 


    // Zapis wyników do pliku 
}