#ifndef PA_ELEMENT_BUFFER_HPP
#define PA_ELEMENT_BUFFER_HPP

#include <GL/glew.h>
#include <vector>

class CElementBuffer {
public:
    uint32_t Id;

    CElementBuffer(std::vector<uint32_t> *Indices);

    void Bind();
    void Unbind();
    void Delete();
};

#endif