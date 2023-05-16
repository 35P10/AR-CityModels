#pragma once

class Plane_texture {
public:
    Plane_texture() {
        shader();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    void render(cv::Mat frame_output) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glActiveTexture(GL_TEXTURE0);
        
        //glMatrixMode(GL_MODELVIEW);     
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
    
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_output.cols, frame_output.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame_output.data);
     
        glGenerateMipmap(GL_TEXTURE_2D);

        glUseProgram(shaderProgram);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glDeleteTextures(1, &texture);
    }
private:
    unsigned int texture;
    unsigned int VBO, VAO, EBO;
    float vertices2[32] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices2[6] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int    shaderProgram = glCreateProgram();
    
    void shader() {    
        const char* vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos; \n"
            "layout (location = 1) in vec3 aColor; \n"
            "layout (location = 2) in vec2 aTexCoord; \n"
            "out vec3 ourColor;\n"
            "out vec2 TexCoord;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = vec4(aPos, 1.0);\n"
            "    ourColor = aColor;\n"
            "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
            "}\n";
        const char* fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec3 ourColor;\n"
            "in vec2 TexCoord;\n"
            "uniform sampler2D texture1;\n"
            "void main()\n"
            "{\n"
            "   FragColor = texture(texture1, TexCoord);\n"
            "}\n\0";
        
        
        //===============  vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        //===============   fragment Shader
        // create  fragmentShader;
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // link shaders
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
    }
};