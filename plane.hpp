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

    void set_vertices_top_right(float x, float y, float z) {
        vertices2[0] = x ;
        vertices2[1] = y ;
        vertices2[2] = z ; 

        isVertexUpdated = true;
    }

    void set_vertices_bottom_right(float x, float y, float z) {
        vertices2[6] = x ;
        vertices2[7] = y ;
        vertices2[8] = z ; 

        isVertexUpdated = true;
    }

    void set_vertices_bottom_left(float x, float y, float z) {
        vertices2[12] = x ;
        vertices2[13] = y ;
        vertices2[14] = z ; 

        isVertexUpdated = true;
    }

    void set_vertices_top_left(float x, float y, float z) {
        vertices2[18] = x ;
        vertices2[19] = y ;
        vertices2[20] = z ; 

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