#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class Chip8 {
    public:
        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint8_t index{};
        uint8_t pc{};
        uint8_t stack[16]{};
        uint8_t sp{};
        uint8_t dTimer{};
        uint8_t sTimer{};
        uint8_t keypad[16]{};
        uint8_t video[16 * 32]{};
        uint8_t opcode;
};