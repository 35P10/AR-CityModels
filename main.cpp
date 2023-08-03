#include <stdio.h>
#include <iostream>
#include <filesystem>

#include <map>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>
#include "aruco_samples_utility.hpp"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "charuco_calibration.hpp"
#include "aruco_creator.hpp"

#include "shader.hpp"
#include "mesh.hpp"
#include "model.hpp"

#include "obj.hpp"
#include "plane_texture.hpp"
#include "plane.hpp"
#include "interactor.hpp"

#include "augmented_mockup.hpp"

using namespace cv;
namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void create_chArucoBoard(void);

int options = 1;

int main(int argc, char** argv ){

    if (options == 0) {
        create_chArucoBoard();
        return 0;
    }
    
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

    cv::VideoCapture inputVideo(0);
    const int VideoCaptureWidth = inputVideo.get(cv::CAP_PROP_FRAME_WIDTH);
    const int VideoCaptureHeight = inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::cout << "Resolucion de la camara: " << VideoCaptureWidth << "x" << VideoCaptureHeight << std::endl;    

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(VideoCaptureWidth, VideoCaptureHeight, "GLFW Original", NULL, NULL);
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

    ArucoMockup test1(VideoCaptureWidth,VideoCaptureHeight,0.8f);

    cv::Mat cameraMatrix, distCoeffs;

    bool is_camera_params_def = false;
    if(is_camera_params_def) {
        //const string dir_camera_parameters = fs::absolute("resources/camera_parameters.yml").string();
        //std::cout << "Cargar camera_parameters en: " << dir_camera_parameters.c_str() << std::endl;
        readCameraParameters("F:/Projects/ComputerGraphics-FinalProject/camera_parameters.yml", cameraMatrix, distCoeffs); // This function is implemented in aruco_samples_utility.hpp
    }
    else {
        double K_[3][3] =
        { { 675, 0, 320 },
        { 0, 675, 240 },
        { 0, 0, 1 } };
        cameraMatrix = cv::Mat(3, 3, CV_64F, K_).clone();

        double dist_[] = { 0, 0, 0, 0, 0 };
        distCoeffs = cv::Mat(5, 1, CV_64F, dist_).clone();
    }

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    aruco::ArucoDetector detector(dictionary, detectorParams);

    cv::Mat frame_input;
    
    while (!glfwWindowShouldClose(window)) {
        // Capturar un fotograma del flujo de video
        if (!inputVideo.retrieve(frame_input))
            break;

        glfwMakeContextCurrent(window);

        //cv::resize(frame_input, frame_input, cv::Size(512, 512), 0, 0, cv::INTER_CUBIC);
        test1.render(frame_input, detector, cameraMatrix, distCoeffs);   

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

