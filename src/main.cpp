#include <iostream>
#include "chip8.h"
#include <GLFW/glfw3.h>


int main(){
	Chip8 chip;
	std::cout << "hello world"<< std::endl;


	chip.init();
	chip.load_program(nullptr);

	chip.emulate_cycle();
	chip.emulate_cycle();
	//exit(0);
	return 0;
}
