#ifndef PA_VERTEX_BUFFER_HPP
#define PA_VERTEX_BUFFER_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

typedef struct SVertex {
	glm::vec3 Position;
	glm::vec2 UV;

	SVertex(glm::vec3 Position, glm::vec2 UV)
	{
		this->Position = Position;
		this->UV = UV;
	}
} SVertex;

class CVertexBuffer {
public:
    uint32_t Id;

    CVertexBuffer(std::vector<SVertex> *Vertices);

	void Bind();
	void Unbind();
	void Delete();
};

#endif