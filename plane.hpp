#ifndef PLANE_H
#define PLANE_H

#include "shader.hpp"
#include "model.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>

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

    void render_models(Shader shader) {
        shader.use();
        for(auto model:Models) {
           model.render(shader); 
        }
    }

    void add_model(Model Copy, glm::mat4 viewMatrix) {
        Copy.set_viewMatrix(viewMatrix);
        Models.push_back(Copy);
    }

    bool has_collision_with(glm::mat4 object) {
        cv::Vec3d object_centre = get_Position_on_Mat(object);
        std::cout << "object_centre[0]: " << object_centre[0] << " vertices2[12]: " << vertices2[12] << std::endl;
             
        // se encuentra en x // marker 2 | marker 4
        if (!((object_centre[0] < vertices2[12]) && (object_centre[0] > vertices2[0]))) return false;
        // se encuentra en y
        // if ((object_centre[1] < vertices2[13])) {
        //     std::cout << "hay contacto\n" ;
        // se encuentra en z // marker 3 | marker 1
        if (!((object_centre[2] > vertices2[14]) && (object_centre[2] < vertices2[2]))) return false;

        return true;
    }

    void has_collision_with_models(glm::mat4 object) {
        for(int i = 0 ; i < Models.size() ; i++) {
            if(Models[i].has_collision_with(object)) {
                Models.erase(Models.begin() + i);
            } 
        }
    }
    

    cv::Vec3d get_Position_on_Mat(glm::mat4 input){
        return cv::Vec3d(input[3][0], input[3][1], input[3][2]);
    }

    void set_vertices_top(glm::mat4 view) {
        // top - right
        //vertices2[0] = view[3][0];
        vertices2[1] = view[3][1];
        vertices2[2] = view[3][2];

        // top - left
        //vertices2[18] = view[3][0];
        vertices2[19] = view[3][1];
        vertices2[20] = view[3][2];

        isVertexUpdated = true;
    }

    void set_vertices_bottom(glm::mat4 view) {
        // bottom - right
        //vertices2[6] = view[3][0];
        vertices2[7] = view[3][1];
        vertices2[8] = view[3][2]; 

        // bottom - left
        //vertices2[12] = view[3][0];
        vertices2[13] = view[3][1];
        vertices2[14] = view[3][2]; 

        isVertexUpdated = true;
    }

    void set_vertices_left(glm::mat4 view) {
        // top - left
        vertices2[18] = view[3][0];
        //vertices2[19] = view[3][1];
        //vertices2[20] = view[3][2];

        // bottom - left
        vertices2[12] = view[3][0];
        //vertices2[13] = view[3][1];
        //vertices2[14] = view[3][2]; 

        isVertexUpdated = true;
    }

    void set_vertices_right(glm::mat4 view) {
        // top - right
        vertices2[0] = view[3][0];
        //vertices2[1] = view[3][1];
        //vertices2[2] = view[3][2];

        // bottom - right
        vertices2[6] = view[3][0];
        //vertices2[7] = view[3][1];
        //vertices2[8] = view[3][2]; 

        isVertexUpdated = true;
    }


private:
    vector<Model> Models;
    float range_Colission = 0.2f;
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