#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb/stb_image.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "../vendor/glad/include/glad/glad.h"  //glad should always be included before glfw
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
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
    GLFWwindow* window = glfwCreateWindow(800, 400, "CSE420-Lab5", NULL, NULL);
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
        //coordinates(3)       //colors(3)      //tex coords(2)
    float vertices[] = 
    {
        0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, //top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   2.0f, 0.0f, //bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, //bottom left
        -0.5f, 0.5f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 2.0f  //top left
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3,  //second triangle
    };

    stbi_set_flip_vertically_on_load(true); //we tell stbi to flip the image on the y axis because the image axes and OpenGL axes are different on where 1.0 is

    //loading texture using stb_image
    int texWidth, texHeight, nrChannels;
    unsigned char *texData = stbi_load("../src/texture/stone-tex.jpg", &texWidth, &texHeight, &nrChannels, 0);

    int anotherWidth, anotherHeight, anothernrChannels;
    unsigned char *anotherTex = stbi_load("../src/texture/awesomeface.png", &anotherWidth, &anotherHeight, &anothernrChannels, 0);
 

    //generating texture
    unsigned int texture;
    unsigned int anotherTexture;
    glGenTextures(1, &texture);
    glGenTextures(1, &anotherTexture);

    //bind to our texture and set a few parameters
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //generate bound texture from stbi loaded image and free the stbi loaded image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(texData);

    //another texture
    glBindTexture(GL_TEXTURE_2D, anotherTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, anotherWidth, anotherHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, anotherTex);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(anotherTex);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //for colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //for tex coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);   //unbind VBO
    glBindVertexArray(0);               //unbind VAO for later use


    //create the vertex shader from source code file
    std::ifstream vertexFile;
    // ensure ifstream objects can throw exceptions:
    vertexFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    vertexFile.open("../src/vertex.shader");
    std::string vsString((std::istreambuf_iterator<char>(vertexFile)), std::istreambuf_iterator<char>());
    vertexFile.close();
    const char* vertexShaderSource = vsString.c_str();
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
        std::cout << "Error during vertex shader compilation: " << std::endl;
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }


    //now create the fragment shader from source code file
    std::ifstream fragmentFile;
    fragmentFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragmentFile.open("../src/fragment.shader");
    std::string fsString((std::istreambuf_iterator<char>(fragmentFile)), std::istreambuf_iterator<char>());
    fragmentFile.close();
    const char* fragmentShaderSource = fsString.c_str();
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); //connect fragment shader source
    glCompileShader(fragmentShader);    //compile our fragment shader

    //check for errors again
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        std::cout << "Error during fragment shader compilation: " << std::endl;
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
        std::cout << "Error during linking: " << std::endl;
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    




    //get rid of shaders since compilation is done and it is already inside the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);    //use our shader program
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);  //set ourTexture and the first uniform
    glUniform1i(glGetUniformLocation(shaderProgram, "anotherTexture"), 1);  //set anotherTexture as second uniform

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");   //get location of our transform uniform

    //render loop
    while(!glfwWindowShouldClose(window))
    {
        //=====rendering code======
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //note: VAO will have the EBO
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        //translate first and then rotate.
        //why does it look like it's circling around a point?
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));           //send data to this uniform

        glActiveTexture(GL_TEXTURE0);   //active texture unit 0
        glBindTexture(GL_TEXTURE_2D, texture);  //bind our texture
        glActiveTexture(GL_TEXTURE1);   //activate texture unit 1
        glBindTexture(GL_TEXTURE_2D, anotherTexture);   //bind anotherTexture to second unit
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
