#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono> 
#include <iomanip>
#include <algorithm>
#include <math.h>


// Klasa zawierajaca informacje o analize algorytmu
class Result {
    public: 
        std::vector<int> sorted_data;
        double avg_duration; 
        int num_of_instance; 
};

// Odczyt danych 
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
        std::cout<< "Odczyt zakonczony..." << "\n";
        return data; 
    } else { 
        std::cerr << "Error: Nie mozna otworzyc pliku " << filename << " do odczytu\n";
        return {0};
    }
 
}

// Zapis wyników do pliku
void save_data(const std::string filename, Result results){
    std::ofstream file(filename, std::ios::app | std::ios::out);

    if(file.is_open()){
        file << results.avg_duration << ";" << results.num_of_instance << '\n'; 
        file.close();
        std::cout<< "Wynik badania zapisany..." << "\n";
    } else {
        std::cerr << "Error: Nie mozna otworzyc pliku " << filename << " do zapisu\n";
    }
}

// Zapisuje wynik algorytmu 
void save_sorted(const std::string filename, Result results){
    std::ofstream file(filename, std::ios::app | std::ios::out);

    if(file.is_open()){
        file << "**************" << "ROZMIAR INSTANCJI: " << results.num_of_instance << "**************" << '\n';
        for(int i = 0; i < results.sorted_data.size(); i++)
            file << results.sorted_data[i] << ";"; 
        file << '\n';  
        file << "**************************************************************************************" << '\n';
        file.close();
    } else {
        std::cerr << "Error: Nie mozna otworzyc pliku " << filename << "do zapisu\n";
    }
    std::cout<< "Wynik algorytmu zapisany..." << "\n";
}

// Wyswietlanie stanu obliczen
void print_progress(double progress) {
    // Szerokosc paska progresu 
    const int progressbar_width = 50;
    // Okreslenie pozycji kursora  
    int position = progress * progressbar_width;
    std::cout << "\r["; 
    for(int i = 0; i < progressbar_width; i++){
        if( i < position) 
            std::cout << '#'; 
        else 
            std::cout << '.';
    }
    std::cout << "] " << std::setprecision(0) << std::fixed << (progress * 100) << '%' << std::flush; 
    std::cout << std::endl;
}

// Przetwarzanie pliku konfiguracyjnego 
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

// Wyluskanie tablicy instancji z pliku konfiguracyjnego 
std::vector<int> get_instances(std::string config){
    std::vector<int> instances; 
    std::string instance; 
    std::stringstream ss(config);
    while(std::getline(ss, instance, ',')){
        instances.push_back(std::stoi(instance));
    }
    return instances; 
}

// Podzial problemu na mniejsze instancje 
std::vector<int> divide_instances(std::vector<int> numbers, int size_of_instance){
   std::vector<int> subvec(numbers.begin(), numbers.begin() + size_of_instance);
   return subvec; 
}

// *******************************************
// SORTOWANIA - POCZATEK
// *******************************************

// Sortowanie babelkowe
std::vector <int> bubble_sort(std::vector <int> tab)
{
	for(int i=0; i < tab.size(); i++)
		for(int j=1; j < tab.size() - i; j++) 
		if(tab[j-1]>tab[j])
			// Zamiana miejscami - wypchniecie najwiekszego 'babelka' na koniec
			std::swap(tab[j-1], tab[j]);

    return tab; 
}

// Sortowanie przez kopcowanie 
std::vector <int> heap_sort(std::vector<int>& arr);
void build_heap(std::vector<int>& arr);
void convert_to_heap(std::vector<int>& arr, int i, int heap_size);

std::vector <int> heap_sort(std::vector<int>& arr){
    // Tworzenie kopca z wejsciowego wektora
    build_heap(arr);
 
    // Powtarzanie do momentu az kopiec zawiera tylko jeden element
    for (int i = arr.size() - 1; i > 0; i--){

        // Zamiana elementu korzenia z ostatnim elementem 
        std::swap(arr[0], arr[i]);
 
        // Usuniecie po zamianie ostatniego elementu ktory jest we wlasciwym miejscu
        int heap_size = i;
        convert_to_heap(arr, 0, heap_size);
    }

    // Zwrot posortowanego wektora
    return arr; 
}
 
// Budowanie kopca
void build_heap(std::vector<int>& arr){
    // Budowanie maksymalnego kopca
    int n = arr.size();
    // max ilosc wezlow 2^(h + 1) - 1, h - wysokosc 
    for (int i = n / 2 - 1; i >= 0; i--){
        convert_to_heap(arr, i, n);
    }
}
 
// Przeksztalcanie wektora w kopiec 
void convert_to_heap(std::vector<int>& arr, int i, int heap_size){
    // Przeksztalcenie wektora w drzewo binarne
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
 
    if (left < heap_size && arr[left] > arr[largest])
        largest = left;
    if (right < heap_size && arr[right] > arr[largest])
        largest = right;
 
    if (largest != i){
        std::swap(arr[i], arr[largest]);
        convert_to_heap(arr, largest, heap_size);
    }
}

// Sortowanie przez zliczanie 
std::vector <int> count_sort(std::vector<int>& arr)
{
    // Deklaracja minimalnej i maksymalnej wartosci 
    int max = *max_element(arr.begin(), arr.end());
    int min = *min_element(arr.begin(), arr.end());
    // Okreslenie rozmiaru tablicy (rozpietosc zbioru liczb)
    int range = max - min + 1;
 
    // Deklaracja wektora count (ilosc element) oraz output (wyjsciowa posortowana tablica)
    std::vector<int> count(range), output(arr.size());
    
    // Zliczenie wystapien wszystkich liczb 
    // najmniejsza wartosc na 0 indeks 
    for (int i = 0; i < arr.size(); i++)
        count[arr[i] - min]++;
 
    // Ustalenie indeksu 
    for (int i = 1; i < count.size(); i++)
        count[i] += count[i - 1];
 
    // Iteracja od konca i wstawianie liczb
    for (int i = arr.size() - 1; i >= 0; i--) {
        output[count[arr[i] - min] - 1] = arr[i];
        // Zmniejszenie ilosci liczb
        count[arr[i] - min]--;
    }

    return output;
}

// *******************************************
// SORTOWANIA - KONIEC
// *******************************************

// Funkcja sluzaca do badania algorytmu 
Result solve_problem(std::vector<int> instance, int num_of_measurements, std::string precision, int time_limit, std::string algorithm_name){
    auto total_duration = 0; 
    
    std::vector<int> sorted; 
    std::unordered_map<std::string, std::string> results;

    const int mltp_measurements = 1000;
    const int min_instance_size = 500; 

    // Wiecej pomiarow dla mniejszych instancji 
    if(instance.size() <= min_instance_size){
        num_of_measurements *= mltp_measurements;
    }

    // Domyslnie time_limit jest w sekundach 
    if(precision == "ms"){
        time_limit *= 1000;
    } else if (precision == "us") {
        time_limit *= 1000000;
    } else if (precision == "ns") {
        time_limit *= 1000000000;
    }

    // Wykonanie tego samego algorytmu X-razy w celu pomiaru czasu z wykorzystaniem sredniej arytmetycznej 
    for(int i = 0; i < num_of_measurements; i++){
        auto start_timer = std::chrono::high_resolution_clock::now();
        if(algorithm_name == "Bubble_Sort"){
            sorted = bubble_sort(instance);
        } else if (algorithm_name == "Heap_Sort") {
            sorted = heap_sort(instance);
        } else if (algorithm_name == "Count_Sort") {
            sorted = count_sort(instance);           
        } else {
            std::cout << "Nieznana nazwa algorytmu" << std::endl; 
            return Result();
        }
        auto stop_timer = std::chrono::high_resolution_clock::now();
    if(precision == "ms"){
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_timer - start_timer);
        if(total_duration + duration.count() > time_limit){
            std::cout << "PRZEKROCZONO MAKSYMALNY, CALKOWITY CZAS POMIARU INSTANCJI: " << total_duration << " " << precision << std::endl; 
            break;
        }
        total_duration += duration.count(); 
    } else if (precision == "us") {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_timer - start_timer);
        if(total_duration + duration.count() > time_limit){
            std::cout << "PRZEKROCZONO MAKSYMALNY, CALKOWITY CZAS POMIARU INSTANCJI: " << total_duration << " " << precision << std::endl; 
            break;
        }
        total_duration += duration.count(); 
    } else if (precision == "ns") {
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_timer - start_timer);
        if(total_duration + duration.count() > time_limit){
            std::cout << "PRZEKROCZONO MAKSYMALNY, CALKOWITY CZAS POMIARU INSTANCJI: " << total_duration << " " << precision << std::endl; 
            break;
        }
        total_duration += duration.count(); 
    }
    }
    // Wyliczenie sredniego czasu dla zadanej instancji problemu 
    double avg_duration = static_cast<double>(total_duration) / num_of_measurements;
    std::cout << '\n' << "**************************************************************" << '\n';
    std::cout << "CZAS: " <<  avg_duration << precision << '\n' << "ROZMIAR INSTANCJI: " << instance.size() << '\n';
    std::cout << "CZAS CALKOWITY: " << total_duration << precision <<'\n';
    std::cout << "LIMIT POMIARU INSTANCJI: " << time_limit << precision << std::endl;

    Result algorithm_result; 
    algorithm_result.sorted_data = sorted; 
    algorithm_result.avg_duration = avg_duration; 
    algorithm_result.num_of_instance = instance.size(); 
    return algorithm_result; 
}

 
int main(){
    // Rozpoczecie programu
    std::cout << "Nacisnij [ENTER] aby rozpoczac..." << std::endl;
    getchar();
    bool saved = true; 
    // Oczyt pliku sterującego i jego przetworzenie
    std::unordered_map<std::string, std::string> config = parse_config("config.ini");

    // Odczyt danych z pliku 
    std::vector<int> data = read_data(config["Data.input_file"]);

    // Uzyskanie tablicy z instancjami problemu 
    std::vector<int> instances = get_instances(config["SortingAlgorithm1Measurements.instances"]);

    // Przetwarzanie danych za pomoca wybranego algorytmu 
    // Algorytm uruchamiany X-razy dla kazdego rozmiaru instancji
    for(int i = 0; i < std::stoi(config["SortingAlgorithm1Measurements.num_of_instances"]); i++){
        // Podzielenie glownego problemu na wybrane instancje
        int size_of_instance = instances[i]; 
        std::vector<int> sub_vec = divide_instances(data, size_of_instance);
        std::cout << "Rozpoczynam pomiar..." << std::endl; 
        auto resultObj = solve_problem(sub_vec, std::stoi(config["Measurement.measurements"]), config["Measurement.precision"], std::stoi(config["Measurement.time_limit"]), config["SortingAlgorithm.name"]);

        // Zatrzymanie badania gdy wystapil blad dotyczacy blednej nazwy algorytmu
        if (resultObj.sorted_data.empty()) {
            std::cout << "Blad podczas badania! Upewnij sie, ze wprowadziles poprawna nazwe badanego algorytmu w pliku konfiguracyjnym" << std::endl;
            break; 
        } 

        if(std::stoi(config["Verification.show_sorted"])){
        std::cout << "Dane po sortowaniu: " << "\n";
        for(int i = 0; i < resultObj.sorted_data.size(); i++){
            std::cout<< resultObj.sorted_data[i] << " ";
        }std::cout<<std::endl; 
    }
        print_progress(static_cast<double>(i) / std::stoi(config["SortingAlgorithm1Measurements.num_of_instances"]));
        std::cout << '\n' << "**************************************************************" << '\n';
        save_data(config["Data.output_file"], resultObj);
        if(std::stoi(config["Verification.save_sorted"]))
            save_sorted("output.txt", resultObj);
    }
    print_progress(1);
    std::cout << "Pomiar zakonczony..." << std::endl;
    // Zakonczenie programu
    std::cout << "Nacisnij [ENTER] aby zakonczyc..." << std::endl;
    getchar(); 
    return 0; 
}