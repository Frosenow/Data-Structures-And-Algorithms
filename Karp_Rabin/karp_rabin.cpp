#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

// Prime number used for hashing
const int PRIME = 101; 

vector<int> findMatches(const string& pattern, const string& text) {
    vector<int> matches;
    int patternLen = pattern.length();
    int textLen = text.length();

    int patternHash = 0; // Hash value for the pattern
    int windowHash = 0;  // Hash value for the current window

    // Calculate the initial hash values
    for (int i = 0; i < patternLen; i++) {
        patternHash += pattern[i] * pow(PRIME, i);
        windowHash += text[i] * pow(PRIME, i);
    }

    // Compare the hash values and check for matches
    for (int i = 0; i <= textLen - patternLen; i++) {
        if (patternHash == windowHash) {
            bool match = true;
            // Compare characters to confirm the match
            for (int j = 0; j < patternLen; j++) {
                if (pattern[j] != text[i + j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                matches.push_back(i);
            }
        }
        // Calculate the hash value for the next window
        windowHash = (windowHash - text[i]) / PRIME + text[i + patternLen] * pow(PRIME, patternLen - 1);
    }

    return matches;
}

void saveTextWithMatches(const string& text, const vector<int>& matches, const string& outputFilename, const string& matchMarker, const string& pattern) {
    ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        cout << "Error opening file " << outputFilename << endl;
        return;
    }

    int lastMatchEnd = 0;
    for (int matchPos : matches) {
        // Save the text before the match
        outputFile << text.substr(lastMatchEnd, matchPos - lastMatchEnd);

        // Save the matched pattern with a marker
        outputFile << matchMarker << text.substr(matchPos, pattern.length());

        lastMatchEnd = matchPos + pattern.length();
    }

    // Save the remaining text after the last match
    outputFile << text.substr(lastMatchEnd);

    outputFile.close();
}

int main() {
    string pattern; 
    cout << "Wprowadz wzorzec: ";
    getline(cin, pattern);

    string filename;
    cout << "Wprowadz nazwe pliku: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Blad podczas otwierania pliku " << filename << endl;
        return 0;
    }

    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    vector<int> matches = findMatches(pattern, text);

    if (matches.empty()) {
        cout << "Brak dopasowania do wzorca." << endl;
    } else {
        string outputFilename = filename.substr(0, filename.find_last_of(".")) + "_marked.txt";
        string matchMarker = "**" + pattern + "**";
        saveTextWithMatches(text, matches, outputFilename, matchMarker, pattern);;
        cout << "Dopasowanie/a znalezione na pozycji/ach: ";
        for (int pos : matches) {
            cout << pos << " ";
        }
        cout << endl;
        cout << "Plik zapisany z zaznaczonymi dopasowaniami: " << outputFilename << endl;
    }

    return 0;
}
