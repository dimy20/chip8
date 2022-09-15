#pragma once
#include <cstring>
#include <iostream>
#include <unordered_map>

#define MEMORY_SIZE 4096
#define REGISTERS_NUM 16
#define STACK_SIZE 16
#define KEY_PAD_SIZE 16
#define LOAD_ADDRESS 512



class Chip8{
	public:
		Chip8() = default;
		void init();
		void emulate_cycle();
		void load_program(const char * filename);
	private:
		// adds the value of VY to VX, VF is set to 1 if there is a carry, or 0 if
		// there isnt.
		void opcode0x_7XNN(); // Adds NN to VX. (Carry flag is not changed);

		void opcode0x_8XY0();
		void opcode0x_8XY1();  /* Sets VX to VX or VY */
		void opcode0x_8XY2();  
		void opcode0x_8XY3();  
		void opcode0x_8XY4();
		void opcode0x_8XY5(); /* VY is subtracted from VX */
		void opcode0x_8XY6();
		void opcode0x_8XY7(); /* Sets Vx to Vy minus Vx */
		void opcode0x_8XYE();

		void opcode0x_9XY0(); // skip next instruction if vx != vy

		void opcode0x_ANNN();  //  ANNN: Sets I to the address NNN
		void opcode0x_2NNN();  // calls function at adress nnn
		void opcode0x_BNNN();
		void opcode0x_CXNN();

		/* key events */
		void handle_key_events();
		void opcode0x_EX9E();
		void opcode0x_EXA1();

		void opcode0x_6XNN(); // Sets VX to NN.
		void do_nothing();
		void handle_arithmetic();

		/*timers*/
		void handle_misc_and_timers();
		void opcode_timers();
		void opcode0x_FX07();
		void opcode0x_FX15();
		void opcode0x_FX18();

		/*misc*/

		void opcode0x_FX04();
		void opcode0x_FX1E();
		void opcode0x_FX29();
		void opcode0x_FX33();
		void opcode0x_FX55();
		void opcode0x_FX65();
	private:
		unsigned short m_opcode; // 35 opcodes
		unsigned char  m_memory[MEMORY_SIZE]; // 4k memory in total
		unsigned char  m_v[REGISTERS_NUM]; // registers
		unsigned short m_i; // index register
		unsigned short m_pc; // program counter
		unsigned char  m_gfx[64 * 32]; // 2048 black and white pixels
		unsigned char  m_delay_timer;
		unsigned char  m_sound_timer;
		unsigned short m_stack[STACK_SIZE]; // stack
		unsigned short m_sp; // stack pointer
		unsigned char  m_key[KEY_PAD_SIZE]; // key pad // 0x0 - 0xf
		std::unordered_map<int, void (Chip8::*)(void)> m_global_table;
		std::unordered_map<int, void (Chip8::*)(void)> m_arithmetic_table;
		std::unordered_map<int, void (Chip8::*)(void)> m_timers_table;
		std::unordered_map<int, void (Chip8::*)(void)> m_misc_table;
};



