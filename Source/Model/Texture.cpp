#include "Texture.hpp"
#include <iostream>


CTexture::CTexture(const char *Image, const char *TextureType, uint32_t Slot)
{
    Type = TextureType;

	int32_t TextureWidth, TextureHeight, TextureChannels;
	stbi_set_flip_vertically_on_load(true);
	uint8_t *TextureData = stbi_load(Image, &TextureWidth, &TextureHeight, &TextureChannels, 0);

	glGenTextures(1, &Id);
	
	glActiveTexture(GL_TEXTURE0 + Slot);
	Unit = Slot;
	glBindTexture(GL_TEXTURE_2D, Id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	if(TextureChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureWidth, TextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);
    } else if(TextureChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureWidth, TextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData);
    } else if(TextureChannels == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TextureWidth, TextureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, TextureData);
    }
	
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(TextureData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CTexture::TextureUnit(CShader *Shader, const char *Uniform, uint32_t Unit)
{
	Shader->Activate();

	uint32_t TexUnitLocation = glGetUniformLocation(Shader->GetId(), Uniform);
	
	glUniform1i(TexUnitLocation, Unit);	
}

void CTexture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + Unit);
	glBindTexture(GL_TEXTURE_2D, Id);
}

void CTexture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CTexture::Delete()
{
	glDeleteTextures(1, &Id);
}