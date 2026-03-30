#include <iostream>
#include <filesystem>
#include <map>
#include <iomanip>
#include <string>

namespace fs = std::filesystem;
using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <start_directory> <bin_width>\n";
        return 1;
    }

    string startDir = argv[1];
    long long binWidth;

    try {
        binWidth = stoll(argv[2]);
        if (binWidth <= 0) {
            cerr << "Error: bin width must be a positive number.\n";
            return 1;
        }
    } catch (...) {
        cerr << "Error: invalid bin width.\n";
        return 1;
    }

    if (!fs::exists(startDir) || !fs::is_directory(startDir)) {
        cerr << "Error: invalid starting directory.\n";
        return 1;
    }

    map<long long, long long> histogram;
    long long totalFiles = 0;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(startDir)) {
            try {
                if (fs::is_regular_file(entry.path())) {
                    long long fileSize = fs::file_size(entry.path());
                    long long binStart = (fileSize / binWidth) * binWidth;
                    histogram[binStart]++;
                    totalFiles++;
                }
            } catch (const fs::filesystem_error&) {
                continue;
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << "\n";
        return 1;
    }

    cout << "\nHistogram of File Sizes\n";
    cout << "Start Directory: " << startDir << "\n";
    cout << "Bin Width: " << binWidth << " bytes\n";
    cout << "Total Files: " << totalFiles << "\n\n";

    if (histogram.empty()) {
        cout << "No regular files found.\n";
        return 0;
    }

    for (const auto& [binStart, count] : histogram) {
        long long binEnd = binStart + binWidth - 1;

        cout << setw(10) << binStart << " - "
             << setw(10) << binEnd << " : "
             << count << " ";

        for (long long i = 0; i < count; i++) {
            cout << "*";
        }
        cout << "\n";
    }

    return 0;
}