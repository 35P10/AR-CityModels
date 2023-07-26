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
void create_chArucoBoard();


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

    cv::VideoCapture inputVideo(0);
    const double VideoCaptureWidth = inputVideo.get(cv::CAP_PROP_FRAME_WIDTH);
    const double VideoCaptureHeight = inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::cout << "Resolucion de la camara: " << VideoCaptureWidth << "x" << VideoCaptureHeight << std::endl;    

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(512, 512, "GLFW 512x512", NULL, NULL);
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

    ArucoMockup test1(512,512);


    GLFWwindow* window2 = glfwCreateWindow(400, 300, "GLFW 400x300", NULL, NULL);
    if(!window2){
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }  
   
    glfwMakeContextCurrent(window2);
    glfwSetFramebufferSizeCallback(window2, framebuffer_size_callback);

     ArucoMockup test2(400,300);


    GLFWwindow* window3 = glfwCreateWindow(VideoCaptureWidth, VideoCaptureHeight, "GLFW Original", NULL, NULL);
    if(!window3){
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }  
   
    glfwMakeContextCurrent(window3);
    glfwSetFramebufferSizeCallback(window3, framebuffer_size_callback);
    
    
    ArucoMockup test3(VideoCaptureWidth,VideoCaptureHeight);

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
    cv::Mat resizedImage0, resizedImage1, resizedImage2;
    
    while (true){
        // Capturar un fotograma del flujo de video
        if (!inputVideo.retrieve(frame_input))
            break;

        glfwMakeContextCurrent(window);

         cv::resize(frame_input, resizedImage1, cv::Size(512, 512), 0, 0, cv::INTER_CUBIC);
        test1.render(resizedImage1, detector, cameraMatrix, distCoeffs);   
        cv::imshow("OpenCV 512x512", resizedImage1);

        glfwSwapBuffers(window);
        glfwPollEvents();  

        glfwMakeContextCurrent(window2);

        cv::resize(frame_input, resizedImage0, cv::Size(400, 300), 0, 0, cv::INTER_CUBIC);
        test2.render(resizedImage0, detector, cameraMatrix, distCoeffs);
        cv::imshow("OpenCV 400x300", resizedImage0);
    

        glfwSwapBuffers(window2);
        glfwPollEvents(); 

        glfwMakeContextCurrent(window3);

        test2.render(frame_input, detector, cameraMatrix, distCoeffs);
        cv::imshow("OpenCV Original", frame_input);

        glfwSwapBuffers(window3);
        glfwPollEvents();   

          // Esperar 30 milisegundos y salir del bucle si el usuario presiona la tecla 'q'
        if (cv::waitKey(30) == 'q')
            break;

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

