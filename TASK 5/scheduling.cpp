#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
using namespace std;

struct Process {
    int id;
    int arrival;
    int burst;
    int remaining;
    int waiting;
    int finish;
};

bool compareArrival(Process a, Process b) {
    return a.arrival < b.arrival;
}

double fcfs(vector<Process> p) {
    sort(p.begin(), p.end(), compareArrival);

    int currentTime = 0;
    int totalWaiting = 0;

    for (int i = 0; i < p.size(); i++) {
        if (currentTime < p[i].arrival)
            currentTime = p[i].arrival;

        p[i].waiting = currentTime - p[i].arrival;
        totalWaiting += p[i].waiting;
        currentTime += p[i].burst;
    }

    return (double) totalWaiting / p.size();
}

double sjf(vector<Process> p) {
    int n = p.size();
    int completed = 0;
    int currentTime = 0;
    int totalWaiting = 0;
    vector<bool> done(n, false);

    while (completed < n) {
        int idx = -1;
        int shortest = 1000000;

        for (int i = 0; i < n; i++) {
            if (!done[i] && p[i].arrival <= currentTime && p[i].burst < shortest) {
                shortest = p[i].burst;
                idx = i;
            }
        }

        if (idx == -1) {
            currentTime++;
        } else {
            p[idx].waiting = currentTime - p[idx].arrival;
            totalWaiting += p[idx].waiting;
            currentTime += p[idx].burst;
            done[idx] = true;
            completed++;
        }
    }

    return (double) totalWaiting / n;
}

double roundRobin(vector<Process> p, int quantum) {
    int n = p.size();
    sort(p.begin(), p.end(), compareArrival);

    queue<int> q;
    int currentTime = 0;
    int completed = 0;
    int i = 0;
    int totalWaiting = 0;

    while (completed < n) {
        while (i < n && p[i].arrival <= currentTime) {
            q.push(i);
            i++;
        }

        if (q.empty()) {
            currentTime++;
            continue;
        }

        int idx = q.front();
        q.pop();

        if (p[idx].remaining > quantum) {
            currentTime += quantum;
            p[idx].remaining -= quantum;

            while (i < n && p[i].arrival <= currentTime) {
                q.push(i);
                i++;
            }

            q.push(idx);
        } else {
            currentTime += p[idx].remaining;
            p[idx].remaining = 0;
            p[idx].finish = currentTime;

            p[idx].waiting = p[idx].finish - p[idx].arrival - p[idx].burst;
            totalWaiting += p[idx].waiting;
            completed++;

            while (i < n && p[i].arrival <= currentTime) {
                q.push(i);
                i++;
            }
        }
    }

    return (double) totalWaiting / n;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cout << "Usage: ./scheduling input.txt\n";
        return 1;
    }

    ifstream file(argv[1]);

    if (!file) {
        cout << "Error opening file\n";
        return 1;
    }

    int n;
    file >> n;

    vector<Process> processes(n);

    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        file >> processes[i].arrival >> processes[i].burst;
        processes[i].remaining = processes[i].burst;
    }

    int quantum;
    file >> quantum;

    cout << "Average waiting time FCFS: " << fcfs(processes) << endl;
    cout << "Average waiting time SJF: " << sjf(processes) << endl;
    cout << "Average waiting time Round Robin: " << roundRobin(processes, quantum) << endl;

    return 0;
}
