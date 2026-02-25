export module audio;

import <SFML/Audio.hpp>;
import <filesystem>;
import <stdexcept>;
import <string>;

export class AudioManager {
    sf::Music backgroundMusic;
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer destroyBuffer;
    sf::Sound hitSound;
    sf::Sound destroySound;

public:
    AudioManager() {
        // Ładowanie muzyki tła
        if (!backgroundMusic.openFromFile("assets/background.wav")) {
            throw std::runtime_error("Failed to load background music: assets/background.wav");

        }
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50.0f);

        // Ładowanie efektów dźwiękowych
        if (!hitBuffer.loadFromFile("assets/hit.wav")) {
            throw std::runtime_error("Failed to load hit sound: assets/hit.wav");
        }
        if (!destroyBuffer.loadFromFile("assets/destroy.wav")) {
            throw std::runtime_error("Failed to load destroy sound: assets/destroy.wav");
        }

        hitSound.setBuffer(hitBuffer);
        destroySound.setBuffer(destroyBuffer);
    }

    void playBackgroundMusic() {
        if (backgroundMusic.getStatus() != sf::Music::Playing) {
            backgroundMusic.play();
        }
    }

    void stopBackgroundMusic() {
        backgroundMusic.stop();
    }

    void playHitSound() {
        hitSound.play();
    }

    void playDestroySound() {
        destroySound.play();
    }
};