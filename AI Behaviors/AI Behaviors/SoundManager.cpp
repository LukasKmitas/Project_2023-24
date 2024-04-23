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

/// <summary>
/// Loads the sound 
/// </summary>
/// <param name="name"></param>
/// <param name="filename"></param>
void SoundManager::loadSound(const std::string& name, const std::string& filename)
{
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(filename))
    {
        soundBuffers[name] = buffer;
        sounds[name].setBuffer(soundBuffers[name]);
    }
}

/// <summary>
/// plays the sound
/// </summary>
/// <param name="name"></param>
void SoundManager::playSound(const std::string& name) 
{
    if (sounds.find(name) != sounds.end()) 
    {
        sounds[name].setVolume(globalSoundVolume);
        sounds[name].play();
    }
}

/// <summary>
/// to stop the sound playing
/// </summary>
/// <param name="name"></param>
void SoundManager::stopSound(const std::string& name)
{
    if (sounds.find(name) != sounds.end())
    {
        sounds[name].stop();
    }
}

/// <summary>
/// sets the volume
/// </summary>
/// <param name="name"></param>
/// <param name="volume"></param>
void SoundManager::setSoundVolume(const std::string& name, float volume)
{
    if (sounds.find(name) != sounds.end()) 
    {
        sounds[name].setVolume(volume);
    }
}

/// <summary>
/// stes all sounds volume
/// </summary>
/// <param name="volume"></param>
void SoundManager::setGlobalSoundVolume(float volume) 
{
    globalSoundVolume = volume;
    for (auto& sound : sounds)
    {
        sound.second.setVolume(volume);
    }
}

/// <summary>
/// Loads music
/// </summary>
/// <param name="name"></param>
/// <param name="filename"></param>
void SoundManager::loadMusic(const std::string& name, const std::string& filename)
{
    sf::Music* music = new sf::Music();
    if (music->openFromFile(filename)) 
    {
        musics[name] = music;
    }
}

/// <summary>
/// plays music
/// </summary>
/// <param name="name"></param>
/// <param name="loop"></param>
void SoundManager::playMusic(const std::string& name, bool loop)
{
    auto it = musics.find(name);
    if (it != musics.end()) 
    {
        it->second->setLoop(loop);
        it->second->play();
    }
}

/// <summary>
/// stops music
/// </summary>
/// <param name="name"></param>
void SoundManager::stopMusic(const std::string& name)
{
    if (musics.find(name) != musics.end())
    {
        musics[name]->stop();
    }
}

/// <summary>
/// sets music volume
/// </summary>
/// <param name="name"></param>
/// <param name="volume"></param>
void SoundManager::setMusicVolume(const std::string& name, float volume)
{
    if (musics.find(name) != musics.end())
    {
        musics[name]->setVolume(volume);
    }
}

/// <summary>
/// sets all music volume
/// </summary>
/// <param name="volume"></param>
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

/// <summary>
/// adds music to playlist 
/// </summary>
/// <param name="name"></param>
void SoundManager::addToPlaylist(const std::string& name)
{
    if (musics.find(name) != musics.end())
    {
        playlist.push(name);
    }
}

/// <summary>
/// plays the next track for music if there is any
/// </summary>
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

/// <summary>
/// updates the track when the current one stops playing
/// </summary>
void SoundManager::updateMusicTrack()
{
    if (!currentTrack.empty() && musics[currentTrack]->getStatus() != sf::Music::Playing) 
    {
        playNextTrack();
    }
}

/// <summary>
/// stops the music
/// </summary>
void SoundManager::stopAllMusic()
{
    for (auto& music : musics) 
    {
        music.second->stop();
    }
    currentTrack = "";
}
