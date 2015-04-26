#include "VideoReader.h"

VideoReader::VideoReader() {
	bSetup = false;
}


void VideoReader::setup(int _max, string _path)
{
	maxvids = _max;
    path = _path;
    ofDirectory dir;
    dir.allowExt("mp4");
    dir.open(path);
    if(dir.exists()) {
		bSetup = true;
	} else {
		bSetup = false;
		ofLogError() << "Failed to open directory " << path <<", it doesn't exist.";
	}
}

int VideoReader::size() {
	return vidCount;	
}

void VideoReader::refresh()
{
	if(!bSetup) {
		ofLogError() << "VideoReader not setup() yet, aborting";
	}
	
	dir.reset();
	ofLogNotice() << "Refresh: open path -  " << path;
	dir.allowExt("mp4");
	dir.open(path);	
	ofLogNotice() << "Refresh: list dir ";
    dir.listDir();
    ofLogNotice() << "Refresh: sort by date ";
    dir.sortDate();
	prevVidCount = vidCount;    
	vidCount = dir.numFiles();	
	ofLogNotice() << "Video count = " << vidCount;
}

bool VideoReader::update() 
{
	refresh();
	if(prevVidCount != vidCount) {
		ofLogNotice() << "New video count = " << vidCount;
	}
	if(vidCount < maxvids) 
		return true;
	else
		return false;
	
}

string VideoReader::nextVideo()
{
    	
	if(vidCount > 0) {
		string mypath ="";
		
		if(vidCount > 1)
		{
			refresh();
			mypath = dir.getPath(0);
			mypath = ofToDataPath(mypath,true);
			string command = "mv "+mypath+" ./data/archive";
			//string command = "rm -f "+path+" &";
			//string command = "rm -f "+path;
			ofLogNotice() << "Moving file: " << command;

			FILE *in;
			char buff[2048];
			
			if(!(in = popen(command.c_str(), "r"))){
				cout << "failed to popen" << endl;
				return "";
			}

			while(fgets(buff, sizeof(buff), in)!=NULL){
				;
			}
			pclose(in);	
	
			//ofLogNotice() << "File Moved! "<< buff << endl;
		} 
				
		refresh();
		mypath = dir.getPath(0);
		mypath = ofToDataPath(mypath,true);
		ofLogNotice() << "nextVideo() = " << mypath;
		return mypath;		
	}
	
	return "";
}
