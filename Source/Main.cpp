#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "ElementBuffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

void ResizeCallback(GLFWwindow *Window, int32_t FrameBufferWidth, int32_t FrameBufferHeight)
{
    glViewport(0, 0, FrameBufferWidth, FrameBufferHeight);
}

void GeneratePlaneRing(float InnerRadius, float OuterRadius, int Segments, std::vector<SVertex>& Vertices, std::vector<uint32_t>& Indices)
{
    float AngleStep = 2.0f * M_PI / Segments;

    Vertices.clear();
    for (int i = 0; i < Segments; ++i) {
        float Angle = i * AngleStep;
        float XOuter = OuterRadius * cos(Angle);
        float YOuter = OuterRadius * sin(Angle);
        float XInner = InnerRadius * cos(Angle);
        float YInner = InnerRadius * sin(Angle);

        Vertices.push_back(SVertex(glm::vec3(XOuter, YOuter, 0.0f), glm::vec2((XOuter / OuterRadius + 1.0f) / 2.0f, (YOuter / OuterRadius + 1.0f) / 2.0f)));
        Vertices.push_back(SVertex(glm::vec3(XInner, YInner, 0.0f), glm::vec2((XInner / InnerRadius + 1.0f) / 2.0f, (YInner / InnerRadius + 1.0f) / 2.0f)));
    }

    Indices.clear();
    for (int i = 0; i < Segments; ++i) {
        int Next = (i + 1) % Segments;
        
        Indices.push_back(i * 2);
        Indices.push_back(Next * 2);
        Indices.push_back(i * 2 + 1);

        Indices.push_back(Next * 2);
        Indices.push_back(Next * 2 + 1);
        Indices.push_back(i * 2 + 1);
    }
}

int main(void)
{
    if (!glfwInit()) {
        return -1;
    }

    uint32_t WindowWidth = 1920;
    uint32_t WindowHeight = 1080;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::string WindowTitle = "Procedural Animation";
    GLFWwindow *Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), NULL, NULL);
    if (!Window) {
        std::cerr << "Failed to create GLFW Window!\n" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(Window, ResizeCallback);
    glfwMakeContextCurrent(Window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Glew failed to initialize!" << std::endl;
        return -1;
    }

    CShader DefaultShader;
    DefaultShader.Create();
    DefaultShader.Load("Source\\Resource\\Default.vert", EShaderType::VERTEX);
    DefaultShader.Load("Source\\Resource\\Default.frag", EShaderType::FRAGMENT);
    DefaultShader.Activate();

    std::vector<SVertex> Vertices;
    std::vector<uint32_t> Indices;

    int SegmentCount = 200;

    std::vector<std::vector<SVertex>> VerticesArray(SegmentCount);
    std::vector<std::vector<uint32_t>> IndicesArray(SegmentCount);
    std::vector<CVertexArray> VertexArray(SegmentCount);
    std::vector<CVertexBuffer> VertexBuffer;
    std::vector<CElementBuffer> ElementBuffer;

    float Radius = 0.3f;
    float RingSize = Radius * 0.3f;

    for (int i = 0; i < SegmentCount; ++i) {
        GeneratePlaneRing(Radius - RingSize, Radius, 16, VerticesArray[i], IndicesArray[i]);
        VertexArray[i].Bind();
        VertexBuffer.push_back(CVertexBuffer(&VerticesArray[i]));
        ElementBuffer.push_back(CElementBuffer(&IndicesArray[i]));
        VertexArray[i].LinkAttrib(&VertexBuffer[i], 0, 3, GL_FLOAT, sizeof(SVertex), (void*)0);
        VertexArray[i].LinkAttrib(&VertexBuffer[i], 1, 2, GL_FLOAT, sizeof(SVertex), (void*)(3 * sizeof(float)));
        VertexArray[i].Unbind();
    }

    float AspectRatio = static_cast<float>(WindowWidth) / WindowHeight;
    float FOV = glm::radians(120.0f);
    float NearPlane = 0.1f;
    float FarPlane = 100.0f;

    glm::mat4 ProjectionMatrix = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);

    DefaultShader.Activate();
    GLint ProjectionLoc = glGetUniformLocation(DefaultShader.GetId(), "Projection");
    glUniformMatrix4fv(ProjectionLoc, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

    glm::vec3 CameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    float CameraSpeed = 10.0f;

    glm::mat4 ViewMatrix = glm::translate(glm::mat4(1.0f), -CameraPosition);
    GLint ViewLoc = glGetUniformLocation(DefaultShader.GetId(), "View");
    glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(ViewMatrix));

    CTexture SomeTexture = CTexture("Assets/Textures/aaa.png", "Diffuse", 0);
    SomeTexture.TextureUnit(&DefaultShader, "Diffuse", 0);
    SomeTexture.Bind();

    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    std::vector<glm::vec3> WorldPositions(SegmentCount);
    for (int i = 0; i < SegmentCount; ++i) {
        WorldPositions[i] = glm::vec3(static_cast<float>(i), 0.0f, 0.0f);
    }

    std::vector<float> Distances(SegmentCount, 0.3f);

    float AvgFPS = 0.0f;

    auto LastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(Window)) {
        auto CurrentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> ElapsedTime = CurrentTime - LastTime;
        float DeltaTime = ElapsedTime.count();
        LastTime = CurrentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 InputMovement = glm::vec3(0.0f);
        if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            InputMovement.z -= CameraSpeed * DeltaTime;
        }
        if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            InputMovement.z += CameraSpeed * DeltaTime;
        }
        if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS) {
            InputMovement.y += CameraSpeed * DeltaTime;
        } 
        if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS) {
            InputMovement.y -= CameraSpeed * DeltaTime;
        }
        if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS) {
            InputMovement.x -= CameraSpeed * DeltaTime;
        } 
        if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS) {
            InputMovement.x += CameraSpeed * DeltaTime;
        }

        WorldPositions[0] += InputMovement;
        for (int i = 1; i < SegmentCount; ++i) {
            glm::vec3 Direction = glm::normalize(WorldPositions[i - 1] - WorldPositions[i]);
            WorldPositions[i] = WorldPositions[i - 1] - Direction * Distances[i];
        }

        for (int i = 0; i < SegmentCount; ++i) {
            VertexArray[i].Bind();
            GLint WorldPositionLocation = glGetUniformLocation(DefaultShader.GetId(), "WorldPosition");
            glUniform3fv(WorldPositionLocation, 1, glm::value_ptr(WorldPositions[i]));
            glDrawElements(GL_TRIANGLES, IndicesArray[i].size(), GL_UNSIGNED_INT, 0);
            VertexArray[i].Unbind();
        }

        if(AvgFPS == 0.0f) AvgFPS == 1.0f / DeltaTime;

        AvgFPS = (AvgFPS + (1.0f / DeltaTime)) / 2.0f;

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
    
    std::cout << "Average FPS: " << AvgFPS << std::endl;

    glfwDestroyWindow(Window);
    glfwTerminate();
    return 0;
}
