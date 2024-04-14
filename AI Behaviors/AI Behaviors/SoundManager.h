#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <queue>

class SoundManager 
{
public:

    static SoundManager& getInstance() 
    {
        static SoundManager instance;
        return instance;
    }

    // stuff that plays for a short time
    void loadSound(const std::string& name, const std::string& filename);
    void playSound(const std::string& name);
    void stopSound(const std::string& name);
    void setSoundVolume(const std::string& name, float volume);
    void setGlobalSoundVolume(float volume);

    // Stuff that plays for a long time
    void loadMusic(const std::string& name, const std::string& filename);
    void playMusic(const std::string& name, bool loop);
    void stopMusic(const std::string& name);
    void setMusicVolume(const std::string& name, float volume);
    void setGlobalMusicVolume(float volume);

    // Playlist functions
    void addToPlaylist(const std::string& name);
    void playNextTrack();
    void updateMusicTrack();
    void stopAllMusic();

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

private:

    SoundManager();
    ~SoundManager();

    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, sf::Sound> sounds;
    std::map<std::string, sf::Music*> musics;
    std::queue<std::string> playlist;
    std::string currentTrack;
    float globalSoundVolume = 100.0f;
    float globalMusicVolume = 100.0f;

};
