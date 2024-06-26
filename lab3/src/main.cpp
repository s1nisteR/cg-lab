#include <iostream>

#include <glad/glad.h>  //glad should always be included before glfw
#include <GLFW/glfw3.h>



//framebuffer size callback to handle window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

int main() 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  //needed for macos
    
    //create a window
    GLFWwindow* window = glfwCreateWindow(800, 400, "CSE420-Lab3", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create the glfw window" << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(window);

    //set callback to handle window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //initialize glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize glad" << std::endl;
        return 1;
    }

    //data to use during our rendering
        //coordinates(3)       //colors(3)
    float vertices[] = 
    {
        0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f ,      //top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,       //bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,       //bottom left
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 0.0f,       //top left

        0.8f, 0.8f, 0.0f,   0.0f, 0.0f, 1.0f,      //top right
        0.8f, -0.1f, 0.0f,  0.0f, 0.0f, 1.0f,       //bottom right
        0.0f, -0.1f, 0.0f,  0.0f, 0.0f, 1.0f,       //bottom left
        0.0f, 0.8f, 0.0f,   0.0f, 0.0f, 1.0f        //top left
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3,  //second triangle
        4, 5, 7,
        5, 6, 7
    };

    //our vertex buffer object
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //bind the vertex array, this vertex array will store all configurations on our VBOs, etc
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //then bind our VBO
    
    //now copy vertices to GPU memory using the VBO, because ARRAY_BUFFER is bound to our VBO from  now
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //bind our EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);    //send over our indices data

    //set vertex attrib pointer to describe our vertex data and how to process it
    //for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //for colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);   //unbind VBO
    glBindVertexArray(0);               //unbind VAO for later use

    //create the vertex shader from source code
    const char* vertexShaderSource = "#version 330 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aColor;\nout vec3 ourColor;\n void main()\n{\n gl_Position = vec4(aPos, 1.0);\n ourColor = aColor;\n}";
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //now connect the shader source code and program
    glCompileShader(vertexShader);  //now compile our vertex shader

    //check for errors during compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }


    //now create the fragment shader from source code
    const char* fragmentShaderSource = "#version 330 core\nout vec4 FragColor;\nin vec3 ourColor;\nvoid main()\n{\nFragColor = vec4(ourColor, 1.0f);\n}";
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); //connect fragment shader source
    glCompileShader(fragmentShader);    //compile our fragment shader

    //check for errors again
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    //now create a shader program that encompasses both these shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    //now attach our created shaders to this program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);   //now link all shaders of this program

    //check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    

    //get rid of shaders since compilation is done and it is already inside the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    //render loop
    while(!glfwWindowShouldClose(window))
    {
        //=====rendering code======
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //note: VAO will have the EBO
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUseProgram(shaderProgram);    //use our shader program
        glBindVertexArray(VAO);         //bind our VAO to bring in all vertex attribute settings, VBOs, etc
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
        //pass into glDrawElements the number of indices, for that many items to draw
        glBindVertexArray(0);           //unbind VAO

        //=========================
        glfwSwapBuffers(window);    //swap buffers for double buffering
        glfwPollEvents();           //poll for events
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
