#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class Chip8 {
public:

	void initialize();
	void load_game(const char* game);
	void emulate_cycle();

	unsigned char key[16]; //0x0-0xF
	unsigned char gfx[64 * 32]; //2048
	bool draw_flag = true;
private:
	unsigned short opcode;   
	unsigned char memory[4096];  
	unsigned char V[16];		 
	unsigned short I; //index register
	unsigned short pc; //program counter
	
	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned short stack[16];
	unsigned short sp; //stack pointer

	void execute_opcode(unsigned short opcode);
	/*
	MEMORY MAP
	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM
	*/

	typedef void(Chip8::* opcode_fun)(unsigned short);

	opcode_fun run_opcode[16] = {
		&Chip8::_0, // _0NNN, _00E0, _00EE
		&Chip8::_1NNN, &Chip8::_2NNN, &Chip8::_3XNN, &Chip8::_4XNN, &Chip8::_5XY0, &Chip8::_6XNN, &Chip8::_7XNN,
		&Chip8::_8, // _8XY0, _8XY1, _8XY2, _8XY3, _8XY4, _8XY5, _8XY6, _8XY7, _8XYE
		&Chip8::_9XY0, &Chip8::_ANNN, &Chip8::_BNNN, &Chip8::_CXNN, &Chip8::_DXYN,
		&Chip8::_E, // _EX9E, _EXA1
		&Chip8::_F, // _FX07, _FX0A, _FX15, _FX18, _FX1E, _FX29, _FX33, _FX55, _FX65
	};

	opcode_fun opcode_F[9] = {
		&Chip8::_FX07, &Chip8::_FX0A, &Chip8::_FX15,
		&Chip8::_FX18, &Chip8::_FX1E, &Chip8::_FX29,
		&Chip8::_FX33, &Chip8::_FX55, &Chip8::_FX65
	};

	opcode_fun opcode_8[9] = {
		&Chip8::_8XY0, &Chip8::_8XY1, &Chip8::_8XY2,
		&Chip8::_8XY3, &Chip8::_8XY4, &Chip8::_8XY5,
		&Chip8::_8XY6, &Chip8::_8XY7, &Chip8::_8XYE
	};

	 void _0(unsigned short opcode);
	 void _0NNN(unsigned short opcode);
	 void _00E0();
	 void _00EE();

	 void _1NNN(unsigned short opcode);
	 void _2NNN(unsigned short opcode);
	 void _3XNN(unsigned short opcode);
	 void _4XNN(unsigned short opcode);
	 void _5XY0(unsigned short opcode);
	 void _6XNN(unsigned short opcode);
	 void _7XNN(unsigned short opcode);

	 void _8(unsigned short opcode);
	 void _8XY0(unsigned short opcode);
	 void _8XY1(unsigned short opcode);
	 void _8XY2(unsigned short opcode);
	 void _8XY3(unsigned short opcode);
	 void _8XY4(unsigned short opcode);
	 void _8XY5(unsigned short opcode);
	 void _8XY6(unsigned short opcode);
	 void _8XY7(unsigned short opcode);
	 void _8XYE(unsigned short opcode);

	 void _9XY0(unsigned short opcode);
	 void _ANNN(unsigned short opcode);
	 void _BNNN(unsigned short opcode);
	 void _CXNN(unsigned short opcode);
	 void _DXYN(unsigned short opcode);

	 void _E(unsigned short opcode);
	 void _EX9E(unsigned short opcode);
	 void _EXA1(unsigned short opcode);

	 void _F(unsigned short opcode);
	 void _FX07(unsigned short opcode);
	 void _FX0A(unsigned short opcode);
	 void _FX15(unsigned short opcode);
	 void _FX18(unsigned short opcode);
	 void _FX1E(unsigned short opcode);
	 void _FX29(unsigned short opcode);
	 void _FX33(unsigned short opcode);
	 void _FX55(unsigned short opcode);
	 void _FX65(unsigned short opcode);
};