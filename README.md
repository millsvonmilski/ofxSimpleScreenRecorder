# ofxSimpleScreenRecorder
a simple oF class which record screen to h.264 mpeg video without significant frame drops

## How it works
1. Set the capture size and path with setup(width, height, path). You can specify the path where video will be saved, just make sure put "/" at the last (ex. "local/somewhere/" not "local/somewhere") or simply leave it as a blank, it will use the bin folder in your oF project

2. wrap your sketch with begin(), end() in your draw loop

"'
    begin();
        draw something, this will be saved
    end();
        draw semething, this will be ignored
'"

