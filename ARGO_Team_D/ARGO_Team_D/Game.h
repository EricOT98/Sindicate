#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "ECS/Entities/Entity.h"
#include "ECS/Systems/RenderSystem.h"
#include <tmxlite/Map.hpp>
#include "InputHandler.h"
#include "Resource Manager/ResourceManager.h"
#include "ControlSystem.h"
#include "Level/Level.h"


using namespace std;

class Game
{
public:
	Game();
	~Game();
	void run();

private:
	void processEvents();
	void update();
	void render();
	void initialiseEntitys();
	void initialiseComponents();
	void initialiseSystems();
	void setUpFont();
	void quit();

	SDL_Window * p_window;
	SDL_Renderer * m_renderer;
	bool m_quit = false;

	int m_windowWidth = 1280;
	int m_windowHeight = 720;
	TTF_Font* Sans;

	RenderSystem m_renderSystem;

	InputHandler *inputHandler;

	ResourceManager* m_resourceManager;

	SDL_Texture * texture;
	Mix_Music *	m_testMusic;

	ControlSystem m_controlSystem;
	Level * level;
};
#endif // !GAME_H
