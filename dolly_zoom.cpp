#include "EcoTracker/eco.hpp"
#include <opencv2/highgui.hpp>

int main(int argc, char* argv[]) {
    eco::ECO tracker;
    eco::EcoParameters parameters;
    
    std::string path;
    if (argc > 1) {
        path = std::string(argv[1]);
    } else {
        std::cout << "Input file name: " << std::flush;
        std::cin >> path;
    }
    
    cv::VideoCapture video(path);

    if (!video.isOpened()) {
        std::cout << "Cannot open '" << path << "'.\n" << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat frame;
    video >> frame;
    std::cout << "Frame size: " << frame.size() << "\n";

    std::string windowName = "Video";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::namedWindow("Result", cv::WINDOW_NORMAL);

    cv::Rect2f userChoice = cv::selectROI(windowName, frame);
    auto roi = userChoice;
    std::cout << "Selected ROI: " << roi << "\n";
    std::cout << std::flush;

    cv::Rect2f croppedFrame = cv::selectROI(windowName, frame);
    std::cout << "Cropped window: " << croppedFrame << "\n";
    std::cout << std::flush;

    auto shift = roi.tl() - croppedFrame.tl();

    tracker.init(frame, roi, parameters);

    while (video.isOpened()) {
        video >> frame;
        if (frame.empty()) { break; }
        
        tracker.update(frame, roi);

        auto ratio = std::sqrt(roi.area() / userChoice.area());
        std::cout << "ratio: " << ratio << std::endl;
        auto currentFrameTopLeft = roi.tl() - (shift * ratio);
        cv::Rect2f currentFrame(currentFrameTopLeft, croppedFrame.size() * ratio);
        cv::imshow("Result", frame(currentFrame));
        cv::waitKey(20);

        cv::rectangle(frame, roi, cv::Scalar(0,255,0), 5);
        cv::rectangle(frame, currentFrame, cv::Scalar(255,0,0), 5);
        cv::imshow(windowName, frame);
        cv::waitKey(30);
    }
}
