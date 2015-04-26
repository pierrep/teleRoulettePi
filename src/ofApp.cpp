#include "ofApp.h"


void ofApp::exit()
{
	bExiting = true;
	ofLogNotice() << "Stopping thread..." << endl;
	//loader.waitForThread();
	loader.stopThread();
    ofRemoveListener(loader.youTubeURLEvent, this, &ofApp::receivedYouTubeURLEvent);
}

//------------------------------------------------------------------------------
void ofApp::onVideoEnd(ofxOMXPlayerListenerEventData& e)
{
	ofLogVerbose(__func__) << " RECEIVED";
	endVideo();
}

//------------------------------------------------------------------------------
void ofApp::endVideo()
{
	if(!bLoading) {
		ytVideo.setPaused(true);
		ofLogNotice() << "video END!";
		prevTime = curTime;
		bTriggerNextMovie = true;
	}
}

//------------------------------------------------------------------------------
void ofApp::setup()
{
	//ofToggleFullscreen();
	
	ofBackground(0,0,0);	
    ofSeedRandom(ofGetSystemTimeMicros());
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofLogToFile("Log.txt",false);
	font.loadFont("frabk.ttf", 26, false);

	settings.useHDMIForAudio = false;	//default true
	settings.enableLooping = false;		//default true
	settings.enableTexture = true;		//default true
	settings.listener = this;			//this app extends ofxOMXPlayerListener so it will receive events ;
	
	if(!settings.enableTexture) {
		settings.displayRect.width=320;
		settings.displayRect.height=240;
		settings.displayRect.x =0;
		settings.displayRect.y =0;
	}		  
    
    string path = "videos";
    int maxvids = 100;
	vidreader.setup(maxvids,path);
	bLoading = true;
	bInitVids = false;
    bShowInfo = true;
    bTriggerNextMovie = false;
    bReceivedEvent = false;

    bExiting = false;	
	bEndMovie = false;
	ratio = 0.0f;
	currentVideo = "";
	curTime = ofGetElapsedTimeMillis();
	prevTime = curTime;
	
    ofAddListener(loader.youTubeURLEvent, this, &ofApp::receivedYouTubeURLEvent);
    
    getNewVideo();

}

//------------------------------------------------------------------------------
void ofApp::update()
{
	if(bReceivedEvent) {
	    bool bDoUpdate = vidreader.update();
		if(bDoUpdate) {
			getNewVideo();
		}
		bReceivedEvent = false;
	}
	
	curTime = ofGetElapsedTimeMillis();
	if(curTime - prevTime > (1000*120)) {
		endVideo();
	}
	
	if(bTriggerNextMovie) {
		if(!bEndMovie) 	
			bEndMovie = true;
		else {
			if(!bLoading) {
				ofLogNotice() << "Trigger Next Movie";
				getNextVideo();					
			}
			else bTriggerNextMovie = false;	
		}
	}
	
	if(bLoading) {
		if(vidreader.size() > 2) {
			bLoading = false;
			bInitVids = true;
			getNextVideo();	
		}
		else return;
	}

	if((ytVideo.getWidth() > 0) && (ratio <= 0)) {
		ratio = (float)(ytVideo.getHeight() / (float)ytVideo.getWidth());
		cout << "ratio =" << 1.0f/ratio << endl;
	}

}

//------------------------------------------------------------------------------
void ofApp::draw()
{
    if(bLoading) {
        font.drawString("LOADING", ofGetWidth()/2 - 45,ofGetHeight()/2);
        return;
    }
    
	if(!ytVideo.isTextureEnabled()) {
		ofLogError() << "Texture not enabled on OMXVideoPlayer";
		return;
	}
	    
	if(vidreader.size() > 0) 
	{
		if(!bEndMovie) 
		{		
			if(ratio != 0) 
			{
				if(ofGetWidth()*ratio < ofGetHeight()) {
					ytVideo.draw(0,(ofGetHeight() - ofGetWidth()*ratio) /2.0f,ofGetWidth(),ofGetWidth()*ratio);
				}
				else {
					ytVideo.draw(0,0,ofGetWidth(),ofGetWidth()*ratio);
				}
			} 
			//else 
			//{
				//ofSetColor(0,0,0);
				//ofRectangle(0,0,ofGetWidth(),ofGetHeight());//cout << "RATIO is zero!" << endl;
			//}
		}
	}

	//if(vidreader.size() > 0) {
		//if(!bEndMovie) {
			//ytVideo.draw(0, 0, 320, 240);
		//}
	//}

    if(bShowInfo) {
        ofPushStyle();
		stringstream info;
		info <<"\n" <<	"FPS: "		<< "fps="+ofToString(ofGetFrameRate());
		info <<"\n" <<	"MOVIE: "		<< currentVideo;
		ofDrawBitmapStringHighlight(ytVideo.getInfo() + info.str(), 0, 250, ofColor(ofColor::black, 90), ofColor::yellow);        		
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofApp::receivedYouTubeURLEvent(ofxYouTubeURLEvent & event){
	
	if(bExiting) return;	
	
    ofLogNotice() << "receivedYouTubeURLEvent()" << endl;
    
	bReceivedEvent = true;
    
}

//------------------------------------------------------------------------------
void ofApp::getNewVideo()
{
	ofLogNotice() << "Get New Video...";
	int id = 0;
    loader.loadYouTubeURL("",id); //empty string = random url
}

//--------------------------------------------------------------
void ofApp::getNextVideo()
{
	currentVideo = "";
	
	if(bLoading) return;

    
	ofLogNotice() << "** get next video! **";

	string path = vidreader.nextVideo();
	if(path != "") {
		currentVideo = path;
		
		if(bInitVids) {
			ofLogNotice() << "Setup video...";
			settings.videoPath = path;
			ytVideo.setup(settings);
			bInitVids = false;
		} else {
			ofLogNotice() << "Load video...";
			ytVideo.loadMovie(path);
			ytVideo.setPaused(false);
		}

	} else {
		ofLogError() << "Path is empty";
	}
	
	getNewVideo();
	bTriggerNextMovie = false;
	bEndMovie = false;
	
	if(ytVideo.getDurationInSeconds() <= 0.0f) {
		endVideo();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key)
{
    switch(key){
        case ' ':
            endVideo();
        break;
            
        case'p':
            bPaused = !bPaused;
            ytVideo.setPaused(bPaused);
        break;
        case 'f':
            bShowInfo = !bShowInfo;
            ofToggleFullscreen();
        break;
    }
}


