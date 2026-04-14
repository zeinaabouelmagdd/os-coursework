#include <openssl/des.h>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

using namespace std;

string bytesToHex(const unsigned char *data, int len) {
    stringstream ss;
    ss << uppercase << hex << setfill('0');
    for (int i = 0; i < len; i++) {
        ss << setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

uint16_t generateSalt() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint16_t> dist(0, 0xFFFF);
    return dist(gen);
}

string encryptPassword(const string &password, uint16_t salt) {
    DES_cblock key = {0};

    for (int i = 0; i < 8; i++) {
        if (i < static_cast<int>(password.length())) {
            key[i] = static_cast<unsigned char>(password[i]);
        } else {
            key[i] = 0;
        }
    }

    key[0] ^= static_cast<unsigned char>((salt >> 8) & 0xFF);
    key[1] ^= static_cast<unsigned char>(salt & 0xFF);

    DES_set_odd_parity(&key);

    DES_key_schedule schedule;
    DES_set_key_unchecked(&key, &schedule);

    DES_cblock block = {'P', 'A', 'S', 'S', 'W', 'D', '!', '!'};
    DES_cblock output;

    for (int i = 0; i < 25; i++) {
        DES_ecb_encrypt(&block, &output, &schedule, DES_ENCRYPT);
        memcpy(block, output, sizeof(DES_cblock));
    }

    stringstream ss;
    ss << uppercase << hex << setfill('0')
       << setw(4) << salt
       << "$"
       << bytesToHex(reinterpret_cast<unsigned char *>(output), 8);

    return ss.str();
}

bool checkPassword(const string &password, const string &storedHash) {
    size_t pos = storedHash.find('$');
    if (pos == string::npos) {
        return false;
    }

    string saltHex = storedHash.substr(0, pos);

    if (saltHex.length() != 4) {
        return false;
    }

    uint16_t salt;
    try {
        salt = static_cast<uint16_t>(stoul(saltHex, nullptr, 16));
    } catch (...) {
        return false;
    }

    string newHash = encryptPassword(password, salt);
    return newHash == storedHash;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage:\n";
        cout << "./encrypted_passwords gen <password>\n";
        cout << "./encrypted_passwords check <password> <stored_hash>\n";
        return 1;
    }

    string mode = argv[1];

    if (mode == "gen") {
        string password = argv[2];
        uint16_t salt = generateSalt();
        cout << encryptPassword(password, salt) << endl;
    } else if (mode == "check") {
        if (argc != 4) {
            cout << "Usage: ./encrypted_passwords check <password> <stored_hash>\n";
            return 1;
        }

        string password = argv[2];
        string storedHash = argv[3];

        if (checkPassword(password, storedHash)) {
            cout << "Valid password" << endl;
        } else {
            cout << "Invalid password" << endl;
        }
    } else {
        cout << "Invalid mode\n";
        cout << "Use:\n";
        cout << "./encrypted_passwords gen <password>\n";
        cout << "./encrypted_passwords check <password> <stored_hash>\n";
        return 1;
    }

    return 0;
}