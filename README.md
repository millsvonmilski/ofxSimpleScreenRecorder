# ofxSimpleScreenRecorder
a simple oF class which record screen to h.264 mpeg video without significant frame drops

## How it works
1. Set the capture size and path with setup(width, height, path). You can specify the path where video will be saved, just make sure put "/" at the last (ex. "local/somewhere/" not "local/somewhere") or simply leave it as a blank, it will use the bin folder in your oF project

2. wrap your sketch with begin(), end() in your draw loop


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

