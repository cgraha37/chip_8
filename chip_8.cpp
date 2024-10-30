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
        void load_ROM(char const* filename);
        void OP_00E0();
        void OP_00EE();
        void OP_1nnn();
        void OP_2nnn();
        void OP_3xkk();
        void OP_4xkk();
        void OP_5xy0();
        void OP_6xkk();
        void OP_7xkk();
        void OP_8xy0();
        void OP_8xy1();
        void OP_8xy2();
        void OP_8xy3();
        void OP_8xy4();
        void OP_8xy5();
        void OP_8xy6();
        void OP_8xy7();
        void OP_8xyE();
        void OP_9xy0();
        void OP_Annn();
        void OP_Bnnn();
        void OP_Cxkk();
        void OP_Dxyn();
        void OP_Ex9E();
        void OP_ExA1();
        void OP_Fx07();
        void OP_Fx0A();
        void OP_Fx15();
        void OP_Fx18();
        void OP_Fx1E();
        void OP_Fx29();
        void OP_Fx33();
        void OP_Fx55();
        void OP_Fx65();

    
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
void chip8::load_ROM(char const* filename){

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

// clear display
void chip8::OP_00E0(){
	memset(video, 0, sizeof(video));
}

// return from subroutine
void chip8::OP_00EE(){
	--sp;
	pc = stack[sp];
}

// jump to nnn
void chip8::OP_1nnn(){
	uint16_t address = opcode & 0x0FFFu;

	pc = address;
}

// call subroutine at nnn
void chip8::OP_2nnn(){
	uint16_t address = opcode & 0x0FFFu;

	stack[sp] = pc;
	++sp;
	pc = address;
}

// skip next instruction if Vx = kk
void chip8::OP_3xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] == byte)
	{
		pc += 2;
	}
}

// skip next instruction if Vx != kk
void chip8::OP_4xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] != byte)
	{
		pc += 2;
	}
}

// skip next instruction if Vx = Vy
void chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] == registers[Vy])
	{
		pc += 2;
	}
}

// set Vx = kk
void chip8::OP_6xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = byte;
}

// set Vx = Vx + kk.
void chip8::OP_7xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] += byte;
}

// set Vx = Vy
void chip8::OP_8xy0(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

// set Vx = Vx OR Vy
void chip8::OP_8xy1(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}

// set Vx = Vx AND Vy
void chip8::OP_8xy2(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}

// set Vx = Vx XOR Vy
void chip8::OP_8xy3(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}

// set Vx = Vx + Vy, set VF = carry
 /*
    The values of Vx and Vy are added together. If the result 
    is greater than 8 bits (i.e., > 255,) VF is set to 1, 
    otherwise 0. Only the lowest 8 bits of the result are kept, 
    and stored in Vx.
 */
void chip8::OP_8xy4(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu;
}

// set Vx = Vx - Vy, set VF = NOT borrow
/*
   if Vx > Vy, then VF is set to 1, otherwise 0. Then Vy 
   is subtracted from Vx, and the results stored in Vx
*/

void chip8::OP_8xy5(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}

// set Vx = Vx SHR 1
/*
   if the least-significant bit of Vx is 1, then VF
   is set to 1, otherwise 0. Then Vx is divided by 2
*/

void chip8::OP_8xy6(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

// set Vx = Vy - Vx, set VF = NOT borrow
/*
    if Vy > Vx, then VF is set to 1, otherwise 0. Then 
    Vx is subtracted from Vy, and the results stored in Vx
*/

void chip8::OP_8xy7(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}