void create_chArucoBoard(){
    // Definir las dimensiones del Charuco Board
    int squaresX = 5;
    int squaresY = 7;
    float squareLength = 0.04; // Tamaño del cuadrado en metros
    float markerLength = 0.02; // Tamaño del marcador en metros
    int margins = 10;

    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    // Crear el Charuco Board
    cv::aruco::CharucoBoard board(cv::Size(squaresX, squaresY), squareLength, markerLength, dictionary);
    cv::Mat boardImage;
    board.generateImage(cv::Size(600, 500), boardImage, margins, 1);
    cv::imwrite("BoardImage.jpg", boardImage);
}

int take_images(int squaresX = 5, int squaresY = 7, float squareLength = 0.04, float markerLength = 0.02, std::string outputFile = "camera_parameters2.yml", int camId = 0) {

    bool showChessboardCorners = false;
    int calibrationFlags = 0;

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::VideoCapture inputVideo;

    int waitTime = 10;
    inputVideo.open(camId);

    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

    // Crear charuco board como objecto
    cv::aruco::CharucoBoard board(cv::Size(squaresX, squaresY), squareLength, markerLength, dictionary);
    cv::aruco::CharucoParameters charucoParams;

    cv::aruco::CharucoDetector detector(board, charucoParams, detectorParams);

    // Para almacenar los datos de cada Frame
    std::vector<cv::Mat> allCharucoCorners;
    std::vector<cv::Mat> allCharucoIds;

    std::vector<std::vector<cv::Point2f>> allImagePoints;
    std::vector<std::vector<cv::Point3f>> allObjectPoints;

    std::vector<cv::Mat> allImages;
    cv::Size imageSize;

    while(inputVideo.grab()) {
        cv::Mat image, imageCopy;
        inputVideo.retrieve(image);

        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedMarkers;
        cv::Mat currentCharucoCorners;
        cv::Mat currentCharucoIds;
        std::vector<cv::Point3f> currentObjectPoints;
        std::vector<cv::Point2f> currentImagePoints;

        // Detectar ChArUco board
        detector.detectBoard(image, currentCharucoCorners, currentCharucoIds);

        // Dibujar
        image.copyTo(imageCopy);

        if(!markerIds.empty()) {
            cv::aruco::drawDetectedMarkers(imageCopy, markerCorners);
        }

        if(currentCharucoCorners.total() > 3) {
            cv::aruco::drawDetectedCornersCharuco(imageCopy, currentCharucoCorners, currentCharucoIds);
        }

        putText(imageCopy, "Presionar 'c' para aniadir el frame. 'ESC' para calibrar",
                cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);

        imshow("out", imageCopy);

        // Wait for key pressed
        char key = (char)cv::waitKey(waitTime);

        // ESC
        if(key == 27) {
            break;
        }

        if(key == 'c' && currentCharucoCorners.total() > 3) {
            // Match image points
            board.matchImagePoints(currentCharucoCorners, currentCharucoIds, currentObjectPoints, currentImagePoints);

            if(currentImagePoints.empty() || currentObjectPoints.empty()) {
                std::cout << "Error al detectar puntos, intentalo de nuevo." << std::endl;
                continue;
            }

            std::cout << "Frame obtenido!" << std::endl;

            allCharucoCorners.push_back(currentCharucoCorners);
            allCharucoIds.push_back(currentCharucoIds);
            allImagePoints.push_back(currentImagePoints);
            allObjectPoints.push_back(currentObjectPoints);
            allImages.push_back(image);

            imageSize = image.size();
        }
    }

    if(allCharucoCorners.size() < 4) {
        std::cerr << "No hay suficientes frames para calibrar" << std::endl;
        return 0;
    }

    cv::Mat cameraMatrix, distCoeffs;

    // Calibrate camera using ChArUco
    double repError = calibrateCamera(
        allObjectPoints, allImagePoints, imageSize,
        cameraMatrix, distCoeffs, cv::noArray(), cv::noArray(), cv::noArray(),
        cv::noArray(), cv::noArray(), calibrationFlags
    );

    bool saveOk =  saveCameraParams(
        outputFile, imageSize, 1, calibrationFlags,
        cameraMatrix, distCoeffs, repError
    );

    if(!saveOk) {
        std::cerr << "No pudo guardarse el archivo" << std::endl;
        return 0;
    }

    std::cout << "Rep Error: " << repError << std::endl;
    std::cout << "Calibracion guardada en " << outputFile << std::endl;

    // Show interpolated charuco corners for debugging
    if(showChessboardCorners) {
        for(size_t frame = 0; frame < allImages.size(); frame++) {
            cv::Mat imageCopy = allImages[frame].clone();

            if(allCharucoCorners[frame].total() > 0) {
                cv::aruco::drawDetectedCornersCharuco(
                    imageCopy, allCharucoCorners[frame], allCharucoIds[frame]
                );
            }

            imshow("out", imageCopy);
            char key = (char)cv::waitKey(0);
            if(key == 27) {
                break;
            }
        }
    }

    return 0;
}
