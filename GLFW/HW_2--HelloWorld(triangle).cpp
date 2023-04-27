#include <iostream>


#include <GL\glew.h>

#include <GLFW/glfw3.h>



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
            
            float positions[6] = {
                   -0.5f, -0.5f,
                    0.0f,  0.5f,
                    0.5f, -0.5f
            };

        /* ------------- END VERTEX INFO ------------- */


        /* ------------- BUFFER DATA ------------- */
            
            unsigned int buffer;
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
        
        /* ------------- END BUFFER DATA ------------- */


        /* ------------- VERTEX_Attribute ------------- */
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

        /* ------------- END VERTEX_ATTRIB ------------- */

        
        /* ------------- SHADERS ------------- */
            

            /* ------------- VERTEX SHADER ------------- */
                
                std::string vertexShader =
                    "#version 330 core\n"
                    "\n"
                    "layout(location = 0) in vec4 position;\n"
                    "out gl_PerVertex { vec4 gl_Position; };\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "   gl_Position = position;\n"
                    "}\n";


            /* ------------- FRAGMENT SHADER ------------- */
                
                std::string fragmentShader =
                    "#version 330 core\n"
                    "\n"
                    "layout(location = 0) out vec4 color;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                    "}\n";



            unsigned int shader = CreateShader(vertexShader, fragmentShader);
            glUseProgram(shader);

        /* ------------- END SHADERS ------------- */


    /* ------------- END OF GENERATING DATA ------------- */



    /* WHILE Loop to keep the window active till window is closed */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glClear(GL_COLOR_BUFFER_BIT);   /* clear the screen to remove object render in previous frame */


        /* ------------- This is where Traingle is Displayed to window ------------- */
            
            glDrawArrays(GL_TRIANGLES, 0, 3);

        /* ------------- End ------------- */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events (like pressing key aur using mouse */
        glfwPollEvents();
    }

    glfwTerminate();    /* terminates window and any other glfw function (like bufferes and shaders*/
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


/* Craetes a program Which contain vertex and fragment shader */
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






