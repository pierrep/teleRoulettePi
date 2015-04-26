#include "ofxThreadedYouTubeDownloader.h"



ofxThreadedYouTubeDownloader::ofxThreadedYouTubeDownloader()
{
    startThread();
}

ofxThreadedYouTubeDownloader::~ofxThreadedYouTubeDownloader()
{
	cout << "ofxThreadedYouTubeDownloader destructor" << endl;
    condition.signal();

    // stop threading
    if(isThreadRunning())
		waitForThread(true);
}


// Load a url
//--------------------------------------------------------------
void ofxThreadedYouTubeDownloader::loadYouTubeURL(string _url, int _id)
{
	ofLogNotice("ofxThreadedYouTubeDownloader") << "Load YouTube URL" << _url;
	ofYouTubeLoaderEntry entry(_url, _id);

    lock();
	urls_to_load_buffer.push_back(entry);
    condition.signal();
    unlock();
    ofLogNotice("ofxThreadedYouTubeDownloader") << "END Load YouTube URL";
}

//------------------------------------------------------------------------------
const string ofxThreadedYouTubeDownloader::genRandomString(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    string s;

    for (int i = 0; i < len; ++i) {
        char c = alphanum[rand() % (sizeof(alphanum) - 1)];
        s.push_back(c);
    }

    return s;
}

//------------------------------------------------------------------------------
const string ofxThreadedYouTubeDownloader::getRandomURL()
{
    string url = "";
	const string search_url = "https://gdata.youtube.com/feeds/api/videos?q=\"v="+genRandomString(4)+"\"&alt=json";
//    cout << "--------------------------------------------------" << endl;
	ofLogNotice("ofxThreadedYouTubeDownloader") << "new random URL=" << search_url;

	if (!response.open(search_url)) {
		cout  << "Failed to parse JSON\n" << endl;
	}

    //cout << response.getRawString(true) << endl;

    string encoding = response["encoding"].asString();
    //cout << "encoding = " << encoding << endl;

	int numVideos = response["feed"]["entry"].size();
	ofLogNotice("ofxThreadedYouTubeDownloader") << "num new Random Videos = " << numVideos << endl;

    int i = ofRandom(0,numVideos);


    Json::Value entryNode = response["feed"]["entry"][i];

    Json::StyledWriter writer;
   //cout << "JSON output:" << endl;
    //cout << writer.write(entryNode) << endl;

//    string seconds = entryNode["media$group"]["yt$duration"]["seconds"].asString();
//    cout << "seconds = " << seconds << endl;
//    cout << "title = " << entryNode["title"]["$t"].asString() << endl;
//    cout << "author = " << entryNode["author"][0]["name"]["$t"].asString() << endl;
//    cout << "link = " << entryNode["link"][0]["href"].asString() << endl;
//    cout << "--------------------------------------------------" << endl;
    url = entryNode["link"][0]["href"].asString();

    return url;
}

// Get a new url - called from within thread
//--------------------------------------------------------------
bool ofxThreadedYouTubeDownloader::getNewURL(ofYouTubeLoaderEntry& entry )
{
    string new_url = entry.input_url;

    if(new_url == "") {
        new_url = getRandomURL();
        if(new_url == "") {
			ofLogError("ofxThreadedYouTubeDownloader") << "Empty new URL!";
			return false;
		}
    }

    //string video_url = "youtube-dl -g -f 18 \"" + new_url + "\"";

	FILE *in;
	char buff[2048];

	//if(!(in = popen(video_url.c_str(), "r"))){
		//cout << "failed to popen" << endl;
		//return false;
	//}


	
	ofLogNotice("ofxThreadedYouTubeDownloader") << "Downloading file";
	string video_url = "youtube-dl -o 'data/videos/%(id)s.%(ext)s' -f 18 \"" + new_url + "\"";

	if(!(in = popen(video_url.c_str(), "r"))){
		cout << "failed to popen" << endl;
		pclose(in);	
		return false;
	}
	int count = 0;

	string vid;
	while(fgets(buff, sizeof(buff), in)!=NULL){
		count+=1;		
		cout << buff;
		if(count == 2) {			
			//char* ptr;
			//char name[11];
			//ptr = buff + 10;
			//vid = strncpy(name,ptr,11);
			vid = buff;
			vid = vid.substr(10,11);
			vid = vid + string(".mp4");
			cout << "name=" << vid << endl;
		}
		
	}
	pclose(in);	
	video_url = vid;
	string cmd = "touch ./data/videos/"+video_url;
	ofLogNotice("ofxThreadedYouTubeDownloader") << "Command:" << cmd;
	
	struct stat st1;
	
	string p = "./data/videos/"+video_url;
	int err1 = stat(p.c_str(),&st1);
	if(err1 != 0)
    {
        ofLogError("ofxThreadedYouTubeDownloader") << "stat() failed. Error num = " << err1;
        return false;
    } else {
		ofLogNotice("ofxThreadedYouTubeDownloader") << "size = " << st1.st_size;
	}
	
	if(!(in = popen(cmd.c_str(), "r"))){
		cout << "failed to popen" << endl;
		return false;
	}
	pclose(in);	
	ofLogNotice("ofxThreadedYouTubeDownloader") << "File Downloaded.";

	cmd = "rm -f ./data/archive/*";
	ofLogNotice("ofxThreadedYouTubeDownloader") << cmd << endl;
	if(!(in = popen(cmd.c_str(), "r"))){
		cout << "failed to popen" << endl;
		return false;
	}
	pclose(in);

    //video_url = buff;
    //video_url.erase( std::remove_if(video_url.begin(), video_url.end(), ::isspace ), video_url.end() );

    entry.url = video_url;
    return true;

}
// Reads from the queue and loads new images.
//--------------------------------------------------------------
void ofxThreadedYouTubeDownloader::threadedFunction()
{
    deque<ofYouTubeLoaderEntry> urls_to_load;

	while( isThreadRunning() ) {
		lock();
		if(urls_to_load_buffer.empty()) condition.wait(mutex);
		urls_to_load.insert( urls_to_load.end(),
							urls_to_load_buffer.begin(),
							urls_to_load_buffer.end() );

		urls_to_load_buffer.clear();
		unlock();


        while( !urls_to_load.empty() ) {
            ofYouTubeLoaderEntry  & entry = urls_to_load.front();

            if(!getNewURL(entry)) {
                ofLogError("ofxThreadedYouTubeDownloader") << "couldn't load url: \"" << entry.input_url << "\"";
                //get another random video and try again
                //loadYouTubeURL("",entry.id);
            }
            else {
				ofLogNotice() << "ofxThreadedYouTubeDownloader got video";
                ofxYouTubeURLEvent e = ofxYouTubeURLEvent(entry.url, 0);
                ofNotifyEvent(youTubeURLEvent, e, this);
            }

    		urls_to_load.pop_front();
        }
	}
}
