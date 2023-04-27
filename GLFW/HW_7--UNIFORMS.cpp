/*

There are two ways to pass data from CPU to GPU 
----> via VERTEX Buffer
----> or via U

UNIFORMS are variables that can be passed to shader program as parameter    like color in fragment shader 

*/


#include <GL\glew.h>

#include <GLFW/glfw3.h>


#include <iostream>
#include <fstream>  // to read file
#include <string>   // To use getline func
#include <sstream>


/* ------------ MACRO ------------ */
#define ASSERT(x) if (!(x)) __debugbreak(); // MSVC (microsoft compiler) specific command to stop executing the program

#define GLCall(x) GLCLearError();\
    x;\
    ASSERT(GlLogCall(#x, __FILE__, __LINE__))    

/* 
# before a fucntion pass it as a stringand __FILE__and __LINE__ are macros to get file loc and line of fucntion
and since MACROS HAS TO BE IN SAME LINE we use \ before line ends to write macros in multiple lines
*/


static void GLCLearError() {
    while (glGetError() != GL_NO_ERROR);    // glGetError get error and we get all the error until there are no arror left
}

static bool GlLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGl Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}


struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath);

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

static unsigned int CompileShader(unsigned int shaderType, const std::string& source);


int main(void)
{
    /* GLFW BASIC STUFF */
        GLFWwindow* window;

        /* Initialize the GLFW library */
        if (!glfwInit())
            return -1;



        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;

        }
        /* Make the window's context current */
        glfwMakeContextCurrent(window);
    /*  END BASIC GLFW   */


    /* Intitialize GLEW */
        if (glewInit() != GLEW_OK) {
            std::cout << "Error!" << std::endl;
        }
    /* END */

    std::cout << glGetString(GL_VERSION) << std::endl;  /* prints the version of opengl using */



    /* ------------- Generating Data to be used to display in the window ------------- */
        
        /* ------------- Vertex Info (like position, color, texture, smoothness, normal,etc) ------------- */
            
            // 4 positions of vertices of sqaure
            float positions[] = {
                -0.5f, -0.5f,    // 0
                 0.5f, -0.5f,    // 1
                 0.5f,  0.5f,    // 2
                -0.5f,  0.5f     // 3
            };

            // Index data -----> position in which vertices is to be rendered to form a square
            unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
            };
        



        /* ------------- BUFFER DATA ------------- */
            
            unsigned int buffer;
            GLCall(glGenBuffers(1, &buffer));
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
            GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW));
        


        /* ------------- VERTEX_Attribute ------------- */
            
            GLCall(glEnableVertexAttribArray(0));
            GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));


        /* ------------- INDEX BUFFER ------------- */
        
            unsigned int ibo;       // index buffer object
            GLCall(glGenBuffers(1, &ibo));
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
            GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));   
            /* Here we can use Gluint but it is equal to unsigned int and value of unsigned int may not be same for all platform 
            but using Gluint is not good if using multiple API like Directx, Vulkan , Metal, etc */ 



        /* ------------- SHADERS ------------- */
            
            ShaderProgramSource shaderSource = ParseShader("res/shaders/Basic.shader");
            unsigned int shader = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
            GLCall(glUseProgram(shader));

        
        /* ------------- Setting UNIFORMS ------------- */

            GLCall(int location = glGetUniformLocation(shader, "u_Color"));
            ASSERT(location != -1);
            GLCall(glUniform4f(location, 0.8f, 0.3f, 0.2f, 1.0f));


    /* ------------- END OF GENERATING DATA ------------- */



    /* WHILE Loop to keep the window active till window is closed */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glClear(GL_COLOR_BUFFER_BIT);


        
        /* for this glDrawElements is used instead of glDrawArrays */
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); 

        


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}




/* Makes and compile the shader by inputing the type and Source code */
static unsigned int CompileShader(unsigned int shaderType, const std::string& source) {

    unsigned int id = glCreateShader(shaderType);   /* generate Shader and return id */
    const char* src = source.c_str();               /* convert inputed string to char* */
    GLCall(glShaderSource(id, 1, &src, nullptr));           /* attaching source code to the shader */
    GLCall(glCompileShader(id));                            /* compile shader */

    /* -------- ERROR HANDLING -------- */
        int result;
        GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE) {
            int length;
            GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
            char* message = (char*) _malloca(length * sizeof(char));      /* just means char Array of length 'length'  or    char message[length] */
            GLCall(glGetShaderInfoLog(id, length, &length, message));

            std::cout << "Failed To Compile " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;

            std::cout << message << std::endl;
            GLCall(glDeleteShader(id));
            return 0;
        }

    /* -------- END ERROR HANDLING -------- */

    return id;

}


/* Creates a program Which contain vertex and fragment shader */
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

    GLCall(unsigned int program = glCreateProgram());                           /* generate program to store all shader and program to be run by GPU during while loop */
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));                                        /* Attach shader to program to be run by GPU */
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));
    GLCall(glDeleteShader(vs));                                                 /* DELETING shader to save space as shader is already attached to program */
    GLCall(glDeleteShader(fs));

    return program;

}


/* Read file and output vertex and fragment shader source code */
static ShaderProgramSource ParseShader(const std::string& filepath) {

    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {

        if (line.find("#shader") != std::string::npos) {
            
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
                
        }
        else {
            ss[int(type)] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}




