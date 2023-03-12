#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono> 

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

std::vector<int> divide_instances(std::vector<int> numbers, int size_of_instance){
   std::vector<int> subvec(numbers.begin(), numbers.begin() + size_of_instance);
   return subvec; 
}

// Testowy algorytm
void sortowanie_babelkowe(std::vector <int> tab)
{
	for(int i=0; i < tab.size(); i++)
		for(int j=1; j < tab.size() - i; j++) //pętla wewnętrzna
		if(tab[j-1]>tab[j])
			//zamiana miejscami
			std::swap(tab[j-1], tab[j]);
}

// Funkcja sluza do badania algorytmu 
auto solve_problem(std::vector<int> instance, int num_of_measurements, std::string precision){
    long long total_duration = 0; 
    // Wykonanie tego samego algorytmu X razy w celu pomiaru czasu z wykorzystaniem sredniej arytmetycznej 
    for(int i = 0; i < num_of_measurements; i++){
        auto start_timer = std::chrono::high_resolution_clock::now();
        sortowanie_babelkowe(instance);
        auto stop_timer = std::chrono::high_resolution_clock::now();
        if(precision == "ms"){
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_timer - start_timer);
            total_duration += duration.count(); 
        } else if (precision == "us") {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_timer - start_timer);
            total_duration += duration.count(); 
        } else if (precision == "ns") {
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_timer - start_timer);
            total_duration += duration.count(); 
        }
    }
    // Wyliczenie sredniego czasu dla zadanej instancji problemu 
    double avg_duration = static_cast<double>(total_duration) / num_of_measurements;
    std::cout << avg_duration << precision << " ROZMIAR INSTANCJI: " << instance.size() << '\n';
}

int main(){
    // Oczyt pliku sterującego i jego przetworzenie
    std::unordered_map<std::string, std::string> config = parse_config("config.ini"); 

    // Odczyt danych z pliku 
    std::vector<int> data = read_data(config["Data.input_file"]);

    // Uzyskanie tablicy instancji problemu 
    std::vector<int> instances = get_instances(config["SortingAlgorithm1Measurements.instances"]);


    // Przetwarzanie danych za pomoca wybranego algorytmu 
    // Algorytm uruchamiany X dla kazdego rozmiaru instancji 
    for(int i = 0; i < instances.size(); i++){
        // Podzielenie glownego problemu na wybrane instancje
        int size_of_instance = instances[i]; 
        std::vector<int> sub_vec = divide_instances(data, size_of_instance);
        solve_problem(sub_vec, std::stoi(config["Measurement.measurements"]), config["Measurement.precision"]);
    }
    std::cout << "Pomiar zakonczony..." << std::endl;

    // Zapis wyników do pliku

}