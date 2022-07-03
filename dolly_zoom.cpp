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
#ifdef VISUALIZE_PROCESS
    cv::namedWindow("Output", cv::WINDOW_NORMAL);
#endif
    cv::Rect2f userChoice = cv::selectROI(windowName, frame);
    auto roi = userChoice;
    std::cout << "Selected ROI: " << roi << "\n";
    std::cout << std::flush;

    cv::Rect2f croppedFrame = cv::selectROI(windowName, frame);
    std::cout << "Cropped window: " << croppedFrame << "\n";
    std::cout << std::flush;

    auto shift = roi.tl() - croppedFrame.tl();

    tracker.init(frame, roi, parameters);

    cv::Size2i outSize(1280,720);

    auto pos = path.find_last_of('.');
    auto fileExtension = path.substr(pos); // .xxx
    std::string outFileName = path.substr(0, pos) + "-out" + fileExtension;

    cv::VideoWriter out(outFileName, cv::VideoWriter::fourcc('H','2','6','4'), 30, outSize);

    while (video.isOpened()) {
        video >> frame;
        if (frame.empty()) { break; }
        
        tracker.update(frame, roi);

        auto ratio = std::sqrt(roi.area() / userChoice.area());
        std::cout << "ratio: " << ratio << std::endl;
        auto currentFrameTopLeft = roi.tl() - (shift * ratio);
        cv::Rect2f currentFrame(currentFrameTopLeft, croppedFrame.size() * ratio);
#ifdef VISUALIZE_PROCESS
        cv::rectangle(frame, roi, cv::Scalar(0,255,0), 5);
        cv::rectangle(frame, currentFrame, cv::Scalar(255,0,0), 5);
        cv::imshow(windowName, frame);

        cv::imshow("Output", frame(currentFrame));
        if (cv::waitKey(30) == 27) break;
#endif
        cv::Mat t;
        cv::resize(frame(currentFrame), t, outSize);
        out << t;
    }
    video.release();
    out.release();
    return 0;
}
