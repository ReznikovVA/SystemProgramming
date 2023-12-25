#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h>
#include <string>
#include <algorithm>
#include <thread>
#include <vector>
//schitaem hash
std::string calculate_md5_hash(const std::string &data) {
    MD5_CTX md5Context;
    MD5_Init(&md5Context);
    MD5_Update(&md5Context, data.c_str(), data.size());

    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_Final(digest, &md5Context);

    std::ostringstream oss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    return oss.str();
}

void iterateCombinations(int length, std::string prefix, const std::string &target_hash) {
    if (length == 0) {
        std::string hash_guess = calculate_md5_hash(prefix);
        if (hash_guess == target_hash) {
            std::cout << "Match found: '" << prefix << "' hash is '" << target_hash << "'" << std::endl;
        }
        return;
    }

    std::string all_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (char c : all_chars) {
        iterateCombinations(length - 1, prefix + c, target_hash);
    }
}

void checkCombinations(const std::string &target_hash, int length, int start, int end) {
    std::string all_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = start; i < end; ++i) {
        std::string prefix(1, all_chars[i]);
        iterateCombinations(length - 1, prefix, target_hash);
    }
}

void mainFunction(const std::string &input_data) {
    std::string target_hash = calculate_md5_hash(input_data);
    std::cout << "MD5 hash for the input data '" << input_data << "': " << target_hash << std::endl;

    int length = input_data.length(); //schitaem dlinu sekreta

    const int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    int chunkSize = 62 / num_threads; //delim alphanumer. znacheniya na kol-vo threads

//prohodimsya po threadam
    for (int i = 0; i < num_threads; ++i) {
        int start = i * chunkSize;
        int end = (i == num_threads - 1) ? 62 : (i + 1) * chunkSize; //uchitivaem nechetniy sluchai
        threads.emplace_back(checkCombinations, std::ref(target_hash), length, start, end);
    }

//todo peredelat na zaversheniye pri nahozhdenii
//zhdem zaversheniya vsex threadov
    for (auto &t : threads) {
        t.join();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_data>" << std::endl;
        return 1;
    }

    std::string input_data = argv[1];
    mainFunction(input_data);
    return 0;
}
