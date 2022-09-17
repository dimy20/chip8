#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader{
	// program id
	unsigned int m_id;

	public:
		Shader(const std::string& vertex_path, const std::string& fragment_path);
		// use/activate the shader
		void use();

		// utility uniform functions
		void setBool(const std::string &name, bool value) const;  
		void setInt(const std::string &name, int value) const;   
		void setFloat(const std::string &name, float value) const;
		void setMat4(const std::string& name, glm::mat4& value) const;
		unsigned int id() const { return m_id;  };
};
