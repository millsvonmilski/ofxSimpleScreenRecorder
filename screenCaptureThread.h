#pragma once
#include "ofMain.h"
#define GLSL(version, shader)  "#version " #version "\n" #shader


// ffmpeg must be installed
// $sudo apt install ffmpeg
class screenCaptureThread : public ofThread{
public:
    screenCaptureThread(){}
    void setup(int _w, int _h, std::string _path){
        w = _w, h = _h;
        frame = 0;
        path = _path;
        initFbo();
    }
    void start(){startThread();}
    void stop(){
        stopThread();
        runFFMpeg();
    }
    void drawBegin(){
        img.clear();
        fbo.begin();
        ofClear(0);
    }
    void drawEnd(){
        fbo.end();
        fbo.draw(0,0);
        img.grabScreen(0,0,w,h);
    }
    void reset(){
        frame = 0;
    }

private:
    int w, h, frame, resCount = 0;
    std::string path;
    ofFbo fbo;
    ofBuffer bffr;
    ofImage img;
    void initFbo(){
        ofFbo::Settings settings;
        settings.width = w;
        settings.height = h;
        settings.internalformat = GL_RGB32F;
        settings.minFilter = GL_NEAREST;
        settings.maxFilter = GL_NEAREST;
        fbo.allocate(settings);
        fbo.begin();ofClear(0);fbo.end();
    }
    std::string getPath(std::string _r){
        std::ostringstream _n;
        _n << setw(4) << setfill('0') << frame;
        std::string _p =  _r + "render_" + _n.str() + ".jpg";
        frame++;
        return _p;
    }
    void runFFMpeg(){
        std::string _tstamp = ofGetTimestampString();
        std::string _cmd = "cd " + path + " && ffmpeg -framerate 25 -i render_%04d.jpg video"+_tstamp+".mov";
//        std::cout << _cmd << endl;
        int _res = std::system(_cmd.c_str());
        std::cout << "response : " << _res << endl;
        std:string _rmfiles = "cd " + path + " && rm -rf ";
        for(int i = 0; i < frame; i++){
            std::ostringstream _n;
            _n << setw(4) << setfill('0') << i;
            std::string _p =  "render_" + _n.str() + ".jpg ";
            _rmfiles += _p;
        }
        std::system(_rmfiles.c_str());
        std::cout << "image sequences deleted" << endl;
        reset();
    }
    void threadedFunction(){
        // start
        while(isThreadRunning()){
            // do something
            std::string _p = getPath(path);
            if(img.isAllocated())
                img.save(bffr, OF_IMAGE_FORMAT_JPEG, OF_IMAGE_QUALITY_HIGH);
            bool _log = ofBufferToFile(_p, bffr, true);
            ofSleepMillis(1);

            if(_log)
                std::cout << "img saved at " << _p << endl;
        }
        // done
    }
};
