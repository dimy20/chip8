#include "chip8.h"
#include <cassert>

unsigned char chip8_fontset[80] = {
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

void Chip8::handle_arithmetic(){
	const int key = (m_opcode & 0x000f);
	if(m_arithmetic_table.find(key) != m_arithmetic_table.end()){
		void (Chip8::* fn)(void) = m_arithmetic_table[key];
		(this->*fn)();
	}else{
		std::cerr << "Uknown arithmetic opcode " << key << std::endl;
	}
};

void Chip8::handle_misc_and_timers(){
	const int key = (m_opcode & 0xf000) >> 12;
	if(key == 0x07 || key == 0x15 || key == 0x18){
		if(m_timers_table.find(key) != m_timers_table.end()){
			void (Chip8::* fn)(void) = m_timers_table[key];
			(this->*fn)();
		}else{
			std::cerr << "Uknown timer opcode " << key << std::endl;
		}
	}else{
		if(m_misc_table.find(key) != m_misc_table.end()){
			void (Chip8::* fn)(void) = m_misc_table[key];
			(this->*fn)();
		}else{
			std::cerr << "Unknown misc opcode " << m_opcode << std::endl;
		}
	}
}
void Chip8::init(){
	m_pc = 0x2000;
	m_opcode = 0;
	m_i = 0;
	m_sp = 0;

	memcpy(m_memory, chip8_fontset, 80);
	memset(m_memory, 0, sizeof(unsigned char) * MEMORY_SIZE);
	memset(m_v, 0, sizeof(unsigned char) * REGISTERS_NUM);

	m_global_table[2] = &Chip8::opcode0x_2NNN;
	m_global_table[6] = &Chip8::opcode0x_6XNN;
	m_global_table[7] = &Chip8::opcode0x_7XNN; // todo
	m_global_table[8] = &Chip8::handle_arithmetic;
	m_global_table[9] = &Chip8::opcode0x_9XY0;
	m_global_table[0xb] = &Chip8::opcode0x_BNNN;
	m_global_table[0xc] = &Chip8::opcode0x_CXNN;
	m_global_table[0xe] = &Chip8::handle_key_events;
	m_global_table[0xf] = &Chip8::handle_misc_and_timers;

	// timers
	m_timers_table[0x07] = &Chip8::opcode0x_FX07;
	m_timers_table[0x15] = &Chip8::opcode0x_FX15;
	m_timers_table[0x18] = &Chip8::opcode0x_FX18;
	// misc
	m_misc_table[0x04] = &Chip8::opcode0x_FX04;
	m_misc_table[0x1E] = &Chip8::opcode0x_FX1E;
	m_misc_table[0x29] = &Chip8::opcode0x_FX29;
	m_misc_table[0x33] = &Chip8::opcode0x_FX33;
	m_misc_table[0x55] = &Chip8::opcode0x_FX55;
	m_misc_table[0x65] = &Chip8::opcode0x_FX65;
	// arithmetic
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
	m_pc += 2;
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
	m_pc += 2;
};

void Chip8::opcode0x_9XY0(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
	if(m_v[r_x] != m_v[r_y]) m_pc += 2;
	m_pc += 2;
};

void Chip8::opcode0x_7XNN(){
	unsigned int r_x = (m_opcode & 0x0f00) >> 8;
	unsigned char value = (m_opcode & 0x00ff);
	m_v[r_x] += value;
	m_pc += 2;
};

void Chip8::opcode0x_ANNN(){
	m_i = m_opcode & 0x0FFF;
	m_pc += 2;
	std::cout << "ANNN" << std::endl;
};

void Chip8::opcode0x_BNNN(){
	const char addr = (m_opcode & 0x0fff);
	m_pc = m_v[0x0] + addr;
};

void Chip8::opcode0x_CXNN(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const char value = (m_opcode & 0x00ff);
	m_v[r_x] = (rand() % 256) & value;
	m_pc += 2;
};

void Chip8::opcode0x_2NNN(){
	m_stack[++m_sp] = m_pc;
	m_pc = m_opcode & 0x0FFF;
};

void Chip8::opcode0x_6XNN(){
	unsigned int i = (m_opcode & 0x0F00) >> 8;
	unsigned char value = m_opcode & 0x00ff;
	assert(i <= REGISTERS_NUM - 1);
	m_v[i] = value;
	m_pc += 2;
};

void Chip8::handle_key_events(){
	if(m_opcode & 0x000f == 0xe) opcode0x_EX9E();
	if(m_opcode & 0x000f == 0x1) opcode0x_EXA1();
};

void Chip8::opcode0x_EX9E(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	if(m_key[m_v[r_x]]) m_pc += 2;
	m_pc += 2;
}

void Chip8::opcode0x_EXA1(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	if(!m_key[m_v[r_x]]) m_pc += 2;
	m_pc += 2;
}

void Chip8::opcode0x_FX07(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_v[r_x] = m_delay_timer;
	m_pc += 2;
}

void Chip8::opcode0x_FX15(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_delay_timer = m_v[r_x];
	m_pc += 2;
}

void Chip8::opcode0x_FX18(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_sound_timer = m_v[r_x];
	m_pc += 2;
};

void Chip8::opcode0x_FX04(){
	int i = 0;
	while(1){
		if(m_key[i]) break;
		i = (i + 1) % KEY_PAD_SIZE;
	}
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_v[r_x] = i;
	m_pc += 2;
};

void Chip8::opcode0x_FX1E(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_i += m_v[r_x];
	m_pc += 2;
};

void Chip8::opcode0x_FX29(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	unsigned char font_offset = 5 * m_v[r_x];
	m_i = font_offset;
	m_pc += 2;
};

void Chip8::opcode0x_FX55(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	int j = 0;
	while(j <= r_x){
		m_memory[m_i++] = m_v[j++];
	}
	m_i -= r_x;
	m_pc += 2;
}

void Chip8::opcode0x_FX65(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	int j = 0;
	while(j <= r_x){
		m_v[j++] = m_memory[m_i++];
	}
	m_i -= r_x;
	m_pc += 2;
};

void Chip8::opcode0x_FX33(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_memory[m_i] = m_v[r_x] / 100;
	m_memory[m_i + 1] = (m_v[r_x] / 10) % 10;
	m_memory[m_i + 2] = (m_v[r_x] % 100) % 10;
	m_pc += 2;
};

void Chip8::opcode0x_DXYN(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
	int x = m_v[r_x];
	int y = m_v[r_y];
	int n = (m_opcode & 0x000f);
	m_v[0xf] = 0;
	// each row-bytes is a bitmap of wich pixel should be drawn
	for(int row = 0; row < n; row++){
		unsigned char byte = m_memory[m_i + row];
		for(int bit_index = 0; bit_index < 8; bit_index){
			// get bit
			unsigned char bit_value = (byte >> bit_index) & 0x1;
			unsigned char * curr_pixel;
			curr_pixel = &m_gfx[(row + x) % ROWS][(bit_index + y) % COLS];
			// collision?
			if(bit_value == 1 && *curr_pixel == 1) m_v[0xf] = 1;

			//draw
			*curr_pixel ^= bit_value;
		}
	};
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
