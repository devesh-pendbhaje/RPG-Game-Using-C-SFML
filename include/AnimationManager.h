#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

struct Animation {
    sf::Texture* texture;
    int maxFrames;
    float speed;
    bool loop;
};

class AnimationManager {
private:
    std::map<std::string, Animation> animations;
    std::string currentAnim;
    float timer;
    int currentFrame;
    bool finished;

public:
    AnimationManager() : timer(0.f), currentFrame(0), finished(false) {}

    void addAnimation(const std::string& name, sf::Texture* tex, int frames, float spd, bool l = true) {
        animations[name] = {tex, frames, spd, l};
    }

    void play(const std::string& name) {
        if (currentAnim != name) {
            currentAnim = name;
            timer = 0.f;
            currentFrame = 0;
            finished = false;
        }
    }

    void update(float dt, sf::Sprite& sprite, int frameWidth, int frameHeight) {
        if (currentAnim.empty() || finished) return;

        timer += dt;
        auto& anim = animations[currentAnim];

        if (timer >= anim.speed) {
            timer = 0.f;
            currentFrame++;
            if (currentFrame >= anim.maxFrames) {
                if (anim.loop) currentFrame = 0;
                else { 
                    currentFrame = anim.maxFrames - 1; 
                    finished = true; 
                }
            }
        }
        sprite.setTexture(*anim.texture);
        sprite.setTextureRect({currentFrame * frameWidth, 0, frameWidth, frameHeight});
    }

    bool isFinished() const { return finished; }
    std::string getCurrentAnimation() const { return currentAnim; }
    int getCurrentFrame() const { return currentFrame; }
};
