#include "Game.h"
#include "ECS/Components/AnimationComponent.h"
#include "Factories/PlayerFactory.h"
#include "Factories/EnemyFactory.h"

const float WORLD_SCALE = 30.f;

Game::Game() :
	m_gravity(0, 90.81f),
	m_world(m_gravity),
	m_camera(m_windowWidth, m_windowHeight),
	m_physicsSystem(WORLD_SCALE)
{
	m_world.SetContactListener(&m_contactListener);

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "Failed to initialise SDL" << std::endl;
	}

	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;

	if (IMG_Init(imgFlags) != imgFlags)
	{
		cout << "Error: " << IMG_GetError() << endl;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		cout << "Error: " << "Audio Initalisation" << endl;
	}
	//Check for joysticks
	if (SDL_NumJoysticks() < 1)
	{
		cout << ("No Joystick connected!\n") << std::endl;
	}
	else
	{
		//Load joystick
		gGameController = SDL_JoystickOpen(0);
		if (gGameController == NULL)
		{
			cout << ("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError()) << endl;
		}
		else
		{
			// Get controller haptic device
			gControllerHaptic = SDL_HapticOpenFromJoystick(gGameController);
			if (gControllerHaptic == NULL)
			{
				printf("Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Get initialize rumble
				if (SDL_HapticRumbleInit(gControllerHaptic) < 0)
				{
					printf("Warning: Unable to initialize rumble! SDL Error: %s\n", SDL_GetError());
				}
			}

		}
	}

	p_window = SDL_CreateWindow("Argo Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowWidth, m_windowHeight, 0);
	m_renderer = SDL_CreateRenderer(p_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	m_transitionScreen.x = 0;
	m_transitionScreen.y = 0;
	m_transitionScreen.w = m_windowWidth;
	m_transitionScreen.h = m_windowHeight;

	m_transitionAlphaPercent = 0;

	fadeOff = false;
	fadeOn = false;
	doneFading = false;

	if (NULL == p_window)
	{
		std::cout << "Error: Could not create window" << std::endl;
	}

	m_resourceManager = new ResourceManager(m_renderer);

	while (!m_resourceManager->checkLoaded()) {
		cout << "Loading..." << endl;
	}

	m_music = m_resourceManager->getSoundResource("music");


	jumpSound = Mix_LoadWAV("ASSETS/SOUNDS/jump.wav");
	Mix_VolumeChunk(jumpSound, 128/4);

	deadSound = Mix_LoadWAV("ASSETS/SOUNDS/dead.wav");
	Mix_VolumeChunk(jumpSound, 128 / 4);

	m_gameState = State::Menu;
	m_menu = new MainMenu(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);
	m_options = new OptionsMenu(m_windowWidth, m_windowHeight, *this, m_renderer, p_window, vibrationOn, musicOn);
	m_credits = new CreditScreen(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);
	m_levelSelect = new LevelSelectMenu(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);
	m_pauseScreen = new PauseScreen(m_windowWidth, m_windowHeight, *this, m_renderer, p_window, m_camera);
	m_deathScreen = new DeathScreen(m_windowWidth, m_windowHeight, *this, m_renderer, p_window, m_camera);
	m_modeSelect = new ModeSelectScreen(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);
	m_lobby = new LobbyScreen(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);


	m_particleSystem = new ParticleSystem(&m_camera);

	m_levelData = new LevelData(3);
	m_levelObserver = new LevelObserver(1);
	m_levelData->registerObserver(m_levelObserver);

	m_bulletManager = new BulletManager(m_world, WORLD_SCALE, m_resourceManager);

	playeraiSystem = new PlayerAiSystem(m_bulletManager, m_levelObserver);

	initialiseFactories();
	initialiseEntities();
	initialiseSystems();
	setUpFont();

	m_controlSystem.bindJump(jumpSound);

	for (Enemy* i : m_flyEnemies) {
		playeraiSystem->addEnemy(i);
	}

	for (Enemy* i : m_bigEnemies) {
		playeraiSystem->addEnemy(i);
	}

	for (Enemy* i : m_gunEnemies) {
		playeraiSystem->addEnemy(i);
	}

	aiComponent = new PlayerAiComponent(m_player);
	m_player->addComponent(aiComponent);
	playeraiSystem->addComponent(aiComponent);


	inputHandler = new InputHandler(m_controlSystem, *gGameController, *gControllerHaptic, &m_camera);

	m_controlSystem.bindBullets(m_bulletManager);
	srand(time(NULL));

	m_levelManager.parseLevelSystem("ASSETS/LEVELS/LevelSystem.json", m_world, WORLD_SCALE, Sans, m_gunEnemies, m_flyEnemies, m_bigEnemies);

	m_hud = new Hud(m_camera, *m_renderer, p_window, *m_player, m_levelData);

	m_texture = m_resourceManager->getImageResource("MenuBackground");
	m_background.x = 0;
	m_background.y = 0;
	m_background.w = 1920;
	m_background.h = 1080;

	rifle = Mix_LoadWAV("ASSETS/SOUNDS/AssaultRifle.wav");
	Mix_VolumeChunk(rifle, 128);

	portal = Mix_LoadWAV("ASSETS/SOUNDS/teleport.wav");
	Mix_VolumeChunk(portal, 128);

	online = true;
}

Game::~Game()
{
}

void Game::run()
{
	bool exit = false;
	float timePerFrame = 1000.f / 60.f;
	Uint32 timeSinceLastUpdate = 0;
	Uint32 timeSinceStart = SDL_GetTicks();

	while (!m_quit)
	{
		processEvents();
		timeSinceLastUpdate += SDL_GetTicks() - timeSinceStart;
		timeSinceStart = SDL_GetTicks();

		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}
		render();
	}
	quit();
}

void Game::processEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (m_gameState)
		{
		case Menu:
			m_menu->handleInput(event);
			break;
		case ModeSelect:
			m_modeSelect->handleInput(event);
			break;
		case PlayScreen:
			inputHandler->handleKeyboardInput(event);
			inputHandler->handleControllerInput(event,vibrationOn);
			break;
		case Options:
			m_options->handleInput(event);
			break;
		case Credits:
			m_credits->handleInput(event);
			break;
		case LevelSelect:
			m_levelSelect->handleInput(event);
			break;
		case Pause:
			m_pauseScreen->handleInput(event);
			break;
		case Dead:
			m_deathScreen->handleInput(event);
			break;
		case Lobby:
			m_lobby->handleInput(event);
			break;
		default:
			break;
		}

		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch (m_gameState)
			{
			case PlayScreen:
				//m_camera.m_shaking = true;
				//m_gameState = State::Dead;
				break;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			switch (m_gameState)
			{
			case PlayScreen:
				//m_camera.m_shaking = true;
				break;
			}
			break;
		case SDL_KEYDOWN:

			switch (m_gameState)
			{
			case PlayScreen:
				if (event.key.keysym.sym == SDLK_TAB)
				{
					m_gameState = State::Pause;
				}
				
				break;
			}

			if (event.key.keysym.sym == SDLK_ESCAPE)
				m_quit = true;
			if (event.key.keysym.sym == SDLK_RETURN) {
				m_camera.m_shaking = true;
			}
			if (event.key.keysym.sym == SDLK_SPACE) {
				VectorAPI scale = m_camera.m_scale;
				scale += 0.1f;
				m_camera.setScale(scale);
				SDL_RenderSetScale(m_renderer, m_camera.m_scale.x, m_camera.m_scale.y);
			}
			if (event.key.keysym.sym == SDLK_BACKSPACE) {
				VectorAPI scale = m_camera.m_scale;
				scale -= 0.1f;
				m_camera.setScale(scale);
				SDL_RenderSetScale(m_renderer, m_camera.m_scale.x, m_camera.m_scale.y);
			}
			break;
		case SDL_QUIT:
			m_quit = true;
			break;


		case SDL_JOYBUTTONDOWN:
			//Play rumble at 75% strenght for 500 milliseconds
			//SDL_HapticRumblePlay(gControllerHaptic, 0.75, 500);
			switch (event.jbutton.button)
			{
			case 7:
				//cout << "A button" << endl;
				if (m_gameState == State::PlayScreen)
				{
					fadeToState(State::Pause);
				}
				break;
			}
		default:
			m_camera.m_shaking = false;
			break;
		}
	}
}

void Game::update(const float & dt)
{

	if (!musicOn)
	{
		Mix_HaltMusic();
	}
	if (Mix_PlayingMusic() == 0 && musicOn)
	{
		//Play the music
		Mix_PlayMusic(m_music, -1);
		Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	}
	if (m_gameState == State::PlayScreen)
	{
		SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		SDL_ShowCursor(SDL_ENABLE);
	}
	switch (m_gameState)
	{
	case Menu:
		m_menu->update();

		break;
	case PlayScreen:
		if (doneFading) // dont update the game unless screen is done fading
		{
			m_world.Step(1 / 60.f, 10, 5); // Update the Box2d world
			m_controlSystem.update();
			//if ai is active
			if (aiActive)
			{
				playeraiSystem->runTree();
			}
			
			m_aiSystem->update(dt);
			m_bulletManager->update(dt);
			m_physicsSystem.update();
			m_camera.update(VectorAPI(m_playerBody->getBody()->GetPosition().x * WORLD_SCALE, m_playerBody->getBody()->GetPosition().y * WORLD_SCALE), 0);
			inputHandler->update();
			m_animationSystem.update(dt / 1000);
			m_levelManager.update(dt/1000);
			if (m_levelObserver->getComplete()) {
				if (m_levelManager.checkPlayerCollisions(m_player, *m_resourceManager, WORLD_SCALE, m_renderer)) {
					if (Mix_PlayChannel(-1, portal, 0) == -1)
					{
						//return 1;
					}
					if (m_levelManager.getCurrentLevel() == 0) {
						m_levelData->reset(3); // to be changed depending on hoe many enemys we need to kill
						fadeToState(State::PlayScreen);
					}
					else if (m_levelManager.getCurrentLevel() == 1) {
						m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
						fadeToState(State::PlayScreen);
					}
					else if (m_levelManager.getCurrentLevel() == 2) {
						m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
						fadeToState(State::PlayScreen);
					}
					else if (m_levelManager.getCurrentLevel() == 3) {
						m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
						fadeToState(State::PlayScreen);
					}
				}	
			}
			m_particleSystem->update();
			m_healthSystem->update();
			m_hud->update();
			if (!m_healthSystem->playerAlive())
			{
				m_healthSystem->setPlayerAliveStatus(true);
				if (Mix_PlayChannel(-1, deadSound, 0) == -1)
				{
					//return 1;
				}
				if (m_levelManager.getCurrentLevel() == 0) {
					m_levelData->reset(3); // to be changed depending on hoe many enemys we need to kill
				}
				else if (m_levelManager.getCurrentLevel() == 1) {
					m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
				}
				else if (m_levelManager.getCurrentLevel() == 2) {
					m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
				}
				else if (m_levelManager.getCurrentLevel() == 3) {
					m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
				}
				fadeToState(State::Dead);
			}
		}
		if (m_levelManager.isGameFinished())
		{
			fadeToState(State::Menu);
			m_levelManager.setGameFinished(false);
		}
		if (online) {
			m_network.update();
		}
		break;
	case Options:
		m_options->update();
		break;
	case ModeSelect:
		m_modeSelect->update();
		break;
	case Credits:
		m_credits->update();
		break;
	case LevelSelect:
		m_levelSelect->update();
		break;
	case Multiplayer:
		break;
	case Pause:
		m_pauseScreen->update();
		m_pauseScreen->updatePositions();
		break;
	case Dead:
		m_deathScreen->update();
		m_deathScreen->updatePositions();
		break;
	case Lobby:
		m_network.update();
		if (m_network.m_inGame && !m_network.m_inGameChecked) {
			m_lobby->StartGame();
			m_network.m_inGameChecked = true;
		}
		if (!m_network.m_inGame && m_network.m_lobbiesUpdated) {
			m_lobby->updateLobbies();
		}
		m_lobby->update();
		break;
	default:
		break;
	}
}

void Game::render()
{
	const SDL_Rect bounds = m_camera.getBounds();
	if (m_renderer == nullptr)
	{
		SDL_Log("Could not create a renderer: %s", SDL_GetError());
	}

	SDL_SetRenderDrawColor(m_renderer, 0, 155, 200, 255);

	SDL_RenderClear(m_renderer);

	SDL_RenderCopy(m_renderer, m_texture, NULL, &m_background);

	switch (m_gameState)
	{
	case Menu:
		m_menu->draw();
		break;
	case PlayScreen:
		m_levelManager.render(m_renderer, m_camera);
		m_renderSystem.render(m_renderer, m_camera);
		m_particleSystem->draw();
		m_bulletManager->render(m_renderer, m_camera);
		m_hud->draw();
		break;
	case ModeSelect:
		m_modeSelect->draw();
		break;
	case Pause:
		m_renderSystem.render(m_renderer, m_camera);
		m_levelManager.render(m_renderer, m_camera);
		m_renderSystem.render(m_renderer, m_camera);
		m_particleSystem->draw();
		m_bulletManager->render(m_renderer, m_camera);
		m_pauseScreen->drawBackground();
		m_pauseScreen->draw();
		break;
	case Dead:
		m_renderSystem.render(m_renderer, m_camera);
		m_levelManager.render(m_renderer, m_camera);
		m_particleSystem->draw();
		m_bulletManager->render(m_renderer, m_camera);
		m_deathScreen->drawBackground();
		m_deathScreen->draw();
		break;
	case Options:
		m_options->draw();
		break;
	case Credits:
		m_credits->draw();
		break;
	case LevelSelect:
		m_levelSelect->draw();
		break;
	case Lobby:
		m_lobby->draw();
		break;
	default:
		break;
	}

	fade();

	SDL_RenderPresent(m_renderer);
}

void Game::quit()
{
	Mix_CloseAudio();
	TTF_Quit();
	SDL_HapticClose(gControllerHaptic);
	SDL_JoystickClose(gGameController);
	gGameController = NULL;
	gControllerHaptic = NULL;
	SDL_DestroyWindow(p_window);
	SDL_Quit();
}

void Game::setGameState(State state)
{

	if (m_gameState == State::PlayScreen)
	{
		SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		SDL_ShowCursor(SDL_ENABLE);
	}

	inputHandler->resetHandler();
	m_gameState = state;
}

void Game::fadeToState(State state)
{

	

	inputHandler->resetHandler();
	m_nextState = state;
	fadeOn = true;
	doneFading = false;
	if (m_nextState == State::PlayScreen && m_gameState == State::PlayScreen)
	{
		doneFading = true;
		//SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		SDL_ShowCursor(SDL_ENABLE);
	}
}

void Game::fade()
{
	if (fadeOn)
	{
		if (m_gameState == State::PlayScreen && m_nextState == State::PlayScreen)
		{
			m_transitionAlphaPercent += 1;
		}
		else
		{
			m_transitionAlphaPercent += 0.075;
		}
		
		if (m_transitionAlphaPercent >= 1)
		{
			m_transitionAlphaPercent = 1;
			m_gameState = m_nextState;
			fadeOff = true;
			fadeOn = false;
		}

		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255 * m_transitionAlphaPercent);
		SDL_RenderFillRect(m_renderer, &m_transitionScreen);
	}

	if (fadeOff)
	{
		
		m_transitionAlphaPercent -= 0.075;
		
		if (m_transitionAlphaPercent <= 0)
		{
			m_transitionAlphaPercent = 0;
			fadeOff = false;
			doneFading = true;
		}

		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255 * m_transitionAlphaPercent);
		SDL_RenderFillRect(m_renderer, &m_transitionScreen);
	}
}

/// <summary>
/// initialises entitys.
/// </summary>
void Game::initialiseEntities()
{
	// Init Player
	Entity * e = m_playerFactory->create("Player", VectorAPI(64, 64), VectorAPI(150, 0));
	m_entityList.push_back(e);
	m_controlSystem.addEntity(e);
	m_particleSystem->addEntity(e);
	m_animationSystem.addEntity(e);
	m_network.addEntity(e);
	m_player = e;
	m_players.push_back(e);
	for (int i = 0; i < 3; ++i) {
		Entity * e = m_playerFactory->createOnlinePlayer("Player", VectorAPI(64, 64), VectorAPI(-1500, 0));
		m_entityList.push_back(e);
		m_particleSystem->addEntity(e);
		m_animationSystem.addEntity(e);
		m_network.addEntity(e);
		m_players.push_back(e);
	}
	m_playerBody = dynamic_cast<BodyComponent*>(e->getComponentsOfType({ "Body" })["Body"]);
	playeraiSystem->addEntity(m_player);

	for(int i = 0; i < GUN_ENEMY_COUNT; ++i)
	{
		Enemy * enemy = m_enemyFactory->createGunEnemy();
		m_gunEnemies.push_back(enemy);
		m_entityList.push_back(enemy->entity);
		m_animationSystem.addEntity(enemy->entity);
		m_particleSystem->addEntity(enemy->entity);
	}
	for (int i = 0; i < FLY_ENEMY_COUNT; ++i)
	{
		Enemy * enemy = m_enemyFactory->createFlyEnemy();
		m_flyEnemies.push_back(enemy);
		m_entityList.push_back(m_flyEnemies.at(i)->entity);
		m_animationSystem.addEntity(enemy->entity);
		m_particleSystem->addEntity(enemy->entity);
	}
	for (int i = 0; i < BIG_ENEMY_COUNT; ++i)
	{
		Enemy * enemy = m_enemyFactory->createBigEnemy();
		m_bigEnemies.push_back(enemy);
		m_entityList.push_back(m_bigEnemies.at(i)->entity);
		m_animationSystem.addEntity(enemy->entity);
		m_particleSystem->addEntity(enemy->entity);
	}
}

/// <summary>
/// Adds entitys to systems.
/// </summary>
void Game::initialiseSystems()
{
	m_aiSystem = new AiSystem(m_bulletManager, m_playerBody, WORLD_SCALE, m_levelData, m_camera);
	m_healthSystem = new HealthSystem();
	for (auto e : m_entityList)
	{
		if (e->checkForComponent("Sprite"))
		{
			m_renderSystem.addEntity(e);
		}
		if (e->checkForComponent("Body"))
		{
			m_physicsSystem.addEntity(e);
		}
		if (e->checkForComponent("Ai"))
		{
			m_aiSystem->addEntity(e);
		}
		if (e->checkForComponent("Health"))
		{
			m_healthSystem->addEntity(e);
		}
	}
}

void Game::initialiseFactories()
{
	std::string spriteName = "Player";
	m_playerFactory = new PlayerFactory(m_resourceManager, m_world, WORLD_SCALE, m_renderer);
	m_enemyFactory = new EnemyFactory(m_resourceManager, m_world, WORLD_SCALE, m_renderer);
}

/// <summary>
/// Sets up font for game.
/// </summary>
void Game::setUpFont() {

	if (TTF_Init() < 0)
	{
		std::cout << "Error initialising font" << std::endl;
	}
	const char *path = "ASSETS\\FONTS\\TheBlackFestival.ttf";
	Sans = TTF_OpenFont(path, 50);
}

void Game::loadAlevel(int num)
{
	m_levelManager.unloadAllLevels();
	m_levelManager.loadLevel(m_player,*m_resourceManager, m_renderer, num);
}

void Game::reloadCurrentlevel()
{
	m_levelManager.loadLevel(m_player, *m_resourceManager, m_renderer, m_levelManager.getCurrentLevel());
}

void Game::resetPlayerHealth()
{
	m_healthSystem->reset();
}

void Game::resetKills()
{
	if (m_levelManager.getCurrentLevel() == 0) {
		m_levelData->reset(3); // to be changed depending on hoe many enemys we need to kill
	}
	else if (m_levelManager.getCurrentLevel() == 1) {
		m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
	}
	else if (m_levelManager.getCurrentLevel() == 2) {
		m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
	}
	else if (m_levelManager.getCurrentLevel() == 3) {
		m_levelData->reset(15); // to be changed depending on hoe many enemys we need to kill
	}
}

void Game::setAI(bool b)
{
	aiActive = b;
}
