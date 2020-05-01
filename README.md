# Example project how-to build HLS-streaming server with OpenCV support using oat++.

Live stream is tested with VLC-player.

## Overview

Server generates infinite .m3u8 playlist from the pre-generated 
```playlist1.m3u8``` file in the ```video``` folder.  

Server is built using oat++ Async-Api and has following endpoints:

- ```("GET", "/", Root)``` - Starting html-page aka index.html
- ```("GET", "/frames", Frames)``` - Extract frames from video into video/screenshots 
- ```("GET", "/record", Record)``` -  Extract frames from video into video/record

### Project layout

```

|- CMakeLists.txt                       // projects CMakeLists.txt
|- src/                                 // source folder
|- utility/install-oatpp-modules.sh     // utility script to install required oatpp-modules.
|
|- video/                               // media files and playlists here
     |- generate_pls.sh                 // example how to use ```ffmpeg``` to generate initial playlist and video chunks
     |- playlist1.m3u8              // playlist used to generate infinite playlist for http-live-streaming

```
```
- src/
    |
    |- controller/              // Folder containing UserController where all endpoints are declared
    |- hls/                     // Playlist generator is here
    |- auth/                    // Basic Auth support 
    |- Utils.hpp                // Utils
    |- AppComponent.hpp         // Service config
    |- App.cpp                  // main() is here
    
```

---

### Build and Run

#### Using CMake

**Requires**
- `OpenCV` installed. 
- `oatpp` module installed. You may run `utility/install-oatpp-modules.sh` 
script to install required oatpp modules.

```
$ mkdir build && cd build
$ cmake ..
$ make 
$ ./src/hls_stream_server             # - run application.
```

#### On/In Docker :)

As a docker image base the archlinux/base image is used.
You can find built image on hub.docker.com `https://hub.docker.com/r/lnikon/stream_app`

Following libraries were installed:
  - opencv 
  - vtk 
  - cmake 
  - git 
  - make 
  - gtk3 
  - hdf5 
  - glew 
  - gdk-pixbuf2 
  - base-devel

```
$ docker build -t streamApp .
$ docker run -p 8000:8000 -t streamApp
```
## Basic Auth
Login: hello
Password: zarzand

## Interact
To interact with server, `curl` can be used.
- `CurlStartFrames.sh` - send requests to 127.0.0.1:8000/frames
- `CurlStartRecord.sh` - send requests to 127.0.0.1:8000/record

### Files
- {repo}/video/playlist1.m3u8 - playlist used for live HLS streaming
- {repo}/video/generate_pls.sh - example script used to generate sub-playlists and video fragmets using ffmpeg tool. #EXTINF sections of sub-playlists then have to be manualy moved to playlist_live.m3u8.

### Note
If app can't find playlist of video files then specify full-file-paths for {repo}/video/playlist1.m3u8 file and {repo}/video folder in AppComponent.hpp

