 

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stdio.h>

struct  Vextex
{
    float x, y;
    float ex, ey;
    float wd;
};

static const struct Vextex vertices[] =
{
    { -0.6f,  0.4f, 1.f,  1.f, 0.6f },
    { -0.6f, -0.4f, 1.f, -1.f, 0.6f },
    
    { 0.2f,  0.4f, -1.f,  1.f, 0.6f },
    { 0.2f, -0.4f, -1.f, -1.f, 0.6f },
    
    
};
 
static const unsigned int indices[] = {
    0,1,2,
    1, 3,2,
};

static const char* vertex_shader_text =
"#version 110                                  \n"
"uniform mat4 MVP;                             \n"
"attribute vec3 vCol;                          \n"
"attribute vec2 vPos;                          \n"
"                                              \n"
"varying vec2 normal;                          \n"
"varying float width;                          \n"
"void main()                                   \n"
"{                                             \n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0); \n"
"    normal = vCol.xy;                         \n"
"    width  = vCol.z;                          \n"
"}                                             \n";

static const char* fragment_shader_text =
"#version 110                                   \n"
"varying vec2 normal;                           \n"
"varying float width;                           \n"
"void main()                                    \n"
"{                                              \n"
"    float blur = 0.01;                         \n"
"    float dist = length(normal) * width;       \n"
"    float blur2 = (width - dist) / blur;       \n"
"    float alpha = clamp(blur2, 0.0, 1.0);      \n"
//"    if(alpha<1.0)                            \n"
//"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
//"    else                                     \n"
"    gl_FragColor = vec4(1.0, 0.8, 0.3, alpha); \n"
"}                                              \n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity
    // 启用混合
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
    
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4_identity(m);
        //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawElements(GL_TRIANGLES,  6, GL_UNSIGNED_INT, indices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
