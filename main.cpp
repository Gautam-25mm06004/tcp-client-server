#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;
namespace fs = filesystem;

vector<string> tokenize(const string& text) {
    vector<string> words;
    string word;
    for (unsigned char ch : text) {
        if (ch >= 'A' && ch <= 'Z') ch = ch - 'A' + 'a';
        if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
            word += ch;
        } else if (!word.empty()) {
            words.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) words.push_back(word);
    return words;
}

vector<string> documents;
vector<int> lengths;
unordered_map<string, unordered_map<int, int>> invertedIndex;

void buildIndex(const string& folder) {
    vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") files.push_back(entry.path());
    }
    sort(files.begin(), files.end());
    for (const auto& path : files) {
        ifstream input(path);
        if (!input) throw runtime_error("Cannot read " + path.string());
        ostringstream content;
        content << input.rdbuf();
        if (input.bad()) throw runtime_error("Read failed: " + path.string());
        auto words = tokenize(content.str());
        int id = static_cast<int>(documents.size());
        documents.push_back(path.filename().string());
        lengths.push_back(static_cast<int>(words.size()));
        for (const string& word : words) invertedIndex[word][id]++;
    }
}

set<int> matching(const string& word) {
    set<int> ids;
    auto found = invertedIndex.find(word);
    if (found != invertedIndex.end()) {
        for (const auto& posting : found->second) ids.insert(posting.first);
    }
    return ids;
}

set<int> readTerm(istringstream& input, set<string>& positiveWords) {
    string raw;
    if (!(input >> raw)) throw runtime_error("Expected a search word.");
    bool negate = raw == "NOT";
    if (negate && !(input >> raw)) throw runtime_error("Expected a word after NOT.");
    if (raw == "AND" || raw == "OR" || raw == "NOT") throw runtime_error("Expected a word, not an operator.");
    auto words = tokenize(raw);
    if (words.size() != 1 || words[0].size() != raw.size()) throw runtime_error("Use one letters-or-digits word per term.");
    set<int> ids = matching(words[0]);
    if (!negate) {
        positiveWords.insert(words[0]);
        return ids;
    }
    set<int> opposite;
    for (int id = 0; id < static_cast<int>(documents.size()); id++) {
        if (!ids.count(id)) opposite.insert(id);
    }
    return opposite;
}

void search(const string& query) {
    istringstream input(query);
    set<string> positiveWords;
    set<int> group = readTerm(input, positiveWords);
    set<int> matches;
    string operation;
    while (input >> operation) {
        if (operation != "AND" && operation != "OR") throw runtime_error("Put AND or OR between words.");
        set<int> next = readTerm(input, positiveWords);
        if (operation == "AND") {
            set<int> common;
            for (int id : group) if (next.count(id)) common.insert(id);
            group = common;
        } else {
            matches.insert(group.begin(), group.end());
            group = next;
        }
    }
    matches.insert(group.begin(), group.end());
    vector<pair<double, int>> results;
    for (int id : matches) {
        double score = 0.0;
        for (const string& word : positiveWords) {
            auto found = invertedIndex.find(word);
            if (found == invertedIndex.end()) continue;
            auto posting = found->second.find(id);
            if (posting == found->second.end()) continue;
            double tf = static_cast<double>(posting->second) / lengths[id];
            double idf = log((documents.size() + 1.0) / (found->second.size() + 1.0)) + 1.0;
            score += tf * idf;
        }
        results.push_back({score, id});
    }
    sort(results.begin(), results.end(), [](const auto& left, const auto& right) {
        if (left.first != right.first) return left.first > right.first;
        return left.second < right.second;
    });
    if (results.empty()) cout << "No matching documents.\n";
    for (const auto& result : results) {
        cout << documents[result.second] << "  score=" << fixed << setprecision(4) << result.first << '\n';
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) throw runtime_error("Usage: search DOCUMENT_FOLDER");
        buildIndex(argv[1]);
        cout << "Indexed " << documents.size() << " documents.\n";
        cout << "Use AND, OR, NOT. Precedence: NOT, AND, OR. Type :quit to exit.\n";
        string query;
        while (cout << "> " && getline(cin, query)) {
            if (query == ":quit") break;
            if (query.find_first_not_of(" \t\r") == string::npos) continue;
            try {
                search(query);
            } catch (const exception& error) {
                cerr << "Query error: " << error.what() << '\n';
            }
        }
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
