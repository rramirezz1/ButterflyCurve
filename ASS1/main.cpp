//
//  main.cpp
//  sinc
//
//  Modificado por Tiago Ramos em 23/10/2023.
// 
//

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#define _USE_MATH_DEFINES // Faz com que as variaveis matematicas, como o M_PI, estejam disponiveis nas operações
#include <cmath>          // Fornece várias funções e constantes matematicas, para serem usadas no programa

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// GLM header file
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;


// shaders header file
#include "common/shader.hpp"

// Vertex array object (VAO)
GLuint VertexArrayID;

// Vertex buffer object (VBO)
GLuint vertexbuffer;

// color buffer object (CBO)
GLuint colorbuffer;

// GLSL program from the shaders
GLuint programID;


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 900


const int numPoints = 1100; // Controla a quantidade de pontos desenhados no ecrã

//--------------------------------------------------------------------------------
void transferDataToGPUMemory(void)
{
    GLfloat x = -20.0f; // xmin of the domain
    
    // VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");


    const float escala = 0.3f;
    GLfloat g_vertex_buffer_data[numPoints * 3]; // Declara um array de números, com espaço para armazenar três valores (coordenadas x, y e z) para cada um dos pontos
    for (int i = 0; i < numPoints; i++) // Este loop é usado para calcular os valores para cada ponto da borboleta
    {
        float t = i * (12 * M_PI) / 1000.0; //Changing the last value will affect the polygons
        g_vertex_buffer_data[i * 3] =escala * sin(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t / 12), 5)); // Calcula a coordenada x do ponto i. O resultado é armazenado na primeira posição do array para o ponto i
        g_vertex_buffer_data[(i * 3) + 1] = escala * cos(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t / 12), 5)); //Calcula a coordenada y do ponto i. O resultado é armazenado na segunda posição do array para o ponto i
        g_vertex_buffer_data[(i * 3) + 2] = 0; //  Define a coordenada z do ponto i como 0, isto pois é um plano 2D
    }


    GLfloat g_color_buffer_data[numPoints * 3]; // Este array tem espaço para armazenar três valores de cor (canais vermelho, verde e azul) para cada um dos pontos
    for (int i = 0; i < numPoints; i++) //  Este loop será usado para definir as cores de cada ponto      
    { 

        g_color_buffer_data[i * 3] = 0.0f;
        g_color_buffer_data[(i * 3) + 1] = 0.0f;
        g_color_buffer_data[(i * 3) + 2] = 1.0f;
    }


    // Move vertex data to video memory; specifically to VBO called vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Move color data to video memory; specifically to CBO called colorbuffer
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

}


//--------------------------------------------------------------------------------
void cleanupDataFromGPU()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
}


//--------------------------------------------------------------------------------
void draw(void)
{

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    // create domain in R^2
    //Change the last two values to be able to see the butterfly
    glm::mat4 mvp = glm::ortho(-8.0f, 8.0f, -4.0f, 4.0f); 

    // retrieve the matrix uniform locations
    unsigned int matrix = glGetUniformLocation(programID, "mvp");
    glUniformMatrix4fv(matrix, 1, GL_FALSE, &mvp[0][0]);



    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );


    glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointSize(10);

    // The numPoints is a variable that controls the amount of
    // pixels that are drawn
    glDrawArrays(GL_LINE_STRIP, 0, numPoints); 
    //glDrawArrays(GL_POINTS, 0, 80); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
//--------------------------------------------------------------------------------


int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Butterfly", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


    // transfer my data (vertices, colors, and shaders) to GPU side
    transferDataToGPUMemory();


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {

        draw();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup VAO, VBOs, and shaders from GPU
    cleanupDataFromGPU();

    glfwTerminate();

    return 0;
}