#ifndef PLANE_H
#define PLANE_H

#include "shader.hpp"

class Plane {
public:
    Plane() {
        update_vertices();
    }

    void update_vertices() {
        if(!isVertexUpdated) return;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        isVertexUpdated = false;
    }

    void render(Shader shader) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //nota: Reducir mas adelante a una funcion!!

    void set_vertices_top_right(glm::mat4 view) {
        vertices2[0] = view[3][0];
        vertices2[1] = view[3][1];
        vertices2[2] = view[3][2];

        isVertexUpdated = true;
    }

    void set_vertices_bottom_right(glm::mat4 view) {
        vertices2[6] = view[3][0];
        vertices2[7] = view[3][1];
        vertices2[8] = view[3][2]; 

        isVertexUpdated = true;
    }

    void set_vertices_bottom_left(glm::mat4 view) {
        vertices2[12] = view[3][0];
        vertices2[13] = view[3][1];
        vertices2[14] = view[3][2]; 

        isVertexUpdated = true;
    }

    void set_vertices_top_left(glm::mat4 view) {
        vertices2[18] = view[3][0];
        vertices2[19] = view[3][1];
        vertices2[20] = view[3][2];

        isVertexUpdated = true;
    }


private:
    unsigned int VBO, VAO, EBO;
    float vertices2[24] = {
        // positions          // colors
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f, // top left 
    };
    bool isVertexUpdated = true;
    unsigned int indices2[6] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
};

#endif