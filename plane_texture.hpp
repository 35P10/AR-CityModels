#ifndef CVPLANTETEXTURE_H
#define CVPLANTETEXTURE_H

#include "shader.hpp"

class Plane_texture {
public:
    Plane_texture() {
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
    void render(Shader shader, cv::Mat frame_output) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //glActiveTexture(GL_TEXTURE0);
        
        //glMatrixMode(GL_MODELVIEW);     
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
    
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_output.cols, frame_output.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame_output.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.use();
        
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
};

#endif