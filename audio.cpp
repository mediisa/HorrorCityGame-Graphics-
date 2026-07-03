#include "Audio.h"
#include <iostream>

Audio::Audio() {
    InitAudioDevice();
}

Audio::~Audio() {
    for (auto& pair : musics) {
        UnloadMusicStream(pair.second);
    }
    CloseAudioDevice();
}

void Audio::LoadMusic(const std::string& name, const std::string& filePath) {
    musics[name] = LoadMusicStream(filePath.c_str());
}

void Audio::PlayMusic(const std::string& name, bool preventOverlap) {
    auto it = musics.find(name);
        if (it != musics.end()) {
            Music& music = it->second;
        
        if (!IsMusicStreamPlaying(music)) {
            
            if (preventOverlap){
                for (auto& pair : musics) {
                    if (pair.first != name && IsMusicStreamPlaying(pair.second)) {
                        StopMusicStream(pair.second);
                    }
                }
            }
            SetMusicVolume(music, 1.0f);
            musics[name].looping = true;
            PlayMusicStream(musics[name]);
        }
    }
}


void Audio::StopMusic(const std::string& name) {
    auto it = musics.find(name);
    if (it != musics.end()) {
        StopMusicStream(it->second);
    }
}

void Audio::UpdateMusic() {
    for (auto& pair : musics) {
        if (IsMusicStreamPlaying(pair.second)) {
            UpdateMusicStream(pair.second);
        }
    }
}