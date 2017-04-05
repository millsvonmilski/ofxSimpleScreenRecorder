#pragma once
#include "ofMain.h"

//
// screenCaptureThread.h
//
// *ffmpeg must be installed
// - a. install guide for all platforms
// https://trac.ffmpeg.org/wiki/CompilationGuide
//
// created on Apr, 2017 @FakeLove http://www.fakelove.tv/
// author - av http://avseoul.net http://kimsehyun.kr
//
class screenCaptureThread : public ofThread{
public:
    screenCaptureThread(){}
    ~screenCaptureThread(){
        waitForThread(true);
    }
    void setup(int _w, int _h, std::string _path = ""){
        w = _w, h = _h, frame = 0, bffrCount = 0, thrdBffrCount = 0, saveCount = 0;
        initBuffer();
        
        pixels.resize(numBffr);
        for(int i = 0; i < numBffr; i++)
            pixels[i] = ofPixels();
        
        path = _path == "" ? ofDirectory::ofDirectory(_path).getAbsolutePath() + "/" : _path;
        std::cout << "SCThread::target path - " << path << endl;
        
        threadRunning = false;
        firstFrame = true;
        stopTriggered = false;
    }
    
    void start(){
        if(!threadRunning){
            reset();
            startThread();
            threadRunning = true;
        } else
            std::cout << "SCThread::thread running" << endl;
    }
    
    void stop(){
        if(threadRunning)
            stopTriggered = true;
        else
            std::cout << "SCThread::thread not running" << endl;
    }
    
    void beginDraw(){
        fbo.begin();
        ofClear(0);
    }
    
    void endDraw(){
        if(stopTriggered) showWarning();
        fbo.end();
        fbo.draw(0,0);
        
        bool _save = saveCount%2 == 0 ? true : false;
        if(_save && threadRunning && !stopTriggered){
            if(!firstFrame){
                bffr_ping.unmap();
            }
            firstFrame = false;
            
            fbo.getTexture().copyTo(bffr_ping);
            
            bffr_pong.bind(GL_PIXEL_UNPACK_BUFFER);
            unsigned char * p = bffr_pong.map<unsigned char>(GL_READ_ONLY);
            pixels[bffrCount].setFromPixels(p,w,h,OF_PIXELS_RGB);
            std::swap(bffr_ping, bffr_pong);
            
            bffrCount++;
            
            if(bffrCount >= numBffr) {
                std::cout<<"SCThread::buffer reaches limit - stopping rendering" << endl << "change numBffr to increase limit";
                stop();
            }
        }
        saveCount++;
        
        if(stopTriggered && bffrCount-1 <= thrdBffrCount){
            threadRunning = false;
            runFFMpeg();
            reset();
            stopThread();
        }
    }
    
    int numBffr = 9999;
    
private:
    int w, h, frame, bffrCount, thrdBffrCount, saveCount;
    std::string path;
    ofFbo fbo;
    ofBufferObject bffr_ping, bffr_pong;
    vector<ofPixels> pixels;
    bool threadRunning, firstFrame, stopTriggered;
    
    void initBuffer(){
        fbo.allocate(w,h,GL_RGB);
        bffr_ping.allocate(w*h*3,GL_DYNAMIC_READ);
        bffr_pong.allocate(w*h*3,GL_DYNAMIC_READ);
    }
    
    void reset(){
        saveCount = 0;
        frame = 0;
        thrdBffrCount = 0;
        bffrCount = 0;
        stopTriggered = false;
    }
    
    void showWarning(){
        std::ostringstream warning;
        warning << "//////////////////////////" << endl << "// DO NOT CLOSE THE APP //" << endl << "//////////////////////////" << endl << endl << "PROCESS - " << std::floor((float)thrdBffrCount/(float)(bffrCount-1.f)*10000.f)*.01f << "%";
        ofPushStyle();
        ofSetColor(255,255,0);
        ofBeginShape();
        ofVertex(0, h/2.f-50.f, 0), ofVertex(0, h/2.f+50.f, 0), ofVertex(w, h/2.f+50.f, 0), ofVertex(w, h/2.f-50.f, 0);
        ofEndShape();
        ofSetColor(255,10,100);
        ofDrawBitmapString(warning.str(), w/2.f-90.f, h/2.f-50.f+27.f);
        ofPopStyle();
        warning.str("");
    }
    
    std::string getPath(std::string _r, int _i){
        std::ostringstream _n;
        _n << setw(4) << setfill('0') << _i;
        std::string _p =  _r + "render_" + _n.str() + ".png";
        return _p;
    }
    
    void runFFMpeg(){
        std::string _tstamp = ofGetTimestampString();
        // if is linux
//        std::string _cmd = "cd " + path + " && ffmpeg -start_number 1 -framerate 30 -i render_%04d.png -c:v libx264 -vf fps=30 -pix_fmt yuv420p video"+_tstamp+".mp4";
        // else if is mac
        // check this thread out
        // https://forum.openframeworks.cc/t/launching-and-configuring-terminal-window-from-of-application/18236/10
        std:string _rmfiles = "cd " + path + " && rm -rf ";
        for(int i = 0; i < frame+1; i++){
            std::ostringstream _n;
            _n << setw(4) << setfill('0') << i;
            std::string _p =  "render_" + _n.str() + ".png ";
            _rmfiles += _p;
        }
        std::string _cmd = "osascript -e 'tell application \"Terminal\" to do script \"cd " + path + " && ffmpeg -start_number 1 -framerate 30 -i render_%04d.png -c:v libx264 -vf fps=30 -pix_fmt yuv420p -preset ultrafast video"+_tstamp+".mp4 && " + _rmfiles + " && exit\"'";
        std::system(_cmd.c_str());
        std::cout << "SCThread::rendering completed" << endl;
    }
    
    void threadedFunction(){
        std::cout << "SCThread::thread initiated" << endl;
        ofSleepMillis(1000);
        std::cout << "SCThread::thread started" << endl;
        
        while(threadRunning){
            std::cout<< "SCThread::saved buffers - "  << thrdBffrCount << " ea / stored buffers - " << bffrCount-1 << " ea" << endl;
            if(stopTriggered)
                std::cout<< "SCThread::rendering process - " << std::floor((float)thrdBffrCount/(float)(bffrCount-1.f)*10000.f)*.01f << "%" << endl << endl << "//////////////////////////" << endl << "// DO NOT CLOSE THE APP //" << endl << "//////////////////////////" << endl << endl;
            
            std::string _p = getPath(path, frame);
            ofSaveImage(pixels[thrdBffrCount],_p);
            frame++;
            thrdBffrCount++;
        }
        
        std::cout << "SCThread::thread ended" << endl;
    }
    
};
