#include "chip8.h"
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void Chip8::initialize() {
	opcode = 0x0;
	memset(memory, 0x0, 4096);
	memset(V, 0x0, 16);
	I = 0; pc = 0x200;
	memset(gfx, 0x0, 64 * 32);
	delay_timer = 0;
	sound_timer = 0;
	memset(stack, 0x0, 16 * sizeof(unsigned short));
	sp = 0;
	memset(key, 0x0, 16);

	unsigned char chip8_fontset[80] =
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

	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	srand(time(NULL));

	draw_flag = true;
}

void Chip8::load_game(const char* game) {
	std::FILE* game_file;
	game_file = fopen(game, "rb");

	if (game_file != NULL) {
		int ch, counter = 0;

		ch = fgetc(game_file);
		while (ch != EOF){
			memory[512 + counter] = ch;
			printf("%02x ", ch);
			counter++;
			ch = fgetc(game_file);
		} 

		fclose(game_file);
	}
	else {
		printf("Nie udalo sie wczytac pliku %s\n", game);
	}
}

void Chip8::emulate_cycle() {
	opcode = memory[pc] << 8 | memory[pc + 1];

	execute_opcode(opcode);

	if (delay_timer > 0)
		delay_timer--;

	if (sound_timer > 0) {
		sound_timer--;
		if(sound_timer == 0)
			printf("BEEP");
	}
}

void Chip8::execute_opcode(unsigned short opcode) {
	unsigned char first_opcode_char = opcode >> 12;

	for (int i = 0; i < 16; i++) {
		if (first_opcode_char == i) {
			(this->*run_opcode[i])(opcode);
		}
	}
}

void Chip8::_0NNN(unsigned short opcode) {
	//
}
void Chip8::_00E0() {
	//	Clears the screen.
	for (int i = 0; i < 2048; i++) 
		gfx[i] = 0;
	draw_flag = true;
	pc += 2;
}
void Chip8::_00EE() {
	--sp;			
	pc = stack[sp];					
	pc += 2;
}
void Chip8::_0(unsigned short opcode) {
	switch (opcode & 0x00FF) {
		case 0x00E0: { _00E0(); break; }
		case 0x00EE: { _00EE(); break; }
		default: _0NNN(opcode);
	}
}

void Chip8::_1NNN(unsigned short opcode) {
	//Jumps to address NNN.
	pc = opcode & 0x0FFF;
}
void Chip8::_2NNN(unsigned short opcode) {
	//Calls subroutine at NNN.
	stack[sp] = pc;
	sp++;

	pc = opcode & 0x0FFF;
}
void Chip8::_3XNN(unsigned short opcode) {
	//Skips the next instruction if VX equals NN.
	if ((V[(opcode & 0x0F00) >> 8]) == (opcode & 0x00FF)) 
		pc += 4;
	else pc += 2;
}
void Chip8::_4XNN(unsigned short opcode) {
	//Skips the next instruction if VX doesn't equals NN.
	if ((V[(opcode & 0x0F00) >> 8]) != (opcode & 0x00FF)) pc += 4;
	else pc += 2;
}
void Chip8::_5XY0(unsigned short opcode) {
	//Skips the next instruction if VX equals VY.
	if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) pc += 4;
	else pc += 2;
}
void Chip8::_6XNN(unsigned short opcode) {
	//Sets VX to NN.
	V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
	pc += 2;
}
void Chip8::_7XNN(unsigned short opcode) {
	//Adds NN to VX. (Carry flag is not changed)
	V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
	pc += 2;
}

void Chip8::_8XY0(unsigned short opcode) {
	//Sets VX to the value of VY.
	V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
	pc += 2;
}
void Chip8::_8XY1(unsigned short opcode) {
	//Sets VX to VX or VY. (Bitwise OR operation)
	V[(opcode & 0x0F00) >> 8] =
		V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
	pc += 2;
}
void Chip8::_8XY2(unsigned short opcode) {
	//Sets VX to VX and VY. (Bitwise AND operation)
	V[(opcode & 0x0F00) >> 8] =
		V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
	pc += 2;
}
void Chip8::_8XY3(unsigned short opcode) {
	//Sets VX to VX xor VY.
	V[(opcode & 0x0F00) >> 8] =
		V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
	pc += 2;
}
void Chip8::_8XY4(unsigned short opcode) {
	//Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
	if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
		V[0xF] = 1; //carry
	else
		V[0xF] = 0;
	V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
	pc += 2;
}
void Chip8::_8XY5(unsigned short opcode) {
	//VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
	if (V[(opcode & 0x00F0) >> 4] > (V[(opcode & 0x0F00) >> 8]))
		V[0xF] = 0;
	else
		V[0xF] = 1;
	V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
	pc += 2;
}
void Chip8::_8XY6(unsigned short opcode) {
	//Stores the least significant bit of VX in VF and then shifts VX to the right by 1
	V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
	V[(opcode & 0x0F00) >> 8] >>= 1;
	pc += 2;
}
void Chip8::_8XY7(unsigned short opcode) {
	//0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
	if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// VY-VX
		V[0xF] = 0; // there is a borrow
	else
		V[0xF] = 1;
	V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
	pc += 2;
}
void Chip8::_8XYE(unsigned short opcode) {
	//Stores the most significant bit of VX in VF and then shifts VX to the left by 1
	V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
	V[(opcode & 0x0F00) >> 8] <<= 1;
	pc += 2;
}

void Chip8::_8(unsigned short opcode) {
	unsigned short last_opcode_char = opcode << 12;
	last_opcode_char >>= 12;

	if (last_opcode_char == 0xE) { (this->*opcode_8[8])(opcode); }
	else {
		for (int i = 0; i < 8; i++) {
			if (last_opcode_char == i) {
				(this->*opcode_8[i])(opcode);
				break;
			}
		}
	}
}

void Chip8::_9XY0(unsigned short opcode) {
	//Skips the next instruction if VX doesn't equals VY.
	if ((V[(opcode & 0x0F00) >> 8]) != (V[(opcode & 0x00F0) >> 4])) pc += 4;
	else pc += 2;
}
void Chip8::_ANNN(unsigned short opcode) {
	//Sets I to the address NNN.
	I = opcode & 0x0FFF;
	pc += 2;

}
void Chip8::_BNNN(unsigned short opcode) {
	//Jumps to the address NNN plus V0.
	pc = V[0] + (opcode & 0x0FFF);
}
void Chip8::_CXNN(unsigned short opcode) {
	//Sets VX to the result of a bitwise and operation on a random number 
	//(Typically: 0 to 255) and NN.
	V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
	pc += 2;
}
void Chip8::_DXYN(unsigned short opcode) {
	//Draws a sprite at coordinate (VX, VY)
	//that has a width of 8 pixels and a height of N pixels.
	//Each row of 8 pixels is read as bit-coded starting from memory location I;
	//I value doesn’t change after the execution of this instruction.
	//As described above, VF is set to 1 if any screen pixels
	//are flipped from set to unset when the sprite is drawn,
	//and to 0 if that doesn’t happen

	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short y = V[(opcode & 0x00F0) >> 4];
	unsigned short height = opcode & 0x000F;
	unsigned short pixel;

	V[0xF] = 0;
	for (int yline = 0; yline < height; yline++)
	{
		pixel = memory[I + yline];
		for (int xline = 0; xline < 8; xline++)
		{
			if ((pixel & (0x80 >> xline)) != 0)
			{
				if (gfx[(x + xline + ((y + yline) * 64))] == 1)
					V[0xF] = 1;
				gfx[x + xline + ((y + yline) * 64)] ^= 1;
			}
		}
	}

	draw_flag = true;
	pc += 2;
}

void Chip8::_EX9E(unsigned short opcode) {
	//Skips the next instruction if the key stored in VX is pressed.
	if (key[V[(opcode & 0x0F00) >> 8]] != 0)
		pc += 4;
	else
		pc += 2;
}
void Chip8::_EXA1(unsigned short opcode) {
	//Skips the next instruction if the key stored in VX isn't pressed. 
	if (key[V[(opcode & 0x0F00) >> 8]] == 0)
		pc += 4;
	else
		pc += 2;
}
void Chip8::_E(unsigned short opcode) {
	switch (opcode & 0x00FF) {
		case 0x009E: { _EX9E(opcode); break; }
		case 0x00A1: { _EXA1(opcode); break; }
		default: printf("Unknown opcode: 0x%X\n", opcode);
	}
}

void Chip8::_FX07(unsigned short opcode) {
	//Sets VX to the value of the delay timer.
	V[(opcode & 0x0F00) >> 8] = delay_timer;
	pc += 2;
}
void Chip8::_FX0A(unsigned short opcode) {
	//A key press is awaited, and then stored in VX.
	bool key_pressed = false;

	for (int i = 0; i < 16; i++) {
		if (key[i] == 1) {
			V[(opcode & 0x0F00) >> 8] = i;
			key_pressed = true;
		}
	}

	if (key_pressed)
		pc += 2;

}
void Chip8::_FX15(unsigned short opcode) {
	//Sets the delay timer to VX.
	delay_timer = V[(opcode & 0x0F00) >> 8];
	pc += 2;
}
void Chip8::_FX18(unsigned short opcode) {
	//Sets the sound timer to VX.
	sound_timer = V[(opcode & 0x0F00) >> 8];
	pc += 2;
}
void Chip8::_FX1E(unsigned short opcode) {
	//Adds VX to I.VF is not affected
	if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
		V[0xF] = 1;
	else
		V[0xF] = 0;

	I += V[(opcode & 0x0F00) >> 8];
	pc += 2;
}
void Chip8::_FX29(unsigned short opcode) {
	//FX29: Sets I to the location of the sprite for the character in VX.
	//Characters 0-F (in hexadecimal) are represented by a 4x5 font
	I = V[(opcode & 0x0F00) >> 8] * 0x5;
	pc += 2;
}
void Chip8::_FX33(unsigned short opcode) {
	/*Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I,
	the middle digit at I plus 1, and the least significant digit at I plus 2.
	(In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I,
	the tens digit at location I+1, and the ones digit at location I+2.)*/
	memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
	memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
	memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
	pc += 2;
}
void Chip8::_FX55(unsigned short opcode) {
	/*Stores V0 to VX (including VX) in memory starting at address I.
	The offset from I is increased by 1 for each value written, but I itself is left unmodified.*/
	for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
		memory[I + i] = V[i];

	// On the original interpreter, when the operation is done, I = I + X + 1.
	I += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
}
void Chip8::_FX65(unsigned short opcode) {
	/*Fills V0 to VX (including VX) with values from memory starting at address I.
	The offset from I is increased by 1 for each value written, but I itself is left unmodified.*/
	for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
		V[i] = memory[I + i];

	// On the original interpreter, when the operation is done, I = I + X + 1.
	I += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
}

unsigned char _Fopc[9] = {
	0x07, 0x0A, 0x15, 0x18, 0x1E, 0x29, 0x33, 0x55, 0x65
};

void Chip8::_F(unsigned short opcode) {
	unsigned short o = opcode << 8;
	o >>= 8;

	for (int i = 0; i < 9; i++) {
		if (o == _Fopc[i]) {
			(this->*opcode_F[i])(opcode);
			break;
		}
	}
}