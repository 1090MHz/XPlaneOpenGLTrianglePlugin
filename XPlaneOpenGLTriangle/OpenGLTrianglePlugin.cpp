#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include <GL/glew.h>
#include <cstring>

const GLchar *vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 position;
    void main() {
        gl_Position = vec4(position, 1.0f);
    }
)glsl";

const GLchar *fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 color;
    void main() {
        color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)glsl";

GLuint shaderProgram;
GLuint VAO;

static void compileShaders()
{
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Clean up shaders; they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

static void setupTriangle()
{
    // Note: X-Plane uses clockwise winding order
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, // Left
        0.0f, 0.5f, 0.0f,   // Top
        0.5f, -0.5f, 0.0f   // Right
    };

    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static int drawTriangle(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    return 1;
}

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
    strcpy(outName, "OpenGL Triangle Plugin");
    strcpy(outSig, "example.opengl.triangle");
    strcpy(outDesc, "A simple plugin that draws a triangle using OpenGL.");

    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK)
    {
        XPLMDebugString("GLEW initialization failed: ");
        XPLMDebugString((const char *)glewGetErrorString(glewInitResult));
        return 0; // Or handle the error as appropriate
    }
    compileShaders();
    setupTriangle();

    XPLMRegisterDrawCallback(drawTriangle, xplm_Phase_Window, 0, NULL);
    return 1;
}

PLUGIN_API void XPluginStop(void)
{
    XPLMUnregisterDrawCallback(drawTriangle, xplm_Phase_Window, 0, NULL);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &VAO);
}

PLUGIN_API void XPluginDisable(void) {}
PLUGIN_API int XPluginEnable(void) { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void *inParam) {}