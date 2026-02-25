module audio;

import <stdexcept>;

AudioManager::AudioManager() {
    if (!backgroundMusic.openFromFile("assets/background.wav")) {
        throw std::runtime_error("Failed to load background music: assets/background.wav");
    }
    backgroundMusic.setLoop(true);
    backgroundMusic.setVolume(50.0f);

    if (!hitBuffer.loadFromFile("assets/hit.wav")) {
        throw std::runtime_error("Failed to load hit sound: assets/hit.wav");
    }
    if (!destroyBuffer.loadFromFile("assets/destroy.wav")) {
        throw std::runtime_error("Failed to load destroy sound: assets/destroy.wav");
    }

    hitSound.setBuffer(hitBuffer);
    destroySound.setBuffer(destroyBuffer);
}

void AudioManager::playBackgroundMusic() {
    if (backgroundMusic.getStatus() != sf::Music::Playing) {
        backgroundMusic.play();
    }
}

void AudioManager::stopBackgroundMusic() {
    backgroundMusic.stop();
}

void AudioManager::playHitSound() {
    hitSound.play();
}

void AudioManager::playDestroySound() {
    destroySound.play();
}