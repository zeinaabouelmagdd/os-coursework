#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

bool canSatisfy(const vector<int>& request, const vector<int>& available) {
    for (size_t j = 0; j < request.size(); j++) {
        if (request[j] > available[j]) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file) {
        cerr << "Error: cannot open input file.\n";
        return 1;
    }

    int n, m;  // n = number of processes, m = number of resource types
    file >> n >> m;

    vector<int> E(m);
    for (int j = 0; j < m; j++) {
        file >> E[j];
    }

    vector<vector<int>> C(n, vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            file >> C[i][j];
        }
    }

    vector<vector<int>> R(n, vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            file >> R[i][j];
        }
    }

    // Compute available vector A = E - sum of allocated resources
    vector<int> A(m, 0);
    for (int j = 0; j < m; j++) {
        int allocatedSum = 0;
        for (int i = 0; i < n; i++) {
            allocatedSum += C[i][j];
        }
        A[j] = E[j] - allocatedSum;
    }

    vector<bool> finished(n, false);
    bool progress = true;

    while (progress) {
        progress = false;

        for (int i = 0; i < n; i++) {
            if (!finished[i] && canSatisfy(R[i], A)) {
                // Pretend process i finishes and releases its resources
                for (int j = 0; j < m; j++) {
                    A[j] += C[i][j];
                }
                finished[i] = true;
                progress = true;
            }
        }
    }

    vector<int> deadlocked;
    for (int i = 0; i < n; i++) {
        if (!finished[i]) {
            deadlocked.push_back(i);
        }
    }

    if (deadlocked.empty()) {
        cout << "No deadlock detected.\n";
    } else {
        cout << "Deadlock detected.\n";
        cout << "Deadlocked processes: ";
        for (size_t i = 0; i < deadlocked.size(); i++) {
            cout << "P" << deadlocked[i];
            if (i + 1 < deadlocked.size()) cout << " ";
        }
        cout << "\n";
    }

    return 0;
}