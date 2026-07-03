#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <map>
#include "raylib.h"

class Audio {
private:
    std::map<std::string, Music> musics;
    
public:
    Audio();
    ~Audio();
    
    void LoadMusic(const std::string& name, const std::string& filePath);
    void PlayMusic(const std::string& name, bool preventOverlap);
    void StopMusic(const std::string& name);
    void UpdateMusic(); 
};

#endif