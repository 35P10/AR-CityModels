void create_chArucoBoard(){
    // Definir las dimensiones del Charuco Board
    int squaresX = 5;
    int squaresY = 7;
    float squareLength = 0.04; // Tamaño del cuadrado en metros
    float markerLength = 0.02; // Tamaño del marcador en metros
    int margins = 10;

    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    // Crear el Charuco Board
    cv::aruco::CharucoBoard board(Size(squaresX, squaresY), squareLength, markerLength, dictionary);
    cv::Mat boardImage;
    board.generateImage(cv::Size(600, 500), boardImage, margins, 1);
    cv::imwrite("BoardImage.jpg", boardImage);
}