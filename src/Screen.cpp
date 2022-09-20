#include "Screen.h"
#include "stb_image.h"
void Screen::init(){
	 float vertices[] = {
        // display rect     // texture coords
         0.5f,  0.5f,	     1.0f, 1.0f, // top right
         0.5f, -0.5f,	     1.0f, 0.0f, // bottom right
        -0.5f, -0.5f,	     0.0f, 0.0f, // bottom left
        -0.5f,  0.5f,	     0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {0, 1, 3, 1, 2, 3};

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		std::cerr << "OpenGL error: " << err << " "<< std::endl;
        err = glGetError();
    }

}

void Screen::init_window(){
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = glfwCreateWindow(1024, 768, "Chip8 emulator", NULL, NULL);
	if(!m_window){
		glfwTerminate();
		return;
	};

	glfwMakeContextCurrent(m_window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	gladLoadGL();
	glViewport(0, 0, 1024, 768);

	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		std::cerr << "OpenGL error: " << err << " "<< std::endl;
        err = glGetError();
    }
}

Screen::Screen(){
	init_window();
	const char * vertex = "../shaders/vert.glsl";
	const char * frag = "../shaders/frag.glsl";
	m_shader = std::make_shared<Shader>(vertex, frag);
	init();
};

void Screen::init_texture(int w, int h, unsigned char * data){
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		std::cerr << "OpenGL error: " << err << " "<< std::endl;
        err = glGetError();
    }
};

void Screen::update_texture(unsigned char * data){
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 32, GL_RED, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
};

void Screen::render(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	m_shader->use();

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		std::cerr << "OpenGL error: " << err << " "<< std::endl;
        err = glGetError();
    }
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
