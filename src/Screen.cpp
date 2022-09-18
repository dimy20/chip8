#include "Screen.h"
static void pixel_impl_init(pixel_impl_t * pixel){
	assert(pixel != nullptr);
    float vertices[] = {
		-0.5f, 0.5f,
		-0.5f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.5f
    };
	unsigned int indices[] = {0, 1, 2, 
							0, 3, 2};

	glGenVertexArrays(1, &pixel->vao);
	glGenBuffers(1, &pixel->vbo);
	glGenBuffers(1, &pixel->ebo);
	glBindVertexArray(pixel->vao);

	glBindBuffer(GL_ARRAY_BUFFER, pixel->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pixel->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
};

static void window_init(GLFWwindow ** window){
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	*window = glfwCreateWindow(1024, 768, "Chip8 emulator", NULL, NULL);
	if(!(*window)){
		glfwTerminate();
		return;
	};

	glfwMakeContextCurrent(*window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	gladLoadGL();
	glViewport(0, 0, 1024, 768);
}

Screen::Screen(){
	window_init(&m_window);
	m_shader = std::make_shared<Shader>("../shaders/vert.glsl", "../shaders/frag.glsl");
	pixel_impl_init(&m_pixel);
};

void Screen::draw_pixel(int x, int y){
	m_shader->use();
	glBindVertexArray(m_pixel.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
};

Screen::operator bool(){
	return !glfwWindowShouldClose(m_window);
};


void Screen::swap_buffers(){
	glfwPollEvents();    
	glfwSwapBuffers(m_window); //swap buffer
};

void Screen::close(){ glfwTerminate(); };
void Screen::clear(){ glClear(GL_COLOR_BUFFER_BIT);};
