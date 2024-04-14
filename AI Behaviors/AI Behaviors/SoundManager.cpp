#include "SoundManager.h"

SoundManager::SoundManager() : currentTrack("")
{
}

SoundManager::~SoundManager()
{
    for (auto& music : musics)
    {
        delete music.second;
    }
}

void SoundManager::loadSound(const std::string& name, const std::string& filename)
{
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(filename))
    {
        soundBuffers[name] = buffer;
        sounds[name].setBuffer(soundBuffers[name]);
    }
}

void SoundManager::playSound(const std::string& name) 
{
    if (sounds.find(name) != sounds.end()) 
    {
        sounds[name].setVolume(globalSoundVolume);
        sounds[name].play();
    }
}

void SoundManager::stopSound(const std::string& name)
{
    if (sounds.find(name) != sounds.end())
    {
        sounds[name].stop();
    }
}

void SoundManager::setSoundVolume(const std::string& name, float volume)
{
    if (sounds.find(name) != sounds.end()) 
    {
        sounds[name].setVolume(volume);
    }
}

void SoundManager::setGlobalSoundVolume(float volume) 
{
    globalSoundVolume = volume;
    for (auto& sound : sounds)
    {
        sound.second.setVolume(volume);
    }
}

void SoundManager::loadMusic(const std::string& name, const std::string& filename)
{
    sf::Music* music = new sf::Music();
    if (music->openFromFile(filename)) 
    {
        musics[name] = music;
    }
}

void SoundManager::playMusic(const std::string& name, bool loop)
{
    auto it = musics.find(name);
    if (it != musics.end()) 
    {
        it->second->setLoop(loop);
        it->second->play();
    }
}

void SoundManager::stopMusic(const std::string& name)
{
    if (musics.find(name) != musics.end())
    {
        musics[name]->stop();
    }
}

void SoundManager::setMusicVolume(const std::string& name, float volume)
{
    if (musics.find(name) != musics.end())
    {
        musics[name]->setVolume(volume);
    }
}

void SoundManager::setGlobalMusicVolume(float volume)
{
    globalMusicVolume = volume;
    for (auto& music : musics) 
    {
        if (music.second != nullptr) 
        {
            music.second->setVolume(volume);
        }
    }
}

void SoundManager::addToPlaylist(const std::string& name)
{
    if (musics.find(name) != musics.end())
    {
        playlist.push(name);
    }
}

void SoundManager::playNextTrack()
{
    if (!playlist.empty()) 
    {
        if (!currentTrack.empty() && musics[currentTrack]->getStatus() == sf::Music::Playing)
        {
            return;
        }

        currentTrack = playlist.front();
        playlist.pop();
        playlist.push(currentTrack); 

        playMusic(currentTrack, false);
    }
}

void SoundManager::updateMusicTrack()
{
    if (!currentTrack.empty() && musics[currentTrack]->getStatus() != sf::Music::Playing) 
    {
        playNextTrack();
    }
}

void SoundManager::stopAllMusic()
{
    for (auto& music : musics) 
    {
        music.second->stop();
    }
    currentTrack = "";
}
