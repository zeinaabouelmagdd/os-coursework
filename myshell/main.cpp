#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include <unistd.h>
#include <limits.h>
#include <cstdlib>
#include <sys/wait.h>
#include <cstring>
#include <cerrno>

extern char **environ;

// split input into words
static std::vector<std::string> split_words(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string w;
    while (iss >> w) tokens.push_back(w);
    return tokens;
}

int main(int argc, char* argv[]) {
    std::istream* in = &std::cin;
    std::ifstream batchFile;
    bool interactive = true;

    // check if batch file
    if (argc == 2) {
        batchFile.open(argv[1]);
        if (!batchFile) {
            std::cout << "Error: cannot open batch file: " << argv[1] << "\n";
            return 1;
        }
        in = &batchFile;
        interactive = false;
    } else if (argc > 2) {
        std::cout << "Usage: ./myshell [batchfile]\n";
        return 1;
    }

    // main loop
    while (true) {

        // clean finished background processes
        while (waitpid(-1, nullptr, WNOHANG) > 0) {}

        // print prompt
        if (interactive) {
            char buffer[PATH_MAX];
            if (getcwd(buffer, sizeof(buffer)) != nullptr) {
                std::cout << buffer << " > " << std::flush;
            } else {
                std::cout << "myshell> " << std::flush;
            }
        }

        // read line
        std::string line;
        if (!std::getline(*in, line)) {
            break;
        }

        // tokenize
        auto tokens = split_words(line);
        if (tokens.empty()) continue;

        // check background &
        bool background = false;
        if (!tokens.empty() && tokens.back() == "&") {
            background = true;
            tokens.pop_back();
        }
        if (tokens.empty()) continue;

        std::string cmd = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());

        // quit command
        if (cmd == "quit") {
            break;
        }

        // pause command
        if (cmd == "pause") {
            std::cout << "Press Enter to continue..." << std::endl;
            std::string dummy;
            std::getline(std::cin, dummy);
            continue;
        }

        // echo command
        if (cmd == "echo") {
            for (size_t i = 0; i < args.size(); i++) {
                if (i > 0) std::cout << " ";
                std::cout << args[i];
            }
            std::cout << "\n";
            continue;
        }

        // cd command
        if (cmd == "cd") {
            char buffer[PATH_MAX];

            if (args.empty()) {
                if (getcwd(buffer, sizeof(buffer)) != nullptr) {
                    std::cout << buffer << "\n";
                } else {
                    std::perror("getcwd");
                }
                continue;
            }

            if (chdir(args[0].c_str()) != 0) {
                std::perror("cd");
            } else {
                if (getcwd(buffer, sizeof(buffer)) != nullptr) {
                    setenv("PWD", buffer, 1);
                }
            }
            continue;
        }

        // dir command
        if (cmd == "dir") {
            if (args.empty()) {
                std::cout << "dir: missing directory argument\n";
                continue;
            }

            try {
                for (const auto& entry : std::filesystem::directory_iterator(args[0])) {
                    std::cout << entry.path().filename().string() << "\n";
                }
            } catch (const std::filesystem::filesystem_error& e) {
                std::cout << "dir: " << e.what() << "\n";
            }
            continue;
        }

        // environ command
        if (cmd == "environ") {
            for (char **env = environ; *env != nullptr; env++) {
                std::cout << *env << "\n";
            }
            continue;
        }

        // set command
        if (cmd == "set") {
            if (args.size() < 2) {
                std::cout << "set: usage: set VARIABLE VALUE\n";
                continue;
            }

            std::string variable = args[0];
            std::string value;
            for (size_t i = 1; i < args.size(); i++) {
                if (i > 1) value += " ";
                value += args[i];
            }

            if (setenv(variable.c_str(), value.c_str(), 1) != 0) {
                std::perror("set");
            }
            continue;
        }

        // help command
        if (cmd == "help") {
            std::ifstream file("help.txt");
            if (!file) {
                std::cout << "help: could not open help.txt\n";
                continue;
            }

            std::string helpLine;
            while (std::getline(file, helpLine)) {
                std::cout << helpLine << "\n";
            }
            continue;
        }

        // external commands
        pid_t pid = fork();

        if (pid < 0) {
            std::cout << "fork failed: " << std::strerror(errno) << "\n";
            continue;
        }

        if (pid == 0) {
            std::vector<char*> argv_exec;
            argv_exec.reserve(tokens.size() + 1);

            for (auto& s : tokens) {
                argv_exec.push_back(const_cast<char*>(s.c_str()));
            }
            argv_exec.push_back(nullptr);

            execvp(argv_exec[0], argv_exec.data());

            std::cout << argv_exec[0] << ": command not found or failed to run\n";
            std::cout << "reason: " << std::strerror(errno) << "\n";
            std::_Exit(1);
        } else {
            if (!background) {
                int status = 0;
                waitpid(pid, &status, 0);
            } else {
                std::cout << "[background pid " << pid << "]\n";
            }
        }
    }

    return 0;
}
