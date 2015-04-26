#pragma once
#include "ofMain.h"

class VideoReader {

public:
	VideoReader();
	void setup(int maxvids, string path);
	int size();
	string nextVideo();
	bool update();
	void refresh();

    /* video directory*/
    ofDirectory dir;
    
private:

	string path;
	int maxvids;
	bool bSetup;
	bool bStarted;
    int vidCount;
    int prevVidCount;
};
