#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOM_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOM_H

#include <iostream>
#include <chrono>
#include <string>
#include <sstream>

#include <jwt-cpp/jwt.h>

#include "Config.h"
#include "util/Singleton.h"
#include "util/Log.h"
#include "meeting_service_interface.h"
#include "meeting_service_components/meeting_video_interface.h"
#include "zoom_sdk.h"
#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_renderer_interface.h"

#include "meeting_service_components/meeting_audio_interface.h"
#include "meeting_service_components/meeting_participants_ctrl_interface.h"
#include "setting_service_interface.h"

#include "events/AuthServiceEvent.h"
#include "events/MeetingServiceEvent.h"
#include "events/MeetingReminderEvent.h"
#include "events/MeetingRecordingCtrlEvent.h"

#include "raw_record/ZoomSDKRendererDelegate.h"
#include "raw_record/ZoomSDKAudioRawDataDelegate.h"
#include "raw_send/ZoomSDKVideoSource.h"
#include "raw_send/ZoomSDKVirtualAudioMicEvent.h"


using namespace std;
using namespace jwt;
using namespace ZOOMSDK;

typedef chrono::time_point<chrono::system_clock> time_point;

class Zoom : public Singleton<Zoom> {

    friend class Singleton<Zoom>;

    Config m_config;

    string m_jwt;

    time_point m_iat;
    time_point m_exp;

    IMeetingService* m_meetingService;
    ISettingService* m_settingService;
    IAuthService* m_authService;

    IZoomSDKRenderer* m_videoHelper;
    ZoomSDKRendererDelegate* m_videoSource;
    IZoomSDKAudioRawDataHelper* m_audioHelper;
    ZoomSDKAudioRawDataDelegate* m_audioSource;
    ZoomSDKVideoSource* virtual_camera_video_source;
    SDKError createServices();
    void generateJWT(const string& key, const string& secret);

    /**
     * Callback fired when the SDK authenticates the credentials
    */
    function<void()> onAuth = [&]() {
        auto e = isMeetingStart() ? start() : join();
        string action = isMeetingStart() ? "start" : "join";
        
        if(hasError(e, action + " a meeting")) exit(e);
    };

    /**
     * Callback fires when the bot joins the meeting
    */
    function<void()> onJoin = [&]() {
        auto* reminderController = m_meetingService->GetMeetingReminderController();
        reminderController->SetEvent(new MeetingReminderEvent());

        if (m_config.useRawRecording()) {
            auto recordingCtrl = m_meetingService->GetMeetingRecordingController();
            auto* p_videoSourceHelper = GetRawdataVideoSourceHelper();
            // Zoom::virtual_camera_video_source = new ZoomSDKVideoSource("out/run AI on your laptop....it's PRIVATE!!.mp4");
            // SDKError err = SDKERR_SUCCESS;
            // if(p_videoSourceHelper)
            // {
            //     SDKError err = p_videoSourceHelper->setExternalVideoSource(virtual_camera_video_source);
            //     cout << virtual_camera_video_source;
            // }
            // if (err != SDKERR_SUCCESS) {
			// 	printf("attemptToStartRawVideoSending(): Failed to set external video source, error code: %d\n", err);
			// }
			// else {
			// 	printf("attemptToStartRawVideoSending(): Success \n");
				IMeetingVideoController* meetingController = m_meetingService->GetMeetingVideoController();
				meetingController->UnmuteVideo();

			}
            auto m_videoCtrl = m_meetingService->GetMeetingVideoController();
            m_videoCtrl->UnmuteVideo();


            function<void(bool)> onRecordingPrivilegeChanged = [&](bool canRec) {
                if (canRec)
                {
                    StartRawSending();
                }
                else
                {
                    stopRawRecording();
                }
            };

            auto recordingEvent = new MeetingRecordingCtrlEvent(onRecordingPrivilegeChanged);
            recordingCtrl->SetEvent(recordingEvent);

            startRawRecording();
        }
    };

public:
    SDKError init();
    SDKError auth();
    SDKError config(int ac, char** av);

    SDKError join();
    SDKError start();
    SDKError leave();

    SDKError clean();

    SDKError startRawRecording();
    SDKError stopRawRecording();

    SDKError StartRawSending();

    bool isMeetingStart();

    static bool hasError(SDKError e, const string& action="");

};

#endif //MEETING_SDK_LINUX_SAMPLE_ZOOM_H
