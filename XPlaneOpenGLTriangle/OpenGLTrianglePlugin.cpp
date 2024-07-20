#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include <GL/glew.h>
#include <cstring>

#include "imgui.h"
#include "imgui_impl_opengl3.h"

//#include <GLFW/glfw3.h>
// #include "imgui_impl_xplane.h" // This is hypothetical; you'll need to adapt ImGui to work with X-Plane.

// ImGui X-Plane integration initialization
void ImGui_ImplXPlane_Init()
{
    // Initialize ImGui for X-Plane, possibly setting up an OpenGL context or adapting ImGui to use X-Plane's context
}

// Prepare ImGui for a new frame in the X-Plane environment
void ImGui_ImplXPlane_NewFrame()
{
    // Adapt ImGui to the current display size and DPI settings of X-Plane
    // This might involve querying X-Plane for the current window size and passing it to ImGui
    int windowWidth, windowHeight;
    XPLMGetScreenSize(&windowWidth, &windowHeight);
    ImGui::GetIO().DisplaySize = ImVec2((float)windowWidth, (float)windowHeight);
    
    // Start a new ImGui frame after adapting to X-Plane's environment
    // ImGui::NewFrame();
}

// Clean up ImGui resources specific to the X-Plane integration
void ImGui_ImplXPlane_Shutdown()
{
    // Clean up any resources allocated by ImGui that are specific to X-Plane
}

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
         0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

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

// Draw ImGui window in a new function
static void drawImGuiWindow()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplXPlane_NewFrame(); // This is hypothetical; adapt ImGui frame preparation as necessary.

    // At the beginning of a new frame
    ImGui::NewFrame();
    ImGui::Begin("Triangle Control");
    ImGui::Text("This is a simple ImGui window in an X-Plane plugin.");
    ImGui::End();
    // End ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // If using ImGui's docking features, this is necessary
    //if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    //    ImGui::UpdatePlatformWindows();
    //    ImGui::RenderPlatformWindowsDefault();
    //    glfwMakeContextCurrent(backup_current_context);
    //}

    // Call EndFrame() if you're not immediately starting a new frame afterwards
    ImGui::EndFrame();
}

// Modify drawTriangle to include ImGui window drawing
static int drawTriangle(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    return 1;
}

// Initialization in XPluginStart
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
    strcpy(outName, "OpenGL Triangle Plugin");
    strcpy(outSig, "example.opengl.triangle");
    strcpy(outDesc, "A simple plugin that draws a triangle using OpenGL.");

    glewInit();
    compileShaders();
    setupTriangle();

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 330");
    // ImGui_ImplXPlane_Init(); // This is hypothetical; adapt ImGui initialization as necessary.

    XPLMRegisterDrawCallback(drawTriangle, xplm_Phase_Window, 0, NULL);
    return 1;
}

// Cleanup in XPluginStop
PLUGIN_API void XPluginStop(void)
{
    XPLMUnregisterDrawCallback(drawTriangle, xplm_Phase_Window, 0, NULL);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &VAO);

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplXPlane_Shutdown(); // This is hypothetical; adapt ImGui cleanup as necessary.
    ImGui::DestroyContext();
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int XPluginEnable(void) { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void* inParam) { }