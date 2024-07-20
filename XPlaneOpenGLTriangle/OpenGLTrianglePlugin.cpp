#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include <GL/glew.h>
#include <cstring>

// #include <GLFW/glfw3.h>
//  #include "imgui_impl_xplane.h" // This is hypothetical; you'll need to adapt ImGui to work with X-Plane.

// Global variables
static XPLMMenuID g_menu_id = nullptr;
static int g_menu_item_index = -1;
static bool g_triangle_visible = true; // Triangle visibility state

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
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f};

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

// Draw a triangle using OpenGL
static int drawTriangle(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    if (g_triangle_visible)
    {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
    return 1;
}

// Menu handler callback
void menuHandler(void *inMenuRef, void *inItemRef)
{
    if (inItemRef == (void *)0)
    {                                             // Assuming 0 is the ref for the toggle item
        g_triangle_visible = !g_triangle_visible; // Toggle visibility
    }
}

// Function to create menu
void createMenu()
{
    int myPluginItem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "OpenGL Triangle", 0, 0);
    g_menu_id = XPLMCreateMenu("OpenGL Triangle", XPLMFindPluginsMenu(), myPluginItem, menuHandler, nullptr);
    g_menu_item_index = XPLMAppendMenuItem(g_menu_id, "Toggle Triangle", (void *)0, 1);
}

// Initialization in XPluginStart
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
    strcpy(outName, "OpenGL Triangle Plugin");
    strcpy(outSig, "example.opengl.triangle");
    strcpy(outDesc, "A simple plugin that draws a triangle using OpenGL.");

    createMenu();

    glewInit();
    compileShaders();
    setupTriangle();

    // ImGui Implementation for X-Plane initialization
    ImGui_ImplXPlane_Init();

    XPLMRegisterDrawCallback(drawTriangle, xplm_Phase_Window, 0, NULL);

    return 1;
}

// Cleanup in XPluginStop
PLUGIN_API void XPluginStop(void)
{
    XPLMUnregisterDrawCallback(drawTriangle, xplm_Phase_Window, 0, NULL);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &VAO);

    // ImGui Implementation for X-Plane shutdown
    ImGui_ImplXPlane_Shutdown();
}

PLUGIN_API void XPluginDisable(void) {}
PLUGIN_API int XPluginEnable(void) { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void *inParam) {}