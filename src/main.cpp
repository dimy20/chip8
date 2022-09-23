#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <sys/time.h>

#include "chip8.h"
#include "Shader.h"
#include "Screen.h"

void show_usage(){
	CHIP8_LOG("Usage: chip8 [-s] rom");
	CHIP8_LOG("-s, configure clock speed, default is 500Hz.");
}
int main(int argc, char ** argv){
	if(argc < 2){
		show_usage();
		return EXIT_FAILURE;
	};

    int opt;
	long clock_speed = 500;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
			case 's':
				clock_speed = atoi(optarg);
				if(clock_speed == 0){
					show_usage();
					return 1;
				}
				break;
			case '?':
				show_usage();
				break;
			default:
				show_usage();
				break;
			}
    }
	if(clock_speed != 500 && argc < 4){
		show_usage();
		return 1;
	}

	char * rom = argv[argc - 1];
	std::shared_ptr<Chip8> chip = std::make_shared<Chip8>(clock_speed);
	if(!chip->load_program(rom)) return EXIT_FAILURE;
	std::shared_ptr<Screen> screen = std::make_shared<Screen>(chip);

	int fps = 0;
	while(!glfwWindowShouldClose(screen->window())){
		screen->clear();
		if(glfwGetKey(screen->window(), GLFW_KEY_ESCAPE) == GLFW_PRESS){
			glfwSetWindowShouldClose(screen->window(), true);
		}

		auto chip_elapsed_time = chip->emulate_cycles(fps == 0 ? 60 : fps);
		auto render_elapsed_time = screen->render();
		long delta = (chip_elapsed_time - render_elapsed_time) / 1000;
		if(delta > 0) usleep(delta);

		screen->swap_buffers();
		fps = screen->fps();
	 };
	 screen->close();

	return 0;
}
