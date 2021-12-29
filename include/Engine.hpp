#pragma once

// SFML
#include <SFML/System/NonCopyable.hpp> // sf::NonCopyable

// Forward declarations
#include <sol/forward.hpp>

// Standard library
#include <memory> // std::unique_ptr, std::make_unique
#include <map> // std::map
#include <string> // std::string
#include <vector> // std::vector


// Forward declarations
namespace sf
{
    class RenderWindow;
    class Time;
    class Texture;
    class Sprite;
} // namespace sf


class Engine : private sf::NonCopyable
{
    public:
        Engine();
        ~Engine();

        void init();
        void run();
        

    private:
        // Resource and sprite API (should be in a separate classes)
        void HAPI_loadTexture(const std::string &_textureID, 
            const std::string &_filepath);
        void HAPI_createSprite(const std::string &_spriteID, 
            const std::string &_textureID);
        void HAPI_moveSprite(const std::string &_spriteID, float x, float y);
        void HAPI_setSpritePosition(const std::string &_spriteID, float x, float y);
        void HAPI_setSpriteRect(const std::string &_spriteID, 
            int rectLeft, int rectTop, int rectWidth, int rectHeight);
        void HAPI_drawSprite(const std::string &_spriteID, float x, float y);
        void HAPI_windowClear();
        void HAPI_windowOnDispay();
        int HAPI_randInt(int inclusiveMin, int inclusiveMax) const;

        // Initialization
        void initWindow();
        void initLua();

        // Resource Management (should be in a separate class)
        const sf::Sprite &getSprite(const std::string &_spriteID) const;
        sf::Sprite &getSprite(const std::string &_spriteID);
        const sf::Texture &getTexture(const std::string &_textureID) const;
        sf::Texture &getTexture(const std::string &_textureID);

        // Main loop
        void handleEvents();
        void update(sf::Time dt);
        void render();


    private:
        bool m_Initialized_;
        // Clients' resources
        std::unique_ptr<sf::RenderWindow> m_Window_;

        // Lua Virtual Machine
        std::unique_ptr<sol::state> m_VM_;

        // Resources
        std::map<std::string, std::unique_ptr<sf::Sprite>> m_Sprites_;
        std::map<std::string, std::shared_ptr<sf::Texture>> m_Textures_;
};
