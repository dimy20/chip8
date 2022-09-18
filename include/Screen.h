#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

typedef struct pixel_impl_s{
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
}pixel_impl_t;

class Screen{
	public:
		Screen();
		void draw_pixel(int x, int y);
		operator bool();
		void close();
		void clear();
		void swap_buffers();
		GLFWwindow * window() const { return m_window; };
	private:
		GLFWwindow * m_window;
		std::shared_ptr<Shader> m_shader;
		pixel_impl_t m_pixel; // pixel implementation
};
