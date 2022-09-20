#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class Screen{
	public:
		Screen();
		void render();
		operator bool();
		void close();
		void clear();
		void swap_buffers();
		GLFWwindow * window() const { return m_window; };
		void init_texture(int w, int h, unsigned char * data);
		void update_texture(unsigned char * data);

	private:
		void init();
		void init_window();

	private:
		GLFWwindow * m_window;
		std::shared_ptr<Shader> m_shader;
		unsigned m_texture;
		unsigned int m_vao;
		unsigned int m_vbo;
		unsigned int m_ebo;
};
