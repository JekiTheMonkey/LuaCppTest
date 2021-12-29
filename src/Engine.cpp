// Simulation
#include "Engine.hpp"

// sol
#include <sol/sol.hpp>

// SFML
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <random>
#include <ctime>

namespace
{
	std::default_random_engine createRandomEngine()
	{
		auto seed = static_cast<unsigned long>(std::time(nullptr));
		return std::default_random_engine(seed);
	}

	auto RandomEngine = createRandomEngine();
}


Engine::Engine()
: m_Initialized_(false)
, m_Window_(nullptr)
, m_VM_(nullptr)
, m_Sprites_()
, m_Textures_()
{
}

Engine::~Engine() = default;

void Engine::init()
{
    m_Initialized_ = true;

    // sf::RenderWindow has to be wrapped into another class Window which
    // initWindow();
    initLua();
}

void Engine::run()
{
    if (!m_Initialized_)
        throw std::logic_error("run - Engine is not initialized.");
    
    const std::string mainScript = "../Scripts/main.lua";
    const auto loadResult = m_VM_->safe_script_file(mainScript);

    if (!loadResult.valid())
    {
        const auto errObj = loadResult.get<sol::error>();
        throw std::runtime_error(errObj.what());
    }

    initWindow();

    const auto mainInitFun = (*m_VM_)["Init"].get<sol::protected_function>();
    const auto mainInitResult = mainInitFun.call();
    if (!mainInitResult.valid())
    {
        const auto errObj = mainInitResult.get<sol::error>();
        throw std::runtime_error(errObj.what());
    }


    const sf::Time timePerFrame = sf::seconds(1.f / 15.f);

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (m_Window_->isOpen()) 
    {
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate >= timePerFrame) 
        {
            timeSinceLastUpdate -= timePerFrame;

            handleEvents();
            update(timePerFrame);
        }

        render();
    }
}

void Engine::HAPI_loadTexture(const std::string &_textureID, 
    const std::string &_filepath)
{
    // Create some space on heap to store the texture
    auto texture = std::make_shared<sf::Texture>();

    // Try to load the resource, throw an exception if failed
    if (!texture->loadFromFile(_filepath))
        throw std::runtime_error("HAPI_loadTexture - \
            Failed to load texture \"" + _filepath + "\"");

    // Try to insert the texture in the container
    auto inserted = m_Textures_.insert(std::make_pair(_textureID, texture));

    // Control insertion result
    if (!inserted.second)
        throw std::logic_error("HAPI_loadTexture - \
            Failed to insert texture \"" + _textureID + "\"");
}

void Engine::HAPI_createSprite(const std::string &_spriteID, 
    const std::string &_textureID)
{
    // Create some space on heap to store the sprite, and initialize it
    auto sprite = std::make_unique<sf::Sprite>(getTexture(_textureID));

    // Try to insert the sprite in the container
    auto inserted = m_Sprites_.insert(std::make_pair(_spriteID, std::move(sprite)));

    // Control insertion result
    if (!inserted.second)
        throw std::logic_error("HAPI_createSprite - \
            Failed to insert sprite \"" + _spriteID + "\"");
}

void Engine::HAPI_moveSprite(
    const std::string &_spriteID, float x, float y)
{
    getSprite(_spriteID).move(x, y);
}

void Engine::HAPI_setSpritePosition(
    const std::string &_spriteID, float x, float y)
{
    getSprite(_spriteID).setPosition(x, y);
}

void Engine::HAPI_setSpriteRect(const std::string &_spriteID, 
    int rectLeft, int rectTop, int rectWidth, int rectHeight)
{
    auto &sprite = getSprite(_spriteID);
    sprite.setTextureRect(sf::IntRect
        (rectLeft, rectTop, rectWidth, rectHeight));
    sprite.setOrigin(
        static_cast<float>(rectWidth)  / 2.f, 
        static_cast<float>(rectHeight) / 2.f);
}

void Engine::HAPI_drawSprite(const std::string &_spriteID, float x, float y)
{
    auto &sprite = getSprite(_spriteID);
    sprite.setPosition(x, y);
    m_Window_->draw(sprite);
}

void Engine::HAPI_windowClear()
{
    m_Window_->clear();
}

void Engine::HAPI_windowOnDispay()
{
    m_Window_->display();
}

// It shouldn't even be there :P
int Engine::HAPI_randInt(int inclusiveMin, int inclusiveMax) const
{
	std::uniform_real_distribution<> distr(inclusiveMin, inclusiveMax);
	return static_cast<int>(distr(RandomEngine));
}

void Engine::initWindow()
{
    // Initialize new window
    auto window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(1024u, 768u), "Sample", sf::Style::Default);

    // Swap 2 unique pointers
    m_Window_.swap(window);
}

void Engine::initLua()
{
    // Initialize LVM
    m_VM_ = std::make_unique<sol::state>();

    // Create the API
    m_VM_->set_function("LoadTexture", &Engine::HAPI_loadTexture, this);
    m_VM_->set_function("CreateSprite", &Engine::HAPI_createSprite, this);
    m_VM_->set_function("MoveSprite", &Engine::HAPI_moveSprite, this);
    m_VM_->set_function("SetSpritePosition", &Engine::HAPI_setSpritePosition, this);
    m_VM_->set_function("SetSpriteRect", &Engine::HAPI_setSpriteRect, this);
    m_VM_->set_function("DrawSprite", &Engine::HAPI_drawSprite, this);
    m_VM_->set_function("ClearWindow", &Engine::HAPI_windowClear, this);
    m_VM_->set_function("DisplayOnWindow", &Engine::HAPI_windowOnDispay, this);
    m_VM_->set_function("RandInt", &Engine::HAPI_randInt, this);
    m_VM_->set_function("PrintCpp", 
        sol::overload(
            [] (int n)                  { std::cout << n   << std::endl; }, 
            [] (const std::string &str) { std::cout << str << std::endl; }));
}

const sf::Sprite &Engine::getSprite(const std::string &_spriteID) const
{
    const auto &found = m_Sprites_.find(_spriteID);

    if (found == m_Sprites_.end())
        throw std::logic_error("getSprite - \
            Failed to find sprite \"" + _spriteID + "\"");

    return *found->second;
}

sf::Sprite &Engine::getSprite(const std::string &_spriteID)
{
    return const_cast<sf::Sprite&>(
        static_cast<const Engine&>(*this).getSprite(_spriteID));
}

const sf::Texture &Engine::getTexture(const std::string &_textureID) const
{
    const auto &found = m_Textures_.find(_textureID);

    if (found == m_Textures_.end())
        throw std::logic_error("getTexture - \
            Failed to find texture \"" + _textureID + "\"");

    return *found->second;
}

sf::Texture &Engine::getTexture(const std::string &_textureID)
{
    return const_cast<sf::Texture&>(
        static_cast<const Engine&>(*this).getTexture(_textureID));
}

void Engine::handleEvents()
{
    sf::Event event;
    while (m_Window_->pollEvent(event)) 
    {
        switch (event.type) 
        {
            case sf::Event::Closed:
                m_Window_->close();
                break;

            default:
                break;
        }
    }
}

void Engine::update(sf::Time)
{
    const auto mainUpdateFun = (*m_VM_)["Update"].get<sol::protected_function>();
    const auto mainUpdateResult = mainUpdateFun.call();
    if (!mainUpdateResult.valid())
    {
        const auto errObj = mainUpdateResult.get<sol::error>();
        throw std::runtime_error(errObj.what());
    }
}

void Engine::render()
{
    const auto mainRenderFun = (*m_VM_)["Render"].get<sol::protected_function>();
    const auto mainRenderResult = mainRenderFun.call();
    if (!mainRenderResult.valid())
    {
        const auto errObj = mainRenderResult.get<sol::error>();
        throw std::runtime_error(errObj.what());
    }
}