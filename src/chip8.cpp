#include "chip8.h"
#include <cassert>

void Chip8::arithmetic_forward(){
	const int key = (m_opcode & 0x000f);
	if(m_arithmetic_table.find(key) != m_arithmetic_table.end()){
		void (Chip8::* fn)(void) = m_arithmetic_table[key];
		(this->*fn)();
	}else{
		std::cerr << "Uknown arithmetic opcode" << std::endl;
	}
};
void Chip8::do_nothing(){

};

void Chip8::init(){
	m_pc = 0x2000;
	m_opcode = 0;
	m_i = 0;
	m_sp = 0;

	memset(m_memory, 0, sizeof(unsigned char) * MEMORY_SIZE);
	memset(m_v, 0, sizeof(unsigned char) * REGISTERS_NUM);
	m_global_table[2] = &Chip8::opcode0x_2NNN;
	m_global_table[6] = &Chip8::opcode0x_6XNN;
	m_global_table[7] = &Chip8::opcode0x_7XNN;
	m_global_table[8] = &Chip8::arithmetic_forward;

	m_arithmetic_table[0] = &Chip8::opcode0x_8XY0;
	m_arithmetic_table[1] = &Chip8::opcode0x_8XY1;
	m_arithmetic_table[2] = &Chip8::opcode0x_8XY2;
	m_arithmetic_table[3] = &Chip8::opcode0x_8XY3;
	m_arithmetic_table[4] = &Chip8::opcode0x_8XY4;
	m_arithmetic_table[5] = &Chip8::opcode0x_8XY5;
	m_arithmetic_table[6] = &Chip8::opcode0x_8XY6;
	m_arithmetic_table[7] = &Chip8::opcode0x_8XY7;
	m_arithmetic_table[0xE] = &Chip8::opcode0x_8XYE;
};

void Chip8::load_program(const char * filename){
	unsigned short i = 0x6202;
	unsigned short i2  = 0x8124;
	m_memory[m_pc] = (i & 0xFF00) >> 8;
	m_memory[m_pc + 1] = i & 0x00FF;
	m_memory[m_pc + 2] = (i2 & 0xFF00) >> 8;
	m_memory[m_pc + 3] = i2 & 0x00FF;
	//std::cout << (i & 0x00FF) << std::endl;
};


void Chip8::opcode0x_8XY0(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x0f00) >> 4;
	m_v[r_x] = m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY1(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x0f00) >> 4;
	m_v[r_x] |= m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY2(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x0f00) >> 4;
	m_v[r_x] &= m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY3(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x0f00) >> 4;
	m_v[r_x] ^= m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY4(){
	unsigned int r_x = (m_opcode & 0x0f00) >> 8;
	unsigned int r_y = (m_opcode & 0x00f0) >> 4;
	std::cout << "adding r_" << r_y << " to r_" << r_x << std::endl;
	if(m_v[r_y]  + m_v[r_x] > 0x0FF)
		m_v[0xF] = 1; // set carry flag
	else
		m_v[0xF] = 0;
	m_v[r_x] += m_v[r_y];
	m_pc += 2;
	std::cout << (int)m_v[r_x] << std::endl;
}

void Chip8::opcode0x_8XY5(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x0f00) >> 4;
	m_v[0xf] = m_v[r_x] - m_v[r_y] < 0 ? 0 : 1;
	m_v[r_x] -= m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY6(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_v[0xf] = 0x1 & m_v[r_x];
	m_v[r_x] >>=1;
};

void Chip8::opcode0x_8XY7(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x0f00) >> 4;
	m_v[0xf] = m_v[r_y] - m_v[r_x] < 0 ? 0 : 1;
	m_v[r_x] = m_v[r_y] - m_v[r_x];
	m_pc += 2;
};

void Chip8::opcode0x_8XYE(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_v[0xf] = (m_v[r_x] >> 7) & 0x1;
	m_v[r_x] <<= 1;
};

void Chip8::opcode0x_7XNN(){
	unsigned int r_x = (m_opcode & 0x0f00) >> 8;
	unsigned char value = (m_opcode & 0x00ff);
	m_v[r_x] += value;
};

void Chip8::opcode0x_ANNN(){
	m_i = m_opcode & 0x0FFF;
	m_pc += 2;
	std::cout << "ANNN" << std::endl;
};

void Chip8::opcode0x_2NNN(){
	m_stack[++m_sp] = m_pc;
	m_pc = m_opcode & 0x0FFF;
};

void Chip8::opcode0x_FX33(){
	std::cout << "TEST" << std::endl;
};


void Chip8::opcode0x_6XNN(){
	unsigned int i = (m_opcode & 0x0F00) >> 8;
	unsigned char value = m_opcode & 0x00ff;
	assert(i <= REGISTERS_NUM - 1);
	m_v[i] = value;
	m_pc += 2;
};

void Chip8::emulate_cycle(){
	m_opcode = m_memory[m_pc] << 8 | m_memory[m_pc + 1];
	const int key = (m_opcode & 0xf000) >> 12;
	if(m_global_table.find(key) != m_global_table.end()){
		void (Chip8::* tmp)(void) = m_global_table[key];
		(this->*tmp)();
	}else{
		std::cerr << "Unknown opcode [0X0000]: " << m_opcode << std::endl;
		exit(EXIT_FAILURE);
	}
	/*
	switch(m_opcode & 0xF000){
		case 0xA000: opcode0x_ANNN(); break;
		case 0X2000: opcode0x_2NNN(); break;
		case 0x8000: opcode0x_8XY4(); break;
		case 0x0033: opcode0x_FX33(); break;
		case 0x6000: opcode0x_6XNN(); break;
		case 0X0000:
			switch(m_opcode & 0X000F){
				case 0X0000: // 0X00E0 - Clears the screen
				break;
				case 0X000E: // 0X00EE - returns from subroutine
				break;
				default:
					std::cerr << "Unknown opcode [0X0000]: " << m_opcode << std::endl;
					exit(EXIT_FAILURE);
			}
		default:
			std::cerr << "Unknown opcode : " << m_opcode << std::endl;
			exit(EXIT_FAILURE);
	}
	*/
	if(m_delay_timer > 0) m_delay_timer--;
	if(m_sound_timer > 0){
		if(m_sound_timer == 1) std::cout << "BEEP!" << std::endl;
		--m_sound_timer;
	}
}
