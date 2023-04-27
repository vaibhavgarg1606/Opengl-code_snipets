#include <GL\glew.h>

#include <GLFW/glfw3.h>


#include <iostream>
#include <fstream>  // to read file
#include <string>   // To use getline func
#include <sstream>


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
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);
        


        /* ------------- VERTEX_Attribute ------------- */
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);


        /* ------------- INDEX BUFFER ------------- */
        
            unsigned int ibo;       // index buffer object
            glGenBuffers(1, &ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
            /* Here we can use Gluint but it is equal to unsigned int and value of unsigned int may not be same for all platform 
            but using Gluint is not good if using multiple API like Directx, Vulkan , Metal, etc */ 




        /* ------------- SHADERS ------------- */
            
            ShaderProgramSource shaderSource = ParseShader("res/shaders/Basic.shader");
            unsigned int shader = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
            glUseProgram(shader);



    /* ------------- END OF GENERATING DATA ------------- */



    /* WHILE Loop to keep the window active till window is closed */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glClear(GL_COLOR_BUFFER_BIT);


        
        /* for this glDrawElements is used instead of glDrawArrays */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);   // 6 is no of indices and in 4th option we have to put address of ibo but we already bind it 



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
    glShaderSource(id, 1, &src, nullptr);           /* attaching source code to the shader */
    glCompileShader(id);                            /* compile shader */

    /* -------- ERROR HANDLING -------- */
        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*) _malloca(length * sizeof(char));      /* just means char Array of length 'length'  or    char message[length] */
            glGetShaderInfoLog(id, length, &length, message);

            std::cout << "Failed To Compile " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;

            std::cout << message << std::endl;
            glDeleteShader(id);
            return 0;
        }

    /* -------- END ERROR HANDLING -------- */

    return id;

}


/* Creates a program Which contain vertex and fragment shader */
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

    unsigned int program = glCreateProgram();                           /* generate program to store all shader and program to be run by GPU during while loop */
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);                                        /* Attach shader to program to be run by GPU */
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);                                                 /* DELETING shader to save space as shader is already attached to program */
    glDeleteShader(fs);

    return program;

}


/* Read file  */
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




