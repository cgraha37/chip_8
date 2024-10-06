#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <chrono>
#include <random>

using namespace std;

class chip8 {
    public:
        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint8_t index{};
        uint8_t pc{};
        uint8_t stack[16]{};
        uint8_t sp{};
        uint8_t d_Timer{};
        uint8_t s_Timer{};
        uint8_t keypad[16]{};
        uint8_t video[16 * 32]{};
        uint8_t opcode;

        chip8();
        void loadROM(char const* filename);
    
    private:
        std::default_random_engine        rand_gen;
        uniform_int_distribution<uint8_t> rand_byte;
};

// where the memory will begin
const unsigned int START_ADDRESS = 0x200;

// where the fonts will be stored in memory
const unsigned int FONTSET_START_ADDRESS = 0x50;

// size of fontset array
const unsigned int FONTSET_SIZE = 80;

// array of required characters
uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// class constructor
chip8::chip8() : rand_gen(chrono::system_clock::now().time_since_epoch().count())
{

    // initialize PC
    pc = START_ADDRESS;

    // load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; i++){
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // initialize RNG
    rand_byte = uniform_int_distribution<uint8_t>(0, 255U);
}

// function that will load the ROM into memory
void chip8::loadROM(char const* filename){

    // open and read ROM as binary, move fp to the end of the file
    ifstream file(filename, ios::binary | ios::ate);

    if (file.is_open()) {

        // allocate a buffer using the size of the file
        streampos size = file.tellg();
        char* buffer = new char[size];

        // move fp to beginning of the file and read it into the buffer
        file.seekg(0, ios::beg);
        file.read(buffer, size);
        file.close();

        // load ROM contents into chip8's memory
        for(long i = 0; i < size; i++){
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }

}