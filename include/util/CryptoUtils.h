#pragma once
#include <string>
#include <vector>
#include <random>

namespace Util {

class CryptoUtils {
public:
    static const int SALT_SIZE = 8;
    static const int HASH_SIZE = 16;

    static void generateSalt(unsigned char* salt);
    static void hashPassword(const std::string& password, const unsigned char* salt, unsigned char* hash);

    class StreamCipher {
    public:
        StreamCipher(const std::string& password, const unsigned char* salt);
        void process(char* buffer, size_t size);
        unsigned char processByte(unsigned char b);
        
        // Save/Restore PRNG state
        std::string saveState() const;
        void restoreState(const std::string& state);

    private:
        std::mt19937 m_engine;
    };
};

}
