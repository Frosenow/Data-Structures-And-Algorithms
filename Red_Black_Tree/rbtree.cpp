#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <vector>
#include <sstream> 

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

// Struktura Wezla w RB Tree
struct RBTNode
{
  RBTNode * up;
  RBTNode * left;
  RBTNode * right;
  int key;
  char color;
};

// Stworzenie obiektu RB Tree
class TRBTree
{
  private:
    RBTNode S;             // Węzeł strażnika, sluzacy do zonaczania wezlow NIL 
    RBTNode * root;        // Korzeń drzewa czerwono-czarnego
    string cr, cl, cp;     // Znaki do tworzenia reprezentacji graficznej

    void printRBT ( string sp, string sn, RBTNode * p ); // Wyrysowanie drzewa

  public:
    TRBTree( );             // Konstruktor klasy
    ~TRBTree( );            // Destruktor klasy
    void DFSRelease ( RBTNode * p ); // Usuwa rekurencyjnie drzewo
    void print( );          // Wypisuje drzewo
    RBTNode * findRBT ( int k ); // Wyszukuje węzeł o kluczu k
    RBTNode * minRBT ( RBTNode * p ); // Wyszukuje najmniejszy węzeł w p
    RBTNode * succRBT ( RBTNode * p );// Wyszukuje następnik p
    void rot_L ( RBTNode * A ); // Rotacja w lewo względem A
    void rot_R ( RBTNode * A ); // Rotacja w prawo względem A
    void insertRBT ( int k );   // Wstawia węzeł o kluczu k
    void removeRBT ( RBTNode * X ); // Usuwa węzeł X
};

// Budowanie drzewa
TRBTree::TRBTree( )
{
  cr = cl = cp = "  ";
  cr [ 0 ] = 218; cr [ 1 ] = 196;
  cl [ 0 ] = 192; cl [ 1 ] = 196;
  cp [ 0 ] = 179;
  S.color = 'B';             // Inicjujemy strażnika
  S.up    = &S;
  S.left  = &S;
  S.right = &S;
  root    = &S;              // Korzeń wskazuje strażnika
}

// Destruktor drzewa
TRBTree::~TRBTree( )
{
  DFSRelease ( root );       // Rekurencyjne usuwanie węzłow
}

// Usuwa rekurencyjnie drzewo czerwono-czarne
//-------------------------------------------
void TRBTree::DFSRelease ( RBTNode * p )
{
  if( p != &S )
  {
    DFSRelease ( p->left );  // usuwamy lewe poddrzewo
    DFSRelease ( p->right ); // usuwamy prawe poddrzewo
    delete p;                // usuwamy sam węzeł
  }
}

// Wypisywanie zawartości drzewa
void TRBTree::printRBT ( string sp, string sn, RBTNode * p )
{
  string t;

  if( p != &S )
  {
    t = sp;
    if( sn == cr ) t [ t.length( ) - 2 ] = ' ';
    printRBT ( t+cp, cr, p->right );

    t = t.substr ( 0, sp.length( ) - 2 );
    cout << t << sn << p->color << ":" << p->key << endl;

    t = sp;
    if( sn == cl ) t [ t.length( ) - 2 ] = ' ';
    printRBT ( t+cp, cl, p->left );
  }
}

// Funkcja pomocniczna do wypisania zawartosci drzewa
void TRBTree::print( )
{
  printRBT ( "", "", root );
}

// Wyszukuje węzeł o kluczu k
// Jeśli węzeł nie zostanie znaleziony, to zwraca wskazanie puste NULL
RBTNode * TRBTree::findRBT ( int k )
{
  RBTNode * p;

  p = root;
  while( ( p != &S ) && ( p->key != k ) )
    if( k < p->key ) p = p->left;
    else           p = p->right;
  if( p == &S ) return NULL;
  return p;
}

// Wyszukuje najmniejszy węzeł w poddrzewie o korzeniu p
RBTNode * TRBTree::minRBT ( RBTNode * p )
{
  if( p != &S )
    while( p->left != &S ) p = p->left;
  return p;
}

// Zwraca następnik p
RBTNode * TRBTree::succRBT ( RBTNode * p )
{
  RBTNode * r;

  if( p != &S )
  {
    if( p->right != &S ) return minRBT ( p->right );
    else
    {
      r = p->up;
      while( ( r != &S ) && ( p == r->right ) )
      {
        p = r;
        r = r->up;
      }
      return r;
    }
  }
  return &S;
}

// Wykonuje rotację w lewo względem A
void TRBTree::rot_L ( RBTNode * A )
{
  RBTNode * B, * p;

  B = A->right;
  if( B != &S )
  {
    p = A->up;
    A->right = B->left;
    if( A->right != &S ) A->right->up = A;

    B->left = A;
    B->up = p;
    A->up = B;

    if( p != &S )
    {
      if( p->left == A ) p->left = B; else p->right = B;
    }
    else root = B;
  }
}

// Wykonuje rotację w prawo względem A
void TRBTree::rot_R ( RBTNode * A )
{
  RBTNode * B, * p;

  B = A->left;
  if( B != &S )
  {
    p = A->up;
    A->left = B->right;
    if( A->left != &S ) A->left->up = A;

    B->right = A;
    B->up = p;
    A->up = B;

    if( p != &S )
    {
      if( p->left == A ) p->left = B; else p->right = B;
    }
    else root = B;
  }
}

// Wstawia do drzewa węzeł o kluczu k
void TRBTree::insertRBT ( int k )
{
  RBTNode * X, * Y;

  X = new RBTNode;        // Tworzymy nowy węzeł
  X->left  = &S;          // Inicjujemy pola
  X->right = &S;
  X->up    = root;
  X->key   = k;
  if( X->up == &S ) root = X; // X staje się korzeniem
  else
  while( true )           // Szukamy liścia do zastąpienia przez X
  {
    if( k < X->up->key )
    {
      if( X->up->left == &S )
      {
        X->up->left = X;  // X zastępuje lewy liść
        break;
      }
      X->up = X->up->left;
    }
    else
    {
      if( X->up->right == &S )
      {
        X->up->right = X; // X zastępuje prawy liść
        break;
      }
      X->up = X->up->right;
    }
  }
  X->color = 'R';         // Węzeł kolorujemy na czerwono
  while( ( X != root ) && ( X->up->color == 'R' ) )
  {
    if( X->up == X->up->up->left )
    {
      Y = X->up->up->right; // Y -> wujek X

      if( Y->color == 'R' ) // Przypadek 1
      {
        X->up->color = 'B';
        Y->color = 'B';
        X->up->up->color = 'R';
        X = X->up->up;
        continue;
      }

      if( X == X->up->right ) // Przypadek 2
      {
        X = X->up;
        rot_L ( X );
      }

      X->up->color = 'B'; // Przypadek 3
      X->up->up->color = 'R';
      rot_R ( X->up->up );
      break;
    }
    else
    {                  // Przypadki lustrzane
      Y = X->up->up->left;

      if( Y->color == 'R' ) // Przypadek 1
      {
        X->up->color = 'B';
        Y->color = 'B';
        X->up->up->color = 'R';
        X = X->up->up;
        continue;
      }

      if( X == X->up->left ) // Przypadek 2
      {
        X = X->up;
        rot_R ( X );
      }

      X->up->color = 'B'; // Przypadek 3
      X->up->up->color = 'R';
      rot_L ( X->up->up );
      break;
    }
  }
  root->color = 'B';
}

// Usuwa z drzewa węzeł X
void TRBTree::removeRBT ( RBTNode * X )
{
  RBTNode * W, * Y, * Z;

  if( ( X->left == &S ) || ( X->right == &S ) ) Y = X;
  else Y = succRBT ( X );

  if( Y->left != &S ) Z = Y->left;
  else Z = Y->right;

  Z->up = Y->up;

  if( Y->up == &S ) root = Z;
  else if( Y == Y->up->left ) Y->up->left  = Z;
  else Y->up->right = Z;

  if( Y != X ) X->key = Y->key;

  if( Y->color == 'B' )   // Naprawa struktury drzewa czerwono-czarnego
    while( ( Z != root ) && ( Z->color == 'B' ) )
      if( Z == Z->up->left )
      {
        W = Z->up->right;

        if( W->color == 'R' )
        {              // Przypadek 1
          W->color = 'B';
          Z->up->color = 'R';
          rot_L ( Z->up );
          W = Z->up->right;
        }

        if( ( W->left->color == 'B' ) && ( W->right->color == 'B' ) )
        {              // Przypadek 2
          W->color = 'R';
          Z = Z->up;
          continue;
        }

        if( W->right->color == 'B' )
        {              // Przypadek 3
          W->left->color = 'B';
          W->color = 'R';
          rot_R ( W );
          W = Z->up->right;
        }

        W->color = Z->up->color; // Przypadek 4
        Z->up->color = 'B';
        W->right->color = 'B';
        rot_L ( Z->up );
        Z = root;      // To spowoduje zakończenie pętli
      }
      else
      {                // Przypadki lustrzane
        W = Z->up->left;

        if( W->color == 'R' )
        {              // Przypadek 1
          W->color = 'B';
          Z->up->color = 'R';
          rot_R ( Z->up );
          W = Z->up->left;
        }

        if( ( W->left->color == 'B' ) && ( W->right->color == 'B' ) )
        {              // Przypadek 2
          W->color = 'R';
          Z = Z->up;
          continue;
        }

        if( W->left->color == 'B' )
        {              // Przypadek 3
          W->right->color = 'B';
          W->color = 'R';
          rot_L ( W );
          W = Z->up->left;
        }

        W->color = Z->up->color;  // Przypadek 4
        Z->up->color = 'B';
        W->left->color = 'B';
        rot_R ( Z->up );
        Z = root;      // To spowoduje zakończenie pętli
      }

  Z->color = 'B';

  delete Y;
}

// Odczyt danych z pliku .txt
void buildRBTFromFile(const vector<int>& data, TRBTree* RBT){
    for(int num : data){
        RBT->insertRBT(num);
    }
}

void buildRBT(string input_file, TRBTree* RBT){
    ifstream inputFile(input_file);
    int value;
    while (inputFile >> value) {
        RBT->insertRBT(value);
    }
    inputFile.close();
}

// Wyluskanie tablicy instancji z pliku konfiguracyjnego 
vector<int> get_instances(string config){
    vector<int> instances; 
    string instance; 
    stringstream ss(config);
    while(getline(ss, instance, ',')){
        instances.push_back(stoi(instance));
    }
    return instances; 
}

// Zapisanie otrzymanych danych
void save_to_file(string outputfile_name, vector<int>& instances, int num_of_instances, vector<double>& measuredTimes) {
    ofstream outputFile(outputfile_name);
    outputFile << "Rozmiar instancji" << ";" << "Czas budowy drzewa [ms]" << endl;
    for (int i = 0; i < num_of_instances; i++) {
        outputFile << instances[i] << ";" << measuredTimes[i] << endl;
    }
    outputFile.close();
}

void perform_measurements(int num_of_instances, vector<int>  instances, vector<int> data, vector<double>& measuredTimes, int num_of_measurements){
    for(int i = 0; i < num_of_instances; i++) {
      vector<int> instance(data.begin(), data.begin() + instances[i]);
      TRBTree* RBT = new TRBTree;
      auto total_duration = 0; 
      // Pomiar budowy RDT
      for(int i = 0; i < num_of_measurements; i++){
        double total_elapsed_time = 0.0;
        auto start = std::chrono::high_resolution_clock::now();
        buildRBTFromFile(instance, RBT);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        total_duration += elapsed_time;
      }
      delete RBT; 
      measuredTimes.push_back(total_duration/num_of_measurements);
      // Koniec pomiaru
      std::cout << "Czas budowy RBT: " << double(total_duration/num_of_measurements) << "ms " << "dla instancji: "<< instances[i] <<std::endl;
    }
}

int main()
{
    // Odczyt pliku konfiguracyjnego 
    unordered_map<string, string> config = parse_config_file("config.ini");
    string input_file = config["input_file"];
    string output_file = config["output_file"];

    // Uzyskanie informacji o ilosci elementow z tablicy instancji problemu
    int num_of_instances = stoi(config["num_of_instances"]);

    // Uzyskanie tablicy z instancjami problemu 
    vector<int> instances = get_instances(config["instances"]);

    // Ilosc powtorzen pomiarow dla wiekszje dokladnosci 
    int num_of_measurements = stoi(config["num_of_instances"]);

    // Zmienna do przechowywania mierzonych czasow
    vector<double> measuredTimes;

    // Odczyt danych z pliku .txt
    ifstream inputFile(input_file);
    vector<int> data;
    int num;
    while (inputFile >> num) {
        data.push_back(num);
    }
    inputFile.close();

    // Inicjalizacja drzewa RB 
    TRBTree* RBT = new TRBTree;

    int choice;
    while (true) {
        cout << "Menu:" << endl;
        cout << "0. Buduj drzewo" << endl; 
        cout << "1. Dodaj element" << endl;
        cout << "2. Usun element" << endl;
        cout << "3. Wyszukaj element" << endl;
        cout << "4. Wyrysuj drzewo" << endl;
        cout << "6. Wykonaj pomiary budowy drzewa" << endl; 
        cout << "5. Wyjdz" << endl;
        cout << "Wprowadz wybor: ";
        cin >> choice;
        cout<<endl;

        switch (choice) {
            case 0: {
                // Pomiar budowy RDT
               double total_elapsed_time = 0.0;
               auto start = std::chrono::high_resolution_clock::now();
               buildRBT(input_file, RBT);
               auto end = std::chrono::high_resolution_clock::now();
               auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
               std::cout << "Czas budowy RBT: " << elapsed_time << "ms" << std::endl;
               break;
            }
            case 1: {
                int element;
                cout << "Wprowadz element do dodania: ";
                cin >> element;
                RBT->insertRBT(element);
                break;
            }
            case 2: {
                int element;
                cout << "Wprowadz element do usuniecia: ";
                cin >> element;
                RBTNode* node = RBT->findRBT(element);
                if (node != NULL) {
                    RBT->removeRBT(node);
                    cout << "Element usuniety." << endl;
                } else {
                    cout << "Element nie znaleziony." << endl;
                }
                break;
            }
            case 3: {
                int element;
                cout << "Wprowadz szukany element: ";
                cin >> element;
                RBTNode* node = RBT->findRBT(element);
                if (node != NULL) {
                    cout << "Element znaleziony." << endl;
                } else {
                    cout << "Element nie znaleziony." << endl;
                }
                break;
            }
            case 4:
                cout << "Struktura RBT:" << endl;
                RBT->print();
                break;
            case 5:
                delete RBT;
                return 0;
            case 6: {
              perform_measurements(num_of_instances, instances, data, measuredTimes, num_of_measurements);
              save_to_file(output_file, instances, num_of_instances, measuredTimes);
              break;
            }
            default:
                cout << "Zly wybor. Sprobuj ponownie." << endl;
                break;
        }
        cout << endl;
    }

    return 0;
}