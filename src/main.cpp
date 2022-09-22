#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <sys/time.h>

#include "chip8.h"
#include "Shader.h"
#include "Screen.h"

int main(int argc, char ** argv){
	if(argc < 2){
		std::cerr << "Usage: chip8 [OPTIONS] rom" << std::endl;
		return 1;
	}

	char * rom = argv[1];
	std::shared_ptr<Chip8> chip = std::make_shared<Chip8>();
	if(!chip->load_program(rom)) return EXIT_FAILURE;
	std::shared_ptr<Screen> screen = std::make_shared<Screen>(chip);

	while(!glfwWindowShouldClose(screen->window())){
		screen->clear();
		if(glfwGetKey(screen->window(), GLFW_KEY_ESCAPE) == GLFW_PRESS){
			glfwSetWindowShouldClose(screen->window(), true);
		}

		auto chip_elapsed_time = chip->emulate_cycles();
		auto render_elapsed_time = screen->render();
		long delta = (chip_elapsed_time - render_elapsed_time) / 1000;
		if(delta > 0) usleep(delta);

		screen->swap_buffers();
	 };
	 screen->close();

	return 0;
}
