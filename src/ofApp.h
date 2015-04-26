#pragma once


#include "ofMain.h"
#include "ofxJSONElement.h"
#include "ofxThreadedYouTubeDownloader.h"
#include "ofxOMXPlayer.h"

#include "VideoReader.h"



class ofApp : public ofBaseApp, public ofxOMXPlayerListener {
public:
    void setup();
    void draw();
    void update();
    void exit();
    void keyPressed (int key);

    void getNewVideo();
    void getNextVideo();
    void endVideo();
    void receivedYouTubeURLEvent(ofxYouTubeURLEvent & event);
	void onVideoEnd(ofxOMXPlayerListenerEventData& e);
	void onVideoLoop(ofxOMXPlayerListenerEventData& e){ /*empty*/ };

    ofxThreadedYouTubeDownloader loader;
	ofxOMXPlayer ytVideo;	
	ofxOMXPlayerSettings settings; 
	VideoReader vidreader;
    string currentVideo;	
    float ratio;    

    bool bPaused;
    bool bShowInfo;
    
    bool bTriggerNextMovie; 
      
    bool bExiting;
    bool bLoading;
    bool bInitVids;
    bool bEndMovie;
    bool bReceivedEvent;
    
    enum VideoState {LOADING, INITVIDS, ENDMOVIE, EXITING};
    VideoState state;

    float curTime,prevTime;
    
    ofTrueTypeFont font;
};
