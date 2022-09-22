#include "chip8.h"
#include <cassert>
#include <unistd.h>

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
	const int key = (m_opcode & 0x00ff);
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

Chip8::Chip8(){ init(); };

void Chip8::init(){
	m_pc = 0x0200;
	m_opcode = 0;
	m_i = 0;
	m_sp = 0;

	memset(m_memory, 0, sizeof(unsigned char) * MEMORY_SIZE);
	memcpy(m_memory, chip8_fontset, 80);
	memset(m_v, 0, sizeof(unsigned char) * REGISTERS_NUM);
	memset(m_gfx, 0, sizeof(char) * (32 * 64));



	m_global_table[0] = &Chip8::handle_table0;
	m_global_table[1] = &Chip8::opcode0x_1NNN;
	m_global_table[2] = &Chip8::opcode0x_2NNN;
	m_global_table[3] = &Chip8::opcode0x_3XNN;
	m_global_table[4] = &Chip8::opcode0x_4XNN;
	m_global_table[5] = &Chip8::opcode0x_5XY0;
	m_global_table[6] = &Chip8::opcode0x_6XNN;
	m_global_table[7] = &Chip8::opcode0x_7XNN; // todo
	m_global_table[8] = &Chip8::handle_arithmetic;
	m_global_table[9] = &Chip8::opcode0x_9XY0;
	m_global_table[0xa] = &Chip8::opcode0x_ANNN;
	m_global_table[0xb] = &Chip8::opcode0x_BNNN;
	m_global_table[0xc] = &Chip8::opcode0x_CXNN;
	m_global_table[0xd] = &Chip8::opcode0x_DXYN;
	m_global_table[0xe] = &Chip8::handle_key_events;
	m_global_table[0xf] = &Chip8::handle_misc_and_timers;

	// timers
	m_timers_table[0x07] = &Chip8::opcode0x_FX07;
	m_timers_table[0x15] = &Chip8::opcode0x_FX15;
	m_timers_table[0x18] = &Chip8::opcode0x_FX18;
	// misc
	m_misc_table[0x0A] = &Chip8::opcode0x_FX0A;
	m_misc_table[0x1E] = &Chip8::opcode0x_FX1E;
	m_misc_table[0x29] = &Chip8::opcode0x_FX29;
	m_misc_table[0x33] = &Chip8::opcode0x_FX33;
	m_misc_table[0x55] = &Chip8::opcode0x_FX55;
	m_misc_table[0x65] = &Chip8::opcode0x_FX65;
	// arithmetic
	m_arithmetic_table[0x0] = &Chip8::opcode0x_8XY0;
	m_arithmetic_table[0x1] = &Chip8::opcode0x_8XY1;
	m_arithmetic_table[0x2] = &Chip8::opcode0x_8XY2;
	m_arithmetic_table[0x3] = &Chip8::opcode0x_8XY3;
	m_arithmetic_table[0x4] = &Chip8::opcode0x_8XY4;
	m_arithmetic_table[0x5] = &Chip8::opcode0x_8XY5;
	m_arithmetic_table[0x6] = &Chip8::opcode0x_8XY6;
	m_arithmetic_table[0x7] = &Chip8::opcode0x_8XY7;
	m_arithmetic_table[0xE] = &Chip8::opcode0x_8XYE;
	//table0
	m_table0[0x0] = &Chip8::opcode0x_00E0;
	m_table0[0xE] = &Chip8::opcode0x_00EE;
};

void Chip8::handle_table0(){
	const int key = (m_opcode & 0x000f);
	if(m_table0.find(key) != m_table0.end()){
		void (Chip8::* fn)(void) = m_table0[key];
		(this->*fn)();
	}else{
		std::cerr << "Uknown table0 opcode " << key << std::endl;
	}
};
bool Chip8::load_program(const char * filename){
	assert(filename != nullptr);

	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	int size;

	size = file.tellg();
	if(size > (MEMORY_SIZE - LOAD_ADDRESS)){
		std::cerr << "Rom " << filename << " is too big." << std::endl;
		return false;
	}
	char * rom = static_cast<char*>(malloc(sizeof(char) * size));
	if(!rom){
		std::cerr << "failed to load Rom " << filename << std::endl;
		return false;
	}

	file.seekg(0, std::ios::beg);
	file.read(rom, size);

	// load
	std::cout << "Loading rom " << filename << " ..." << std::endl;
	memcpy(m_memory + LOAD_ADDRESS, rom, size);
	return true;
};

void Chip8::opcode0x_00E0(){
	memset(m_gfx, 0, ROWS * COLS * sizeof(unsigned char));
	m_pc += 2;
};

void Chip8::opcode0x_00EE(){
	m_pc = m_stack[--m_sp];
};

void Chip8::opcode0x_8XY0(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
	m_v[r_x] = m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY1(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
	m_v[r_x] |= m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY2(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
	m_v[r_x] &= m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY3(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
	m_v[r_x] ^= m_v[r_y];
	m_pc += 2;
};

void Chip8::opcode0x_8XY4(){
	unsigned int r_x = (m_opcode & 0x0f00) >> 8;
	unsigned int r_y = (m_opcode & 0x00f0) >> 4;
	if(m_v[r_y]  + m_v[r_x] > 0x0FF)
		m_v[0xF] = 1; // set carry flag
	else
		m_v[0xF] = 0;
	m_v[r_x] += m_v[r_y];
	m_pc += 2;
}

void Chip8::opcode0x_8XY5(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
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
	const int r_y = (m_opcode & 0x00f0) >> 4;
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
	m_i = m_opcode & 0x0fff;
	m_pc += 2;
};

void Chip8::opcode0x_BNNN(){
	const short addr = (m_opcode & 0x0fff);
	m_pc = m_v[0x0] + addr;
};

void Chip8::opcode0x_CXNN(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const char value = (m_opcode & 0x00ff);
	m_v[r_x] = (rand() % 256) & value;
	m_pc += 2;
};

void Chip8::opcode0x_2NNN(){
	m_stack[m_sp++] = m_pc + 2;
	m_pc = m_opcode & 0x0FFF;
};

void Chip8::opcode0x_1NNN(){
	const short addr = m_opcode & 0x0fff;
	m_pc = addr;
	//std::cout << "1NNN" << addr << std::endl;
};

void Chip8::opcode0x_3XNN(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	if(m_v[r_x] == (m_opcode & 0x00ff)) m_pc += 2;
	m_pc += 2;
	//std::cout << "3Xnn" << std::endl;
};

void Chip8::opcode0x_4XNN(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	if(m_v[r_x] != (m_opcode & 0x00ff)) m_pc += 2;
	m_pc += 2;
	//std::cout << "4Xnn" << std::endl;
};

void Chip8::opcode0x_5XY0(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
	if(m_v[r_x] == m_v[r_y]) m_pc += 2;
	m_pc += 2;
	//std::cout << "5xy0" << std::endl;
};

void Chip8::opcode0x_6XNN(){
	unsigned int i = (m_opcode & 0x0F00) >> 8;
	unsigned char value = m_opcode & 0x00ff;
	assert(i <= REGISTERS_NUM - 1);
	m_v[i] = value;
	m_pc += 2;
};

void Chip8::handle_key_events(){
	if((m_opcode & 0x000f) == 0xe) opcode0x_EX9E();
	if((m_opcode & 0x000f) == 0x1) opcode0x_EXA1();
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

void Chip8::opcode0x_FX0A(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_wait_key = r_x;
	m_pc += 2;
	m_interrupt = true;
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
	for(int i = 0; i <= r_x; i++){
		m_memory[i + m_i] = m_v[i];
	}
	m_pc += 2;
}

void Chip8::opcode0x_FX65(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	for(int i = 0; i <= r_x; i++){
		m_v[i] = m_memory[i + m_i];
	}
	m_pc += 2;
};

void Chip8::opcode0x_FX33(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	m_memory[m_i] = m_v[r_x] / 100;
	m_memory[m_i + 1] = (m_v[r_x] / 10) % 10;
	m_memory[m_i + 2] = (m_v[r_x] % 10);
	m_pc += 2;
};


void Chip8::opcode0x_DXYN(){
	const int r_x = (m_opcode & 0x0f00) >> 8;
	const int r_y = (m_opcode & 0x00f0) >> 4;
	int x = m_v[r_x];
	int y = m_v[r_y];
	int n = (m_opcode & 0x000f);
	m_v[0xf] = 0;

	// each row-byte is a bitmap of wich pixel should be drawn
	for(int row = 0; row < n && (row + y) < 32;  row++){
		unsigned char byte = m_memory[m_i + row]; // sprite
		for(int bit_index = 0; bit_index < 8; bit_index++){
			if(((byte >> (7 - bit_index)) & 1) == 1){
				if(m_gfx[((row + y) * 64) + (x + bit_index) % 2048] == 1)	
					m_v[0xf] = 1;
				m_gfx[((row + y) * 64) + (x + bit_index) % 2048] ^= 1;
			}
		}
	};
	m_pc += 2;
	m_interrupt = true;
};

long Chip8::emulate_cycles(){
	int n = CYCLES_PER_FRAME;
	int i = 0;
	if(m_wait_key == NO_KEY_WAIT){
		for(; i < n; i++){
			// cycle
			m_opcode = m_memory[m_pc] << 8 | m_memory[m_pc + 1];
			const int key = (m_opcode & 0xf000) >> 12;
			if(m_global_table.find(key) != m_global_table.end()){
				void (Chip8::* tmp)(void) = m_global_table[key];
				(this->*tmp)();
			}else{
				std::cerr << "Unknown opcode [0X0000]: " << m_opcode << std::endl;
			}
			if(m_interrupt) break;
		}
	}
	m_interrupt = false;
	if(m_delay_timer > 0) m_delay_timer--;
	if(m_sound_timer > 0 && --m_sound_timer == 0) std::cout << "BEEP!" << std::endl;

	// how long do these cycles take in chip time?
	const long sec_ns = (1000 * 1000 * 1000);
	long elapsed_time = (i * sec_ns) / CHIP_CLOCK_SPEED;
	return elapsed_time;
}

void Chip8::key_pressed(unsigned char key){
	assert(key <= 0xf);
	assert(m_wait_key != NO_KEY_WAIT);
	m_v[m_wait_key] = key;
}
