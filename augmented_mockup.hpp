#ifndef ARUCO_MOCKUP_H
#define ARUCO_MOCKUP_H

#include <stdio.h>
#include <iostream>
#include <filesystem>

#include <map>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace cv;
namespace fs = std::filesystem;

#ifndef M_PI
#define M_PI       3.14159265358979323846f
#endif

class ArucoMockup {
public:
    int SCR_WIDTH;
    int SCR_HEIGHT;
    float markerLength = 1.75f;
    float model_scale;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    float fov = 53.13f;
    float nearp = 0.001f;
    float farp = 1000.0f;
    float ratio;
    float f;

    bool is_camera_params_def = false; 
    cv::Mat objPoints;
    cv::Mat frame_output;
    Plane_texture CVOutput;
    Plane Mockup_base;
    std::map<int, Model*> Models;
    Interactor myInteractor;
    Shader ourShader;
    Shader cvVideoShader;
    Shader colorShader;

    ArucoMockup (int width, int height,float _markerLength= 1.75f) {
        markerLength = _markerLength;
        SCR_WIDTH = width;
        SCR_HEIGHT = height;
        ratio = (1.0f * SCR_WIDTH) / SCR_HEIGHT;
        model_scale = markerLength / 10;
        f = 1.0f / tan(fov * (M_PI / 360.0f));
        objPoints = cv::Mat(4, 1, CV_32FC3); 

        // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
        stbi_set_flip_vertically_on_load(true);
        glDisable(GL_DEPTH_TEST);

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
        ourShader = Shader(vertex_shader_model.c_str(), fragment_shader_model.c_str());
        cvVideoShader= Shader(vertex_shader_texture.c_str(), fragment_shader_texture.c_str());
        colorShader= Shader(vertex_shader_no_texture.c_str(), fragment_shader_no_texture.c_str());

        // load aruco
        // -----------
        // mockup base
        Models[1] = new Model();
        Models[2] = new Model();
        Models[3] = new Model();
        Models[4] = new Model();
        // interactor
        Models[5] = new Model();
        // models
        Models[21] = new Model();
        Models[22] = new Model();
        Models[23] = new Model();
        Models[24] = new Model();
        
        Models[21]->loadModel("D:/ComputerGraphics-FinalProject/resources/objects/house/house.obj", glm::rotate(glm::scale(glm::mat4(1.0f), 0.5f * glm::vec3(model_scale, model_scale, model_scale)), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)));
        Models[22]->loadModel("D:/ComputerGraphics-FinalProject/resources/objects/minitower/tower.obj", glm::rotate(glm::scale(glm::mat4(1.0f), 0.5f * glm::vec3(model_scale, model_scale, model_scale)), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)));
        Models[23]->loadModel("D:/ComputerGraphics-FinalProject/resources/objects/temple/temple.obj", glm::rotate(glm::scale(glm::mat4(1.0f), 0.4f * glm::vec3(model_scale, model_scale, model_scale)), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)));
        Models[24]->loadModel("D:/ComputerGraphics-FinalProject/resources/objects/tower/tower.obj", glm::rotate(glm::scale(glm::mat4(1.0f), 0.3f * glm::vec3(model_scale, model_scale, model_scale)), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)));
        
        myInteractor = Interactor(5, Models[5]);
        
        // set projection matrix
        projection[0][0] = f / ratio;
        projection[1][1] = f;
        projection[2][2] = (farp + nearp) / (nearp - farp);
        projection[3][2] = (2.0f * farp * nearp) / (nearp - farp);
        projection[2][3] = -1.0f;
        projection[3][3] = 0.0f;

        glEnable(GL_TEXTURE_2D);     // Enable 2D texturing
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */

        // Set coordinate system
        objPoints.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-markerLength/2.f, markerLength/2.f, 0);
        objPoints.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(markerLength/2.f, markerLength/2.f, 0);
        objPoints.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(markerLength/2.f, -markerLength/2.f, 0);
        objPoints.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-markerLength/2.f, -markerLength/2.f, 0);

    } 

    void render(cv::Mat frame_input, aruco::ArucoDetector detector, cv::Mat cameraMatrix, cv::Mat distCoeffs) {
            // input
            // -----
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
                    cv::drawFrameAxes(frame_output, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], markerLength/2);
                    if(ids[i] == myInteractor.get_selector_id()){
                        std:: cout << "Interactor: Selector encontrado" << std::endl;
                        myInteractor.set_selectorMarkerDetected(true);
                    }
                }

                for (size_t i = 0; i < rvecs.size(); ++i) {
                    cv::Mat viewMatrix = cv::Mat::zeros(4, 4, CV_32F);;
                    cv::Mat viewMatrixavg = cv::Mat::zeros(4, 4, CV_32F);
                    cv::Vec3d r = rvecs[i];
                    cv::Vec3d t = tvecs[i];

                    Models[ids[i]]->set_aruco_pose_tvec(t);

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


                    bool flipped = false;
                    

                    for (int jj = 0; jj < 4; jj++) {
                        for (int yy = 0; yy < 4; yy++) {
                            viewMatrixavg.at<float>(jj, yy) = (viewMatrix.at<float>(jj, yy) + Models[ids[i]]->View1.at<float>(jj, yy) + Models[ids[i]]->View2.at<float>(jj, yy)) / 3;
                        }
                    }


                    // CV to GL
                    for (int i = 0; i < 4; i++) {
                        for (int j = 0; j < 4; j++) {
                            view[i][j] = viewMatrix.at<float>(i, j);
                        }
                    }
                    Models[ids[i]]->set_viewMatrix(view);

                    for (int i = 0; i < 4; i++) {
                        for (int j = 0; j < 4; j++) {
                            view[i][j] = viewMatrixavg.at<float>(i, j);
                        }
                    }
                    view[0][3] = 0;
                    view[1][3] = 0;
                    view[2][3] = 0;
                    view[3][3] = 1;
                    Models[ids[i]]->ViewMatrixavg = view;


                    Models[ids[i]]->View0 = viewMatrixavg;
					Models[ids[i]]->View2 = Models[ids[i]]->View1;
					Models[ids[i]]->View1 = viewMatrix;
                }
            }
            else {
                view = glm::mat4(1.0f);
            }

            flip(frame_output, frame_output, 0); // flip loaded frame on the y-axis.

            //std::cout << "Mockup_base: ";
            if(Models.count(1) > 0) {
                //std::cout << " Top Left 1; ";
                //Mockup_base Top Left ID=1 detected
                Mockup_base.set_vertices_top(Models[1]->get_viewMatrix());
            }
            if(Models.count(2) > 0) {
                //std::cout << " Bottom Left 2; ";
                //Mockup_base Bottom Left ID=2 detected
                Mockup_base.set_vertices_left(Models[2]->get_viewMatrix());
            }
            if(Models.count(3) > 0) {
                //std::cout << " Bottom Right 3; ";
                //Mockup_base Bottom Right ID=3 detected
                Mockup_base.set_vertices_bottom(Models[3]->get_viewMatrix());
            }
            if(Models.count(4) > 0) {
                //std::cout << "Top Right 4; ";
                //Mockup_base Top Right ID=4 detected
                Mockup_base.set_vertices_right(Models[4]->get_viewMatrix());
            }
            
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            CVOutput.render(cvVideoShader, frame_output);

            Mockup_base.update_vertices();
            colorShader.use();
            colorShader.setMat4("projection", projection);
            Mockup_base.render(colorShader);


            for(auto i:ids){
                if(i == 1 || i == 2 || i == 3 || i == 4 || i == 5) continue;
                if(myInteractor.isDetected() > 0 && Models[i]->has_collision_with(myInteractor.get_viewMatrix_selector())) {
                    std::cout << "Hay colision con Modelo "<< i << std::endl;
                    std:: cout << "Collision:" << Models[i]->get_Position_on_Mat()[0] << std::endl;
                    myInteractor.set_modelCopy(Models[i]);
                }
                ourShader.use();
                ourShader.setMat4("projection", projection);
                Models[i]->render(ourShader);
            }

            ourShader.use();
            ourShader.setMat4("projection", projection);
            myInteractor.render(ourShader);
                        

    }
};

#endif