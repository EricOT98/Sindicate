#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_haptic.h>
#include "ECS/Entities/Entity.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Systems/AnimationSystem.h"
#include "ECS/Systems/AiSystem.h"
#include "ECS/Systems/HealthSystem.h"
#include "ECS/Components/PositionComponent.h"
#include "ECS/Components/SpriteComponent.h"
#include "ECS/Components/AiComponent.h"
#include "ECS/Components/HealthComponent.h"
#include <tmxlite/Map.hpp>
#include "Input/InputHandler.h"
#include "Resource Manager/ResourceManager.h"
#include "ECS/Systems/ControlSystem.h"
#include <Box2D\Box2D.h>
#include "Level/LevelManager.h"
#include "Factories/Factory.h"
#include "Factories/PlayerFactory.h"
#include "Factories/EnemyFactory.h"
#include "Menu/Button.h"
#include "Menu/MainMenu.h"
#include "Menu/OptionsMenu.h"
#include "Menu/CreditsScreen.h"
#include "Menu/LevelSelectMenu.h"
#include "Utils/VectorAPI.h"
#include "Camera.h"
#include "Client/Client.h"
#include "ECS/Systems/NetworkingSystem.h"
#include <SDL_haptic.h>
#include "ECS/Systems/MovementSystem.h"
#include "ECS/Components/VelocityComponent.h"
#include "ECS/Components/TimeToLiveComponent.h"
#include "ECS/Systems/TimeToLiveSystem.h"
#include "ECS/Components/GunComponent.h"
#include "ECS/Components//AiComponent.h"
#include "AI/PlayerAiComponent.h"
#include "AI/PlayerAiSystem.h"
#include <stdlib.h>
#include <time.h>
#include <functional>
#include "Utils/ContactListener.h"
#include "Particles/Timer.h"
#include "Particles/Emitter.h"
#include "ECS/Components/ParticleEffectsComponent.h"
#include "ECS/Systems/ParticleSystem.h"
#include "Bullets/BulletManager.h"
#include "Hud.h"
#include "Menu/PauseScreen.h"
#include "Observers/LevelData.h"
#include "Observers/levelObserver.h"
#include "Menu/DeathScreen.h"
#include "Menu/ModeSelectScreen.h"
#include "Menu/LobbyScreen.h"

class ControlSystem;
class MainMenu;
class OptionsMenu;
class CreditScreen;
class LevelSelectMenu;
class PauseScreen;
class DeathScreen;
class ModeSelectScreen;
class LobbyScreen;

enum State {
	Menu,
	PlayScreen,
	Options,
	Credits,
	LevelSelect,
	Multiplayer,
	Pause,
	Dead,
	ModeSelect,
	Lobby
};

const int FRAMES_PER_SECOND = 60;

using namespace std;

class Game
{
public:
	// Public Functions
	Game();
	~Game();
	void run();
	void setGameState(State state);
	void fadeToState(State state);
	void fade();
	// Level
	LevelManager m_levelManager;

	// Resources
	ResourceManager * m_resourceManager;

	SDL_Renderer * m_renderer;

	void loadAlevel(int num);
	void reloadCurrentlevel();
	void resetPlayerHealth();
	void resetKills();

private:
	void processEvents();
	void update(const float & dt);
	void render();
	void initialiseEntities();
	void initialiseSystems();
	void initialiseFactories();
	void setUpFont();
	void quit();

	// SDL Window
	SDL_Window * p_window;
	int m_windowWidth = 1920;
	int m_windowHeight = 1080;
	bool m_quit = false;
	TTF_Font* Sans;

	// ECS Entities
	std::vector<Entity*> m_entityList;
	Entity * m_player;
	const int GUN_ENEMY_COUNT = 20;
	const int FLY_ENEMY_COUNT = 20;
	const int BIG_ENEMY_COUNT = 2;
	std::vector<Enemy *> m_gunEnemies;
	std::vector<Enemy *> m_flyEnemies;
	std::vector<Enemy *> m_bigEnemies;

	// ECS Components
	BodyComponent * m_playerBody;

	// Factories
	PlayerFactory * m_playerFactory;
	EnemyFactory * m_enemyFactory;

	// Misc
	Camera m_camera;

	// ECS Systems
	RenderSystem m_renderSystem;
	PhysicsSystem m_physicsSystem;
	ControlSystem m_controlSystem;
	AnimationSystem m_animationSystem;
	AiSystem * m_aiSystem;
	ParticleSystem * m_particleSystem;
	HealthSystem * m_healthSystem;

	// Input
	InputHandler * inputHandler;
	const int JOYSTICK_DEAD_ZONE = 16000;
	SDL_Joystick* gGameController = NULL;
	SDL_Haptic* gControllerHaptic = NULL;

	// Test - TO BE REMOVED
	SDL_Texture * texture;
	SDL_Texture * square;
	SDL_Texture * bulletTexture;
	Mix_Music *	m_testMusic;

	// Box2D
	b2Vec2 m_gravity;
	b2World m_world;
	BodyContactListener m_contactListener;

	// Scene Transitions
	SDL_Rect m_transitionScreen;
	float m_transitionAlphaPercent;
	State m_gameState;
	State m_nextState;
	bool fadeOn;
	bool fadeOff;
	bool doneFading;

	// UI/UX
	std::vector<Button *> m_buttons;
	MainMenu * m_menu;
	OptionsMenu * m_options;
	CreditScreen * m_credits;
	LevelSelectMenu * m_levelSelect;
	PauseScreen * m_pauseScreen;
	DeathScreen * m_deathScreen;
	ModeSelectScreen * m_modeSelect;
	LobbyScreen * m_lobby;

	// Networking
	NetworkingSystem m_network;

	// Bullets
	BulletManager * m_bulletManager;

	// HUD
	Hud * m_hud;

	//Observers and Subjects for level completion
	LevelData *m_levelData;
	LevelObserver *m_levelObserver;

	//Ai
	Entity* m_aiEnt;
	PlayerAiSystem* playeraiSystem;
	PlayerAiComponent* aiComponent;

	// Options
	bool vibrationOn = true;
	bool musicOn = true;

	//Background
	SDL_Texture * m_texture;
	SDL_Rect m_background;

	// Sounds
	Mix_Chunk * rifle = NULL;
	Mix_Chunk * jumpSound = NULL;
	Mix_Chunk * deadSound = NULL;
	Mix_Chunk * portal = NULL;

};
#endif // !GAME_H
