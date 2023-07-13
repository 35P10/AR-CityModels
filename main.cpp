#include <stdio.h>
#include <filesystem>

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
#include <filesystem>
#include <map>

#include "obj.hpp"
#include "plane_texture.hpp"
#include "plane.hpp"
#include "charuco_calibration.hpp"
#include "aruco_creator.hpp"

#include "shader.hpp"
#include "mesh.hpp"
#include "model.hpp"

using namespace cv;
namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void create_chArucoBoard();

// settings
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

const char* vertexShaderSourceModel = "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoords;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 model; \n"
    "uniform mat4 view; \n"
    "uniform mat4 projection; \n"
    "void main()\n"
    "{\n"
    "    TexCoords = aTexCoords; "
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\n";
const char* fragmentShaderSourceModel = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoords;\n"
    "uniform sampler2D texture_diffuse1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture_diffuse1, TexCoords);\n"
    "}\n\0";

#ifndef M_PI
#define M_PI       3.14159265358979323846f
#endif

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
    ///////////////////////

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    const string vertex_shader_model = fs::absolute("resources/shader/vertex_shader_model.vs").string();
    const string fragment_shader_model = fs::absolute("resources/shader/fragment_shader_model.fs").string();
    const string vertex_shader_texture = fs::absolute("resources/shader/vertex_shader_texture.vs").string();
    const string fragment_shader_texture = fs::absolute("resources/shader/fragment_shader_texture.fs").string();
    const string vertex_shader_no_texture = fs::absolute("resources/shader/vertex_shader_no_texture.vs").string();
    const string fragment_shader_no_texture = fs::absolute("resources/shader/fragment_shader_no_texture.fs").string();
    std::cout << "Cargar vertex_shader_model en: " << vertex_shader_model.c_str() << std::endl;
    std::cout << "Cargar fragment_shader_model en: " << fragment_shader_model.c_str() << std::endl;
    std::cout << "Cargar vertex_shader_texture en: " << vertex_shader_texture.c_str() << std::endl;
    std::cout << "Cargar fragment_shader_texture en: " << fragment_shader_texture.c_str() << std::endl;

    Shader ourShader(vertex_shader_model.c_str(), fragment_shader_model.c_str());
    Shader cvVideoShader(vertex_shader_texture.c_str(), fragment_shader_texture.c_str());
    Shader colorShader(vertex_shader_no_texture.c_str(), fragment_shader_no_texture.c_str());
    // load models
    // -----------
    //const string backpack_model = fs::absolute("resources/objects/backpack/backpack.obj").string();
    //std::cout << "Cargar backpack model en: " << backpack_model.c_str() << std::endl;

    std::map<int, Model> Models;
    std::map<int,glm::mat4> ModelViews;
    std::map<int,glm::mat4> Transform;


    Transform[21] = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    Transform[22] = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    Transform[23] = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    Transform[24] = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));    

    Models[21].loadModel("D:/Documents/Projects/ComputerGraphics-FinalProject/resources/objects/house/house.obj");
    Models[22].loadModel("D:/Documents/Projects/ComputerGraphics-FinalProject/resources/objects/minitower/tower.obj");
    Models[23].loadModel("D:/Documents/Projects/ComputerGraphics-FinalProject/resources/objects/temple/temple.obj");
    Models[24].loadModel("D:/Documents/Projects/ComputerGraphics-FinalProject/resources/objects/tower/tower.obj");
    //////////////////////////////

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

    Cube cubito(vertices,indices);
    

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    
    float fov = 53.13f;
    float nearp = 0.1f;
    float farp = 20.0f;
    float ratio = (1.0f * SCR_WIDTH) / SCR_HEIGHT;
    float f = 1.0f / tan(fov * (M_PI / 360.0f));
    projection[0][0] = f / ratio;
	projection[1][1] = f;
	projection[2][2] = (farp + nearp) / (nearp - farp);
	projection[3][2] = (2.0f * farp * nearp) / (nearp - farp);
	projection[2][3] = -1.0f;
	projection[3][3] = 0.0f;


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
    const float markerLength = 10.0f;
    
    // Read camera parameters from tutorial_camera_params.yml
    //readCameraParameters("D:/Documents/Projects/openCV/camera_parameters.yml", cameraMatrix, distCoeffs); // This function is implemented in aruco_samples_utility.hpp
    // Distortion coeffs (fill in your actual values here).
    double K_[3][3] =
    { { 675, 0, 320 },
    { 0, 675, 240 },
    { 0, 0, 1 } };
    cameraMatrix = cv::Mat(3, 3, CV_64F, K_).clone();

    double dist_[] = { 0, 0, 0, 0, 0 };
    distCoeffs = cv::Mat(5, 1, CV_64F, dist_).clone();
    

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
    Plane Mockup_base;
    

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
        for(auto i:ids) std::cout<< i << " ";

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
                cv::Mat viewMatrix = cv::Mat::zeros(4, 4, CV_32F);;
			    cv::Mat viewMatrixavg = cv::Mat::zeros(4, 4, CV_32F);
                cv::Vec3d r = rvecs[i];
			    cv::Vec3d t = tvecs[i];

            	cv::Mat rot;
			    Rodrigues(rvecs[i], rot);
                for (unsigned int row = 0; row < 3; ++row){
                    for (unsigned int col = 0; col < 3; ++col)
                    {
                        viewMatrix.at<float>(row, col) = (float)rot.at<double>(row, col);
                    }
                    viewMatrix.at<float>(row, 3) = (float)tvecs[i][row] * 0.1f;
                }
                viewMatrix.at<float>(3, 3) = 1.0f;

                cv::Mat cvToGl = cv::Mat::zeros(4, 4, CV_32F);
                cvToGl.at<float>(0, 0) = 1.0f;
                cvToGl.at<float>(1, 1) = -1.0f; // Invert the y axis 
                cvToGl.at<float>(2, 2) = -1.0f; // invert the z axis 
                cvToGl.at<float>(3, 3) = 1.0f;

                viewMatrix = cvToGl * viewMatrix;
                cv::transpose(viewMatrix, viewMatrix);
                
                // CV to GL
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        view[i][j] = viewMatrix.at<float>(i, j);
                    }
                }

                ModelViews[ids[i]] = view;
            }
        }
        else {
            view = glm::mat4(1.0f);
        }

        //stbi_set_flip_vertically_on_load(true); // doesnt work!! tell stb_image.h to flip loaded texture's on the y-axis.
        flip(frame_output, frame_output, 0); // flip loaded frame on the y-axis.

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        //glClearDepth(0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
        // Draw texture in GLFW window
        //glLoadIdentity();
        //CVOutput.render(cvVideoShader, frame_output);

        //std::cout << "Mockup_base: ";
        if(ModelViews.count(1) > 0) {
            //std::cout << " Top Left 1; ";
            //Mockup_base Top Left ID=1 detected
            ModelViews[1] = projection * ModelViews[1];
            Mockup_base.set_vertices_top_left(ModelViews[1][0][3], ModelViews[1][1][3], ModelViews[1][2][3]);
        }
        if(ModelViews.count(2) > 0) {
            //std::cout << " Bottom Left 2; ";
            //Mockup_base Bottom Left ID=2 detected
            ModelViews[2] = projection * ModelViews[2];
            Mockup_base.set_vertices_bottom_left(ModelViews[2][0][3], ModelViews[2][1][3], ModelViews[2][2][3]);
        }
        if(ModelViews.count(3) > 0) {
            //std::cout << " Bottom Right 3; ";
            //Mockup_base Bottom Right ID=3 detected
            ModelViews[3] = projection * ModelViews[3];
            Mockup_base.set_vertices_bottom_right(ModelViews[3][0][3], ModelViews[3][1][3], ModelViews[3][2][3]);
        }
        if(ModelViews.count(4) > 0) {
            //std::cout << "Top Right 4; ";
            //Mockup_base Top Right ID=4 detected
            ModelViews[3] = projection * ModelViews[4];
            Mockup_base.set_vertices_top_right(ModelViews[4][0][3], ModelViews[4][1][3], ModelViews[4][2][3]);
        }
        //std::cout << "detected\n";
        Mockup_base.update_vertices();
        // draw plane
        colorShader.use();
        Mockup_base.render(colorShader);

        // don't forget to enable shader before setting uniforms
        for(auto i:ids){
            if(i == 1 || i == 2 || i == 3 || i == 4) continue;
             ourShader.use();
             // view/projection transformations
             ourShader.setMat4("projection", projection);
             ourShader.setMat4("view", ModelViews[i]);
             ourShader.setMat4("model", Transform[i]);
             Models[i].Draw(ourShader);
        }

        //cubito.render(glm::mat4(1.0f), view, projection);
        
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

