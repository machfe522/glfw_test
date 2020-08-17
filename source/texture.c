 

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define  STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    { -0.6f,  0.4f, 0.f,  1.f, 0.6f },
    { -0.6f, -0.4f, 0.f,  0.f, 0.6f },
    
    { 0.2f,  0.4f,  0.666f,  1.f, 0.6f },
    { 0.2f, -0.4f,  0.666f,  0.f, 0.6f },
    
    { 0.6f,  0.4f,  1.f,  1.f, 0.6f },
    { 0.6f, -0.4f,  1.f,  0.f, 0.6f },
};
 
static const unsigned int indices[] = {
    0,1,2,
    1, 3,2,
    2,3,4,
    3, 5,4,
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
"#version 110                                             \n"
"varying vec2 normal;                                     \n"
"varying float width;                                     \n"
"                                                         \n"
" uniform sampler2D u_image;                              \n"
"void main()                                              \n"
"{                                                        \n"
"                                                         \n"
"                                                         \n"
"                                                         \n"
"                                                         \n"
"    gl_FragColor =  texture2D(u_image, normal);          \n"
"    //gl_FragColor = vec4(1.0, 0.8, 0.3, 1.0);             \n"
"}                                                        \n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void printShaderLog(GLuint shader)
{
    GLint shaderState;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderState);
    if(shaderState == GL_TRUE)
    {
        return;
    }
    GLsizei bufferSize = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufferSize);
    if(bufferSize > 0)
    {
        char* buffer = (char*)malloc(bufferSize);
        glGetShaderInfoLog(shader, bufferSize, NULL, buffer);
        printf("%s", buffer);
        free(buffer);
    }
}

static void printLinkerLog(GLuint program)
{
    GLint linkState;
    glGetProgramiv(program, GL_LINK_STATUS, &linkState);
    if(linkState == GL_TRUE)
    {
        return;
    }
    
    GLsizei bufferSize = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufferSize);
    if(bufferSize > 0)
    {
        char* buffer = (char*)malloc(bufferSize);
        glGetProgramInfoLog(program, bufferSize, NULL, buffer);
        printf("%s", buffer);
        free(buffer);
    }
}
 

static unsigned int createTexture(const char* filename){
     
    int width, height, nrChannels;
    
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data)
    {
        unsigned int texture_id;
        glGenTextures(1, &texture_id);

        //激活
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        
        //为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(data);
        
        return texture_id;
    }
    else
    {
        printf("load texture failed.\n");
        return 0;
    }
}
 
int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, tex_location, vpos_location, vcol_location;

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
    printShaderLog(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    printShaderLog(fragment_shader);
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    printLinkerLog(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    tex_location = glGetUniformLocation(program, "u_image");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));

    unsigned int  textureId =  createTexture("/Users/machfe/Documents/08_mapbox/glfw_3.3.2/source/data/724568.png");
    unsigned int  arrowId =  createTexture("/Users/machfe/Documents/08_mapbox/glfw_3.3.2/source/data/arrow.png");
    
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
       
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, arrowId);
            
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1i(tex_location, 1); 
        
        glDrawElements(GL_TRIANGLES,  sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, indices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
