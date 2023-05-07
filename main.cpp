#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>
#include "aruco_samples_utility.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <stb_image.h>

#include <iostream>

using namespace cv;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

    /////////////////////////////////////////////////////////////////////////
   
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
    float markerLength = 0.05;
    
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

    while(!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        // render
        // ------   
        cv::Mat frame_input, frame_output;
        inputVideo.retrieve(frame_input);
        frame_input.copyTo(frame_output);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;

        detector.detectMarkers(frame_input, corners, ids);

        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(frame_output, corners, ids);
            int nMarkers = corners.size();
            std::vector<cv::Vec3d> rvecs(nMarkers), tvecs(nMarkers);
            // Calculate pose for each marker
            for (int i = 0; i < nMarkers; i++) {
                solvePnP(objPoints, corners.at(i), cameraMatrix, distCoeffs, rvecs.at(i), tvecs.at(i));
            }
            // Draw axis for each marker
            for(unsigned int i = 0; i < ids.size(); i++) {
                cv::drawFrameAxes(frame_output, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
            }
        }


        flip(frame_output, frame_output, 0); // flip loaded frame on the y-axis.

        
        /* OpenGL texture binding of the image loaded by DevIL  */
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_output.cols, frame_output.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame_output.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //stbi_set_flip_vertically_on_load(true); // doesnt work!! tell stb_image.h to flip loaded texture's on the y-axis.
        /*
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
        glClearDepth(0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
        
        // Clear color and depth buffers
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix
        */
       
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw texture in GLFW window
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
        glEnd();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        glDeleteTextures(1, &texture);
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