#include "Application.h"

namespace Horizon
{
	Application* Application::kSingleton = nullptr;

	Application::Application()
	{
		assert(!kSingleton && "Instantiation of a singleton class must be one 'single' instance.");
		kSingleton = this;
	}

	Application::~Application()
	{
		delete mLayerStack;
		delete mSceneManager;
		delete mEditor;
	}

	void Application::init()
	{
		LogManager::init();

		// Create Window
		WindowCreateInfo windowInfo;
		windowInfo.width = 1920;
		windowInfo.height = 1080;
		windowInfo.title = "Horizon";
		windowInfo.resizable = true;
		windowInfo.mode = Horizon::WindowMode::Normal;
		mWindow = Window::create(windowInfo, HORIZON_BIND_FUNCTION(Application::onEvent), HORIZON_BIND_FUNCTION(Application::renderOneFrame));

		// Create Renderer
		mRenderer = Renderer::create(mWindow);
		mRenderer->addFrameListener(this);
		mRenderer->setRenderMode(RenderingMode::Editor);

		// Create SceneManager
		mSceneManager = new SceneManager();

		// Create Editor
		mEditor = new Editor(mWindow->getWidth(), mWindow->getHeight());

		// Create Layer Stack
		mLayerStack = new LayerStack();
		mLayerStack->pushLayer(mEditor);
		mEditor->onAttach();

		initGui();

		mActiveScene = createSampleScene();

		mEditor->mSceneContext.editorCamera = Camera::create();
		mEditor->mSceneContext.editorCamera->setPosition(0, 0, -3);
		mEditor->mSceneContext.editorCamera->lookAt(0, 0, 0);
		mEditor->mSceneContext.orbiterCameraController = OrbiterCameraController::create(mEditor->mSceneContext.editorCamera);
		mEditor->mSceneContext.firstPersonCameraController = FirstPersonCameraController::create(mEditor->mSceneContext.editorCamera);
		mEditor->mSceneContext.editorCameraControllerType = EditorCameraControllerType::Orbiter;
		mEditor->mSceneContext.sceneImageHandle = (void*)(Gui::g_DescriptorSet);
		mEditor->mSceneContext.activeScene = mActiveScene;
		mEditor->mSceneContext.sceneRenderer = mRenderer.get();
		mEditor->mSceneContext.sceneFramebuffer = mRenderer->mOffscreenFramebuffer;
	}

	void Application::initGui()
	{        
		Gui::Init();
	}

	void Application::run()
	{
		assert(mRenderer != nullptr);
		mWindow->messageLoop();
	}

	void Application::shutdown()
	{
		mRenderer->shutdown();
		mSceneManager->destroyAllAssets();
		for (auto it = mLayerStack->begin(); it != mLayerStack->end(); ++it)
		{
			(*it)->onDetach();
		}
	}

	void Application::renderOneFrame()
	{
		mRenderer->renderOneFrame();
	}

	bool Application::onWindowClose(const WindowCloseEvent& e)
	{
		mWindow->shutdown();
		return true;
	}

	bool Application::onWindowResize(const WindowResizeEvent& e)
	{
		if (!mRenderer) return false;
		// Get the current window client area size
		uint32 width = mWindow->getWidth();
		uint32 height = mWindow->getHeight();
		// Tell the renderer
		mRenderer->onWindowResize(e.getWidth(), e.getHeight());
		// Tell the GUI
		Gui::onWindowResize(width, height);
		return true;
	}

	Scene* Application::createSampleScene()
	{
		auto sampleScene = mSceneManager->createScene("Sample Scene");

		auto entityManager = sampleScene->getEntityManager();

		auto mesh1 = Mesh::loadFromOBJ(String(HORIZON_ASSETS_PATH) + "armored_knight/armor.obj");
		auto mesh2 = Mesh::loadFromOBJ(String(HORIZON_ASSETS_PATH) + "floor/deferred_floor.obj");

		StaticMeshComponent staticMeshComponent;
		staticMeshComponent.mesh = mesh1;
		staticMeshComponent.materials = mesh1->getMaterials();

		Entity* armor1 = entityManager->createEntity("Armored Knight 1");
		armor1->addComponent<StaticMeshComponent>(staticMeshComponent);
		armor1->addComponent<TransformComponent>();

		TransformComponent transformComponent;
		transformComponent.position = Vector3(-5.0f, 0.0f, -5.0f);

		Entity* armor2 = entityManager->createEntity("Armored Knight 2");
		armor2->addComponent<StaticMeshComponent>(staticMeshComponent);
		armor2->addComponent<TransformComponent>(transformComponent);

		transformComponent.position = Vector3(5.0f, 0.0f, -5.0f);

		Entity* armor3 = entityManager->createEntity("Armored Knight 3");
		armor3->addComponent<StaticMeshComponent>(staticMeshComponent);
		armor3->addComponent<TransformComponent>(transformComponent);

		staticMeshComponent.mesh = mesh2;
		staticMeshComponent.materials = mesh2->getMaterials();

		Entity* floor = entityManager->createEntity("Floor");
		floor->addComponent<StaticMeshComponent>(staticMeshComponent);
		floor->addComponent<TransformComponent>();

		LightComponent lightComponent;
		lightComponent.type = LightType::Point;
		lightComponent.color = Vector4(1.5f);
		lightComponent.range = 15.0f * 0.25f;
		mLights[0] = entityManager->createEntity("White Light");
		mLights[0]->addComponent<TransformComponent>();
		mLights[0]->addComponent<LightComponent>(lightComponent);

		lightComponent.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		lightComponent.range = 15.0f;
		mLights[1] = entityManager->createEntity("Red Light");
		mLights[1]->addComponent<TransformComponent>();
		mLights[1]->addComponent<LightComponent>(lightComponent);

		lightComponent.color = Vector4(0.0f, 0.0f, 2.5f, 1.0f);
		lightComponent.range = 5.0f;
		mLights[2] = entityManager->createEntity("Blue Light");
		mLights[2]->addComponent<TransformComponent>();
		mLights[2]->addComponent<LightComponent>(lightComponent);

		lightComponent.color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
		lightComponent.range = 2.0f;
		mLights[3] = entityManager->createEntity("Yellow Light 1");
		mLights[3]->addComponent<TransformComponent>();
		mLights[3]->addComponent<LightComponent>(lightComponent);

		lightComponent.color = Vector4(0.0f, 1.0f, 0.2f, 1.0f);
		lightComponent.range = 5.0f;
		mLights[4] = entityManager->createEntity("Green Light");
		mLights[4]->addComponent<TransformComponent>();
		mLights[4]->addComponent<LightComponent>(lightComponent);

		lightComponent.color = Vector4(1.0f, 0.7f, 0.3f, 1.0f);
		lightComponent.range = 25.0f;
		mLights[5] = entityManager->createEntity("Yellow Light 2");
		mLights[5]->addComponent<TransformComponent>();
		mLights[5]->addComponent<LightComponent>(lightComponent);

		return sampleScene;
	}


	void Application::update(Timestep timestep)
	{
		static float timer = 0.0f;
		const static float speedModifier = 0.25;
		timer += speedModifier * timestep.seconds();
		if (timer > 1.0f)
			timer -= 1.0f;

		uint32 index = 0;
		for (uint32 index = 0; index < 6; index++)
		{
			auto& transformComponent = mLights[index]->getComponent<TransformComponent>();
			if (index == 0)
			{
				transformComponent.position.x = sin(glm::radians(360.0f * timer)) * 5.0f;
				transformComponent.position.y = 0.0f;
				transformComponent.position.z = cos(glm::radians(360.0f * timer)) * 5.0f;
			}
			else if (index == 1)
			{
				transformComponent.position.x = -4.0f + sin(glm::radians(360.0f * timer) + 45.0f) * 2.0f;
				transformComponent.position.y = 0.0f;
				transformComponent.position.z = 0.0f + cos(glm::radians(360.0f * timer) + 45.0f) * 2.0f;
			}
			else if (index == 2)
			{
				transformComponent.position.x = 4.0f + sin(glm::radians(360.0f * timer)) * 2.0f;
				transformComponent.position.y = -1.0f;
				transformComponent.position.z = 0.0f + cos(glm::radians(360.0f * timer)) * 2.0f;
			}
			else if (index == 3)
			{
				transformComponent.position.x = 0.0f;
				transformComponent.position.y = -0.9f;
				transformComponent.position.z = 0.5f;
			}
			else if (index == 4)
			{
				transformComponent.position.x = 0.0f + sin(glm::radians(360.0f * timer + 90.0f)) * 5.0f;
				transformComponent.position.y = -0.5f;
				transformComponent.position.z = 0.0f - cos(glm::radians(360.0f * timer + 45.0f)) * 5.0f;
			}
			else if (index == 5)
			{
				transformComponent.position.x = 0.0f + sin(glm::radians(-360.0f * timer + 135.0f)) * 10.0f;
				transformComponent.position.y = -1.0f;
				transformComponent.position.z = 0.0f - cos(glm::radians(-360.0f * timer - 45.0f)) * 10.0f;
			}
		}

		for (auto it = mLayerStack->begin(); it != mLayerStack->end(); ++it)
		{
			(*it)->onUpdate(timestep);
		}
	}

	void Application::frameStarted()
	{
		static float lastTime = 0.0f;
		float currentTime = (float)glfwGetTime();
		Timestep timestep = currentTime - lastTime;
		lastTime = currentTime;
		update(timestep);

		mRenderer->beginScene(mEditor->mSceneContext.activeScene, mEditor->mSceneContext.editorCamera.get());
	}

	void Application::frameCommandsQueued()
	{
		Gui::StartFrame();
		{
			for (auto it = mLayerStack->begin(); it != mLayerStack->end(); ++it)
			{
				(*it)->onImGuiRender();
			}
		}
		Gui::Rendering();
	}

	void Application::frameEnded()
	{
		mRenderer->endScene();
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(HORIZON_BIND_FUNCTION(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(HORIZON_BIND_FUNCTION(Application::onWindowResize));
		for (auto it = mLayerStack->rbegin(); it != mLayerStack->rend(); ++it)
		{
			if (e.mHandled)
			{
				break;
			}
			(*it)->onEvent(e);
		}
	}
}