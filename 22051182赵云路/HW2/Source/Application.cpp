#include "Application.h"
#include "LogManager.h"
#include "UI/Gui.h"

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
		mRenderer->setRenderMode(RenderingMode::Offscreen);

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
		auto scene = mSceneManager->loadSceneFromGltf(String(HORIZON_ASSETS_PATH) + "glTF/sun_earth_moon.gltf");
		scene->setName("Sample Scene");

		auto entityManager = scene->getEntityManager();

		// Create point light
		auto light = entityManager->createEntity("Point Light");
		light->addComponent<TransformComponent>();
		LightComponent lightComponent;
		lightComponent.color = Vector4(1, 0, 0, 1);
		lightComponent.type = (uint32)LightType::Point;
		lightComponent.intensity = 1.0f;
		lightComponent.range = 100.0f;
		light->addComponent<LightComponent>(lightComponent);

		// Create Skybox
		// This function is only used for the convenience of testing and may be changed at any time.
		scene->createDefaultSkyBox();

		return scene;
	}

	void Application::frameStarted()
	{
		// Update scene
		
		auto entityManager = mActiveScene->getEntityManager();
		float scale = 1.0f / 30000.f;

		// ̫Sun
		static float sunSpinAngle = 0.0f;
		sunSpinAngle += 360.0f / 25.0f * scale;
		auto sun = entityManager->getEntityByHandle((EntityHandle)0);
		auto& sunTransform = sun->getComponent<TransformComponent>();
		sunTransform.rotation = glm::angleAxis(sunSpinAngle, Vector3(0, 1, 0));

		// Earth 
		float rEarth = 50.0f;
		static float earthOrbitalAngle = 0.0f;
		earthOrbitalAngle += 360.0f / 365.0f * scale;
		static float earthSpinAngle = 0.0f;
		earthSpinAngle += 360.0f * scale;
	
		auto earth = entityManager->getEntityByHandle((EntityHandle)1);
		auto& earthTransform = earth->getComponent<TransformComponent>();
		earthTransform.position = Vector3(rEarth * std::cos(earthOrbitalAngle), 0, rEarth * std::sin(earthOrbitalAngle));
		earthTransform.rotation = glm::angleAxis(earthSpinAngle, Vector3(0, 1, 0));

		// Moon
		float rMoon = 6.0f;
		static float moonOrbitalAngle = 0.0f;
		moonOrbitalAngle += 360.0f / 27.32f * scale;
		static float moonSpinAngle = 0.0f;
		moonSpinAngle += 360.0f / 27.32f * scale;
		auto moon = entityManager->getEntityByHandle((EntityHandle)2);
		auto& moonTransform = moon->getComponent<TransformComponent>();
		moonTransform.position = earthTransform.position + Vector3(rMoon * std::cos(moonOrbitalAngle), 0, rMoon * std::sin(moonOrbitalAngle));
		moonTransform.rotation = glm::angleAxis(moonSpinAngle, Vector3(0, 1, 0));

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
			if (e.isHandled())
			{
				break;
			}
			(*it)->onEvent(e);
		}
	}
}