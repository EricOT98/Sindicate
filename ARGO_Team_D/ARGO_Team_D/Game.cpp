#include "Game.h"
#include "ECS/Components/AnimationComponent.h"

const float WORLD_SCALE = 30.f;

Game::Game() :
	m_gravity(0, 90.81f),
	m_world(m_gravity),
	m_camera(m_windowWidth, m_windowHeight),
	m_physicsSystem(WORLD_SCALE)
{
	m_network = NetworkingSystem();
	m_network.initClientLocalClient();

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

	//m_testMusic = m_resourceManager->getSoundResource("test");
	//if (Mix_PlayMusic(m_testMusic, -1) == -1)
	//{
	//}

	m_gameState = State::Menu;
	m_menu = new MainMenu(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);
	m_options = new OptionsMenu(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);
	m_credits = new CreditScreen(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);
	m_levelSelect = new LevelSelectMenu(m_windowWidth, m_windowHeight, *this, m_renderer, p_window);

	initialiseFactories();
	initialiseEntities();
	initialiseSystems();
	setUpFont();

	inputHandler = new InputHandler(m_controlSystem, *gGameController, *gControllerHaptic);

	for (int i = 0; i < 1; i++)
	{
		Entity * e = new Entity();
		e->addComponent(new PositionComponent(-10000, -10000));
		std::string theName = "bullet";
		e->addComponent(new SpriteComponent(theName, *m_resourceManager, 30, 5));
		e->addComponent(new VelocityComponent(VectorAPI(0, 0)));
		e->addComponent(new TimeToLiveComponent(1.0f));
		m_movementSystem.addEntity(e);
		m_renderSystem.addEntity(e);
		m_ttlSystem.addEntity(e);
		m_bullets.push_back(e);
	}
	m_controlSystem.bindBullets(m_bullets);
	srand(time(NULL));
	m_levelManager.parseLevelSystem("ASSETS/LEVELS/LevelSystem.json", m_world, WORLD_SCALE, Sans, m_gunEnemies, m_flyEnemies, m_bigEnemies);

	//float enemyX = 100;
	//float enemyY = 100;
	//float enemyWidth = 100;
	//float enemyHeight = 100;
	//std::string name = "TestAnimation";
	//Entity * enemy = new Entity();
	//enemy->addComponent(new BodyComponent(enemyX, enemyY, enemyWidth, enemyHeight, m_world, WORLD_SCALE));
	//enemy->addComponent(new PositionComponent(enemyX, enemyY));
	//enemy->addComponent(new SpriteComponent(name, *m_resourceManager, enemyX, enemyY));
	////enemy->addComponent(new AnimationComponent());
	//enemy->addComponent(new AiComponent(AiType::EnemyGun, 0, 200));
	//m_renderSystem.addEntity(enemy);
	//m_physicsSystem.addEntity(enemy);
	////m_animationSystem.addEntity(enemy);
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
			update(timeSinceLastUpdate);
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
		case PlayScreen:
			inputHandler->handleKeyboardInput(event);
			inputHandler->handleControllerInput(event);
			m_controlSystem.processInput(event);
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
		default:
			break;
		}

		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch (m_gameState)
			{
			case PlayScreen:
				m_camera.m_shaking = true;
				break;
			}


		case SDL_KEYDOWN:
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
		default:
			m_camera.m_shaking = false;
			break;
		}
	}
}

void Game::update(const float & dt)
{
	if (!m_network.getHost())
	{
		m_network.updateFromHost();
	}
	else {
		m_network.updateClients();
	}

	switch (m_gameState)
	{
	case Menu:
		m_menu->update();
		break;
	case PlayScreen:
		if (doneFading) // dont update the game unless screen is done fading
		{
			m_controlSystem.update();
			m_aiSystem->update();
			m_world.Step(1 / 60.f, 10, 5); // Update the Box2d world
			m_physicsSystem.update();
			m_camera.update(VectorAPI(m_playerBody->getBody()->GetPosition().x * WORLD_SCALE, m_playerBody->getBody()->GetPosition().y * WORLD_SCALE), 0);
			m_movementSystem.update();
			m_ttlSystem.update();
			inputHandler->update();
			m_animationSystem.update(dt / 1000);
			m_levelManager.update(dt/1000);
			m_levelManager.checkPlayerCollisions(m_player, *m_resourceManager, WORLD_SCALE, m_renderer);
		}
		break;
	case Options:
		m_options->update();
		break;
	case Credits:
		m_credits->update();
		break;
	case LevelSelect:
		m_levelSelect->update();
		break;
	case Multiplayer:
		m_network.updateFromHost();
		break;
	default:
		break;
	}

	if (!m_network.getHost())
	{
		m_network.sendToHost();
	}
	else {
		m_network.sendToClients();
	}
}

void Game::render()
{
	const SDL_Rect bounds = m_camera.getBounds();
	if (m_renderer == nullptr)
	{
		SDL_Log("Could not create a renderer: %s", SDL_GetError());
	}

	SDL_SetRenderDrawColor(m_renderer, 0, 255, 255, 255);

	SDL_RenderClear(m_renderer);

	switch (m_gameState)
	{
	case Menu:
		m_menu->draw();
		break;
	case PlayScreen:
		m_renderSystem.render(m_renderer, m_camera);
		m_levelManager.render(m_renderer, m_camera);
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
	m_gameState = state;
}

void Game::fadeToState(State state)
{
	m_nextState = state;
	fadeOn = true;
	doneFading = false;
}

void Game::fade()
{
	if (fadeOn)
	{
		m_transitionAlphaPercent += 0.075;
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
	Entity * e = m_playerFactory->create(VectorAPI(150, 0));
	m_entityList.push_back(e);
	m_controlSystem.addEntity(e);
	m_player = e;
	m_playerBody = dynamic_cast<BodyComponent*>(e->getComponentsOfType({ "Body" })["Body"]);
	for(int i = 0; i < GUN_ENEMY_COUNT; ++i)
	{
		Enemy * enemy = m_enemyFactory->createGunEnemy();
		m_gunEnemies.push_back(enemy);
		m_entityList.push_back(enemy->entity);
	}
	for (int i = 0; i < FLY_ENEMY_COUNT; ++i)
	{
		//Enemy * enemy = m_enemyFactory->createFlyEnemy();
		//m_flyEnemies.push_back(enemy);
		////m_entityList.push_back(m_flyEnemies.at(i)->entity);
	}
	for (int i = 0; i < BIG_ENEMY_COUNT; ++i)
	{
		//Enemy * enemy = m_enemyFactory->createBigEnemy();
		//m_bigEnemies.push_back(enemy);
		////m_entityList.push_back(m_bigEnemies.at(i)->entity);
	}
}

/// <summary>
/// Adds entitys to systems.
/// </summary>
void Game::initialiseSystems()
{
	m_aiSystem = new AiSystem(m_playerBody);
	for (auto i : m_entityList)
	{
		if (i->checkForComponent("Sprite"))
		{
			m_renderSystem.addEntity(i);
		}
		if (i->checkForComponent("Body"))
		{
			m_physicsSystem.addEntity(i);
		}
		if (i->checkForComponent("Ai"))
		{
			m_aiSystem->addEntity(i);
		}
	}
}

void Game::initialiseFactories()
{
	std::string spriteName = "test";
	m_playerFactory = new PlayerFactory(spriteName, VectorAPI(64, 64), m_resourceManager, m_world, WORLD_SCALE);
	m_enemyFactory = new EnemyFactory(m_resourceManager, m_world, WORLD_SCALE);
}

/// <summary>
/// Sets up font for game.
/// </summary>
void Game::setUpFont() {

	if (TTF_Init() < 0)
	{
		std::cout << "error error error" << std::endl;
	}
	const char *path = "ASSETS\\FONTS\\arial.ttf";
	Sans = TTF_OpenFont(path, 50);
}

void Game::spawnProjectile(float x, float y)
{
	for (auto &b : m_bullets)
	{

		std::vector<std::string> s = { "TimeToLive", "Position", "Velocity" };
		auto comps = b->getComponentsOfType(s);
		TimeToLiveComponent * t = dynamic_cast<TimeToLiveComponent*>(comps["TimeToLive"]);

		if (!t->isActive())
		{
			t->setActive(true);
			PositionComponent * p = dynamic_cast<PositionComponent*>(comps["Position"]);
			p->setPosition(VectorAPI(x, y));

			VelocityComponent * v = dynamic_cast<VelocityComponent*>(comps["Velocity"]);
			v->setVelocity(VectorAPI(30, ((double)rand() / RAND_MAX) * 2 - 1));
			t->setTimer(SDL_GetTicks());
			t->setActive(true);
			break;
		}

	}
}
