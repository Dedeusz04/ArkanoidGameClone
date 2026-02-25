export module audio;

import <SFML/Audio.hpp>;
import <string>;

export class AudioManager {
public:
    AudioManager();
    void playBackgroundMusic();
    void stopBackgroundMusic();
    void playHitSound();
    void playDestroySound();
private:
    sf::Music backgroundMusic;
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer destroyBuffer;
    sf::Sound hitSound;
    sf::Sound destroySound;
};