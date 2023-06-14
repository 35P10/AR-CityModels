#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>
#include "aruco_samples_utility.hpp"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <vector>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "obj.hpp"
#include "plane_texture.hpp"
#include "charuco_calibration.hpp"
#include "aruco_creator.hpp"

using namespace cv;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void create_chArucoBoard();

// settings
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

int main(int argc, char** argv ){
 // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Inicializar GLFW
    if (!glfwInit()) {
        std::cout << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if(!window){
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Hacer la ventana GLFW actual y establecer el contexto de OpenGL
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }  

    //return take_images(5,7,0.02,0.01,"camera_prueba.yml");
    

    float vertices[40] = {
        // positions          // texture coords
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, // 0
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, // 1
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, // 2
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, // 3

        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // 4
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // 5
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // 6
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // 7
    };

    unsigned int indices[36] = {
        0, 1, 2, //up
        0, 2, 3,

        0, 1, 5, //behind
        0, 4, 5,

        0, 3, 4, //left
        3, 4, 7,

        1, 2, 5, //right
        2, 5, 6,

        2, 3, 6, //front
        5, 6, 7,

        4, 5, 6, //down
        4, 6, 7
    };

    //Cube cubito(vertices,indices);
    

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(0.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);


    cv::VideoCapture inputVideo(0);
    
    glEnable(GL_TEXTURE_2D);     // Enable 2D texturing
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
    
    //glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
    //glLoadIdentity();
    //glOrtho(0.0, SCR_WIDTH, SCR_HEIGHT, 0.0, 0.0, 100.0);

    //glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling


    cv::Mat cameraMatrix, distCoeffs;
    float markerLength = 0.5f;
    
    // Read camera parameters from tutorial_camera_params.yml
    readCameraParameters("D:/Documents/Projects/openCV/camera_parameters.yml", cameraMatrix, distCoeffs); // This function is implemented in aruco_samples_utility.hpp
    
    // Set coordinate system
    cv::Mat objPoints(4, 1, CV_32FC3);
    objPoints.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-markerLength/2.f, markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(markerLength/2.f, markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(markerLength/2.f, -markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-markerLength/2.f, -markerLength/2.f, 0);

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    aruco::ArucoDetector detector(dictionary, detectorParams);

    cv::Mat frame_input, frame_output;

    Plane_texture CVOutput;
    
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps

    while(!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        inputVideo.retrieve(frame_input);
        frame_input.copyTo(frame_output);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;

        detector.detectMarkers(frame_input, corners, ids);

        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(frame_output, corners, ids);
            int nMarkers = corners.size();

            // matriz de rotacion y translacion de cada marker
            std::vector<cv::Vec3d> rvecs(nMarkers), tvecs(nMarkers);
            // Calculate pose for each marker
            for (int i = 0; i < nMarkers; i++) {
                solvePnP(objPoints, corners.at(i), cameraMatrix, distCoeffs, rvecs.at(i), tvecs.at(i));
            }
            // Draw axis for each marker
            for(unsigned int i = 0; i < ids.size(); i++) {
                cv::drawFrameAxes(frame_output, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
            }

            for (size_t i = 0; i < rvecs.size(); ++i) {

                Cube cubito(vertices,indices);

                // Convertir los valores de rotación y traslación a glm::vec3
                glm::vec3 rotationVector(rvecs[i][0], rvecs[i][1], rvecs[i][2]);
                glm::vec3 translationVector( tvecs[i][0], -1 * tvecs[i][1] , tvecs[i][2] );

                std::cout << "translation " << i << " : " << tvecs[i][0] << " " << tvecs[i][1] << " " << tvecs[i][2] << std::endl;
                
                // Crear la matriz de transformación
                model = glm::mat4(1.0f);
                model = glm::translate(model, translationVector);
                model = glm::rotate(model, rotationVector.x, glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::rotate(model, rotationVector.y, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, rotationVector.z, glm::vec3(0.0f, 0.0f, 1.0f));

                cubito.render(model, view, projection);
            }
            




        }
        else {
            //model = glm::mat4(1.0f);
        }

        //stbi_set_flip_vertically_on_load(true); // doesnt work!! tell stb_image.h to flip loaded texture's on the y-axis.
        flip(frame_output, frame_output, 0); // flip loaded frame on the y-axis.

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClearDepth(0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
        // Draw texture in GLFW window
        glLoadIdentity();
        
        CVOutput.render(frame_output);
        
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    inputVideo.release();
    glfwTerminate();

    return 1;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

