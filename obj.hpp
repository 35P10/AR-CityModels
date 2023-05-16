#pragma once

class Cube {
public:

    Cube() {
        shader();
    }
    Cube(float new_vertices[40], unsigned int new_indices[36]) {
        shader();
        setVertices(new_vertices, new_indices);
    }
    Cube(float new_vertices[40], unsigned int new_indices[36], glm::vec4 new_color) {
        shader();
        setVertices(new_vertices, new_indices);
        setColor(new_color);
    }
    void setColor(glm::vec4 new_color) {
        color = new_color;
    }
    void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUseProgram(shaderProgram);

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, color[0], color[1], color[2], color[3]);
        // render containers
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void setVertices(float new_vertices[40], unsigned int new_indices[36]) {
        for (int i = 0; i < 40; i++)
            vertices[i] = new_vertices[i];
        for (int i = 0; i < 36; i++)
            indices[i] = new_indices[i];

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    ~Cube() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
private:
    glm::vec4       color = glm::vec4(0.1f, 0.5f, 0.0f, 1.0f);
    unsigned int    VBO, VAO, EBO;
    unsigned int    shaderProgram = glCreateProgram();
    float           vertices[40];
    unsigned int    indices[36];

    bool shader() {
        const char* vertexShaderSource = "#version 330 core\n"
            "layout(location = 0) in vec3 aPos; \n"
            "uniform mat4 model; \n"
            "uniform mat4 view; \n"
            "uniform mat4 projection; \n"
            "void main()\n"
            "{\n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
            "}\n";
        const char* fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec4 ourColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = ourColor;\n"
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
        return true;
    }
};