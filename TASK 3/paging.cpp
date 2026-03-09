#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 2)
        return 1;

    int maxFrames = 10;

    if (argc >= 3) {
        stringstream input(argv[2]);
        input >> maxFrames;
    }

    ifstream inFile(argv[1]);
    if (!inFile)
        return 1;

    vector<int> refs;
    int pageNum;

    while (inFile >> pageNum)
        refs.push_back(pageNum);

    inFile.close();

    vector<double> results(maxFrames);

    for (int frames = 1; frames <= maxFrames; frames++) {

        vector<int> memory(frames, -1);
        vector<int> ages(frames, 0);

        int pageFaults = 0;

        for (int i = 0; i < refs.size(); i++) {

            int page = refs[i];
            int hit = -1;

            for (int j = 0; j < frames; j++)
                ages[j] = ages[j] >> 1;

            for (int j = 0; j < frames; j++) {
                if (memory[j] == page) {
                    hit = j;
                    break;
                }
            }

            if (hit != -1) {
                ages[hit] = ages[hit] | 128;
            } else {
                pageFaults++;

                int pos = -1;

                for (int j = 0; j < frames; j++) {
                    if (memory[j] == -1) {
                        pos = j;
                        break;
                    }
                }

                if (pos == -1) {
                    pos = 0;
                    for (int j = 1; j < frames; j++) {
                        if (ages[j] < ages[pos])
                            pos = j;
                    }
                }

                memory[pos] = page;
                ages[pos] = 128;
            }
        }

        results[frames - 1] = (double) pageFaults * 1000 / refs.size();
        cout << frames << " " << results[frames - 1] << endl;
    }

    return 0;
}
