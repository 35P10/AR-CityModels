int create_marker (int markerId, std::string outfile = "marker.png", int markerSize = 200, int borderBits = 1) {

    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

    cv::Mat markerImg;
    cv::aruco::generateImageMarker(dictionary, markerId, markerSize, markerImg, borderBits);

    imwrite(outfile, markerImg);

    return 0;
}