//SendVideoRawData

#include "ZoomSDKVideoSource.h"
#include <iostream>
#include <thread> 
#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>
#include <fstream>
#include <vector>


//using namespace cv;
using namespace std;

int video_play_flag = -1;
int width = WIDTH;
int height = HEIGHT;



void PlayVideoFileToVirtualCamera(IZoomSDKVideoSender* video_sender, const std::string& video_source) {

    std::ifstream file(video_source, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "failed to open video file: " << video_source << endl;
        return;
    }
    else {
        std::cout << "opened video file: " << video_source << endl;
    }
    
    std::ifstream imageFile("out/image.png", std::ios::binary);
    if (!imageFile.is_open()) {
        std::cout << "Failed to open image file: cat.png" << std::endl;
        return;
    }
    
}
void ZoomSDKVideoSource::onInitialize(IZoomSDKVideoSender* sender, IList<VideoSourceCapability>* support_cap_list, VideoSourceCapability& suggest_cap)
{
    std::cout << "ZoomSDKVideoSource onInitialize waiting for turnOn chat command" << endl;
    video_sender_ = sender;
}

void ZoomSDKVideoSource::onPropertyChange(IList<VideoSourceCapability>* support_cap_list, VideoSourceCapability suggest_cap)
{
    std::cout << "onPropertyChange" << endl;
    std::cout << "suggest frame: " << suggest_cap.frame << endl;
    std::cout << "suggest size: " << suggest_cap.width << "x" << suggest_cap.height << endl;
    width = suggest_cap.width;
    height = suggest_cap.height;
    std::cout << "calculated frameLen: " << height / 2 * 3 * width << endl;
}

void ZoomSDKVideoSource::onStartSend()
{
    std::cout << "onStartSend" << endl;
    std::cout << "video_source_: " << video_source_ << endl;
    std::cout << "video_sender_: " << video_sender_ << endl;
    std::cout << "video_play_flag: " << video_play_flag << endl;
    if (video_sender_ && video_play_flag != 1) {
        while (video_play_flag > -1) {}
        video_play_flag = 1;
        cout << "Starting video thread" << endl;
        cout << "video play flag: " << video_play_flag << endl;
        thread(PlayVideoFileToVirtualCamera, video_sender_, video_source_).detach();
    }
    else {
        std::cout << "video_sender_ is null" << endl;
    }
}

void ZoomSDKVideoSource::onStopSend()
{
    std::cout << "onCameraStopSend" << endl;
    video_play_flag = 0;
}

void ZoomSDKVideoSource::onUninitialized()
{
    std::cout << "onUninitialized" << endl;
    video_sender_ = nullptr;
}

ZoomSDKVideoSource::ZoomSDKVideoSource(string video_source)
{
    video_source_ = video_source;
}

