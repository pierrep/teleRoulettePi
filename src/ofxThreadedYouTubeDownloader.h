#pragma once

#include "ofMain.h"
#include "ofxJSONElement.h"
#include "ofxOMXPlayer.h"

class ofxYouTubeURLEvent;

class ofxThreadedYouTubeDownloader : public ofThread
{
    public:
        ofxThreadedYouTubeDownloader();
        virtual ~ofxThreadedYouTubeDownloader();

        void loadYouTubeURL(string url, int id);

        ofEvent<ofxYouTubeURLEvent> youTubeURLEvent;

    protected:

        // Entry to load.
        struct ofYouTubeLoaderEntry {
            ofYouTubeLoaderEntry() {
                url="";
                bLoaded = false;
                id=0;
            }

            ofYouTubeLoaderEntry(string _url, int _id) {
                id=_id;
                input_url = _url;
                bLoaded = false;
            }

            string url;
            string input_url;
            bool bLoaded;
            int id;
        };


        virtual void threadedFunction();
        bool getNewURL(ofYouTubeLoaderEntry& entry);
        const string getRandomURL();
        const string genRandomString(const int len);

        ofxJSONElement  response;


    	int                 nextID;
        Poco::Condition     condition;
        int                 lastUpdate;

    	deque<ofYouTubeLoaderEntry> urls_to_load_buffer;

    private:

};

class ofxYouTubeURLEvent
{
    public:

    ofxYouTubeURLEvent(string _url, int _id)
    : url(_url),id(_id) {};

    string url;
    int id;


};

