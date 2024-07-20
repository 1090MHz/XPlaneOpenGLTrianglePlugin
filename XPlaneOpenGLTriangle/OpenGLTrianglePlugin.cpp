// Standard OpenGL Extension Wrangler Library
#include <GL/glew.h>

// X-Plane SDK headers for display and menu functionalities
#include <XPLMDataAccess.h> // Access and manipulate data refs and create new data refs for the plugin.
#include <XPLMDisplay.h>    // Create and manage windows, draw in X-Plane's 3D world, and create graphics.
#include <XPLMGraphics.h>   // Draw in the X-Plane 3D world using OpenGL, and manage OpenGL state.
#include <XPLMMenus.h>      // Manage menus in the X-Plane user interface.
#include <XPLMUtilities.h>  // Utility functions for plugins, such as logging messages to the X-Plane log file.

// ImGui integration for X-Plane, used for GUI elements
#include "imgui_impl_xplane.h"

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

    // Check vertex shader compilation status
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        XPLMDebugString("OpenGLTrianglePlugin: ERROR compiling vertex shader:\n");
        XPLMDebugString(infoLog);
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation status
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        XPLMDebugString("OpenGLTrianglePlugin: ERROR compiling fragment shader:\n");
        XPLMDebugString(infoLog);
    }

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check shader program linking status
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        XPLMDebugString("OpenGLTrianglePlugin: ERROR linking shader program:\n");
        XPLMDebugString(infoLog);
    }

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

// Callbacks for dataref
int DataRefReadCallback(void *inRefcon)
{
    return g_triangle_visible ? 1 : 0;
}

void DataRefWriteCallback(void *inRefcon, int inValue)
{
    g_triangle_visible = (inValue != 0);
}

void RegisterTriangleVisibilityDataRef()
{
    XPLMRegisterDataAccessor(
        "XPlaneOpenGLTriangle/triangle_visibility", // inDataName: The name of the data ref to create or modify.
        xplmType_Int,                               // inDataType: The type of data provided by this data ref.
        1,                                          // inIsWritable: 1 if this data ref can be written to, 0 if it is read-only.
        DataRefReadCallback,                        // inReadInt: Read integer value callback
        DataRefWriteCallback,                       // inWriteInt: Write integer value callback
        nullptr,                                    // inReadFloat: Read float value callback
        nullptr,                                    // inWriteFloat: Write float value callback
        nullptr,                                    // inReadDouble: Read double value callback
        nullptr,                                    // inWriteDouble: Write double value callback
        nullptr,                                    // inReadIntArray: Read integer array callback
        nullptr,                                    // inWriteIntArray: Write integer array callback
        nullptr,                                    // inReadFloatArray: Read float array callback
        nullptr,                                    // inWriteFloatArray: Write float array callback
        nullptr,                                    // inReadData: Read raw data callback
        nullptr,                                    // inWriteData: Write raw data callback
        nullptr,                                    // inReadRefcon: Reference constant for the read callback.
        nullptr                                     // inWriteRefcon: Reference constant for the write callback.
    );
}

// An opaque handle to the window we will create
static XPLMWindowID g_window;

// Callbacks we will register when we create our window
void draw_hello_world(XPLMWindowID in_window_id, void *in_refcon);
int dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void *in_refcon) { return 0; }
XPLMCursorStatus dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void *in_refcon) { return xplm_CursorDefault; }
int dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void *in_refcon) { return 0; }
void dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void *in_refcon, int losing_focus) {}

bool SetupWindow()
{
    XPLMCreateWindow_t params{};
    params.structSize = sizeof(params);
    params.visible = 1;
    params.drawWindowFunc = draw_hello_world;
    // Note on "dummy" handlers:
    // Even if we don't want to handle these events, we have to register a "do-nothing" callback for them
    params.handleMouseClickFunc = dummy_mouse_handler;
    params.handleRightClickFunc = dummy_mouse_handler;
    params.handleMouseWheelFunc = dummy_wheel_handler;
    params.handleKeyFunc = dummy_key_handler;
    params.handleCursorFunc = dummy_cursor_status_handler;
    params.refcon = NULL;
    params.layer = xplm_WindowLayerFloatingWindows;
    // Opt-in to styling our window like an X-Plane 11 native window
    // If you're on XPLM300, not XPLM301, swap this enum for the literal value 1.
    params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;

    // Set the window's initial bounds
    // Note that we're not guaranteed that the main monitor's lower left is at (0, 0)...
    // We'll need to query for the global desktop bounds!
    int left, bottom, right, top;
    XPLMGetScreenBoundsGlobal(&left, &top, &right, &bottom);
    params.left = left + 50;
    params.bottom = bottom + 150;
    params.right = params.left + 200;
    params.top = params.bottom + 200;

    g_window = XPLMCreateWindowEx(&params);

    // Position the window as a "free" floating window, which the user can drag around
    XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
    // Limit resizing our window: maintain a minimum width/height of 100 boxels and a max width/height of 300 boxels
    XPLMSetWindowResizingLimits(g_window, 200, 200, 300, 300);
    XPLMSetWindowTitle(g_window, "ImGui Window");

    return g_window != NULL;
}
// Initialization in XPluginStart
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
    strcpy(outName, "OpenGL Triangle Plugin");
    strcpy(outSig, "example.opengl.triangle");
    strcpy(outDesc, "A simple plugin that draws a triangle using OpenGL.");

    createMenu();
    RegisterTriangleVisibilityDataRef();

    glewInit();
    compileShaders();
    setupTriangle();

    // ImGui Implementation for X-Plane initialization
    ImGui::XP::Init();

    XPLMRegisterDrawCallback(drawTriangle, xplm_Phase_Window, 0, NULL);

    SetupWindow();
    return g_window != NULL;
}

// Cleanup in XPluginStop
PLUGIN_API void XPluginStop(void)
{
    XPLMUnregisterDrawCallback(drawTriangle, xplm_Phase_Window, 0, NULL);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &VAO);

    // ImGui Implementation for X-Plane shutdown
    ImGui::XP::Shutdown();
}

PLUGIN_API void XPluginDisable(void) {}
PLUGIN_API int XPluginEnable(void) { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void *inParam) {}

void draw_hello_world(XPLMWindowID in_window_id, void *in_refcon)
{
    // Mandatory: We *must* set the OpenGL state before drawing
    // (we can't make any assumptions about it)
    XPLMSetGraphicsState(
        0 /* no fog */,
        0 /* 0 texture units */,
        0 /* no lighting */,
        0 /* no alpha testing */,
        1 /* do alpha blend */,
        1 /* do depth testing */,
        0 /* no depth writing */
    );

    int l, t, r, b;
    XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);

    float col_white[] = {1.0, 1.0, 1.0}; // red, green, blue

    XPLMDrawString(col_white, l + 10, t - 20, (char *)"Hello world!", NULL, xplmFont_Proportional);
}