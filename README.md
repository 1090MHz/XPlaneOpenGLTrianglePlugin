# XPlaneOpenGLTrianglePlugin

A minimal X-Plane plugin demonstrating modern OpenGL rendering with a simple triangle. This project serves as a basic example for developers looking to integrate modern OpenGL graphics into their X-Plane plugins.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

- X-Plane 11 or later
- X-Plane SDK
- OpenGL development environment
- C++ compiler (GCC, Clang, MSVC, etc.)
- GLEW library

### Installing

1. **Clone the repository**

   ```
   git clone https://github.com/1090MHz/XPlaneOpenGLTrianglePlugin.git
   ```

2. **Set up your development environment**

   Ensure that you have the X-Plane SDK located in a place where your project can access it. You also need to have the GLEW library installed and accessible to your project.

3. **Compile the plugin**

   Use your C++ compiler to compile the plugin. Ensure you link against the OpenGL, GLEW, and X-Plane SDK libraries. The exact compilation command will vary depending on your system and setup.

   Example using GCC:

   ```
   g++ -std=c++11 -I/path/to/XPlaneSDK/CHeaders -I/path/to/glew/includes OpenGLTrianglePlugin.cpp -o OpenGLTrianglePlugin.xpl -L/path/to/glew/libs -lGLEW -lGL -lXPLM -lXPWidgets
   ```

4. **Copy the compiled plugin**

   Copy the compiled `.xpl` file into the `Resources/plugins` directory of your X-Plane installation.

5. **Run X-Plane**

   Start X-Plane and load a flight scenario. You should see a simple triangle rendered in the simulator.

## Built With

- **X-Plane SDK** - The SDK used for plugin development in X-Plane
- **GLEW** - The OpenGL Extension Wrangler Library
