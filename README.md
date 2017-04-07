# ofxSimpleScreenRecorder
A simple oF class which record screen to h.264 mpeg video without significant frame drops.

ofxSimpleScreenRecorder would be useful when you document your sketch, Documentation is a frustrating process especially if the recorder causes damm frame drops. Those frame drops usually comes from saving process, recorder waits until last frame saved in order not to miss next frame of animation. ofxSimpleScreenRecorder saves frames to buffer and save imgages in buffer apart from main app's update so you won't feel any frame drops while you recording your sketch, what you see is what you will have. 

## Requirement 
ofxSimpleScreenRecorder depends on FFmpeg library to convert image sequence to video so FFmpeg should be installed. Follow the instruction on here https://trac.ffmpeg.org/wiki/CompilationGuide

## Usage
1. Set the capture size and path with setup(width, height, path). You can specify the path where video will be saved, just make sure put "/" at the last (ex. "local/somewhere/" not "local/somewhere") or simply leave it as a blank, it will use the bin folder in your oF project

2. Wrap your sketch with begin(), end() in your draw loop

3. Call start() at when you want to start capturing

4. Call stop() at when you want to stop capturing

5. Wait until rendering process done
   do not close the app during the process 
   
6. When process done, ofxSimpleScreenRecorder will open your terminal and run FFmpeg command

7. Folder where the video is will be popped-up

8. Please enjoy the last step of the process, click the red circle at left top on the terminal
   
   
* Mainthread : store pixels of drawing to buffer array every bi-frame
* Ofthread   : save stored pixels .png to given path on local drive (bin is default)
* Terminal   : convert .png sequence to h.264 .mp4 with FFmpeg command and delete image sequence

## Usage Example
```c++
ofxSimpleScreenRecorder mRenderer; 

void ofApp::setup(){
    mRenderer.setup(ofGetWidth(), ofGetHeight());
}

void ofApp::draw(){
    mRenderer.begin();
        draw something, this will be saved
    mRenderer.end();
        draw semething, this will be ignored
}

void ofApp::keyReleased(int key){
    if(key == '1')
        mRenderer.start();
    else if(key == '2')
        mRenderer.stop();
}
```

## TODOs 
* Integrate FFmpeg library into code 
  - to convert buffer directly to video without process saving images to local storage and also without process openning terminal
* Support different OS (Currently only supports OSX / tested on OSX El Capitan & oF_v0.9.7)

