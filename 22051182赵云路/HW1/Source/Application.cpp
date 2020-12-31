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

	}

	void Application::init()
	{
		// Create Window
		WindowCreateInfo windowInfo;
		windowInfo.width = 1280;
		windowInfo.height = 720;
		windowInfo.title = "Horizon";
		windowInfo.resizable = true;
		windowInfo.mode = WindowMode::Normal;
		mWindow = Window::create(windowInfo, HORIZON_BIND_FUNCTION(Application::onEvent), HORIZON_BIND_FUNCTION(Application::renderOneFrame));

		// Create Renderer
		mRenderer = Renderer::create(mWindow);
		mRenderer->addFrameListener(this);

		// Create SceneManager
		mSceneManager = SceneManager::create();

		// Create Scene
		mActiveScene = createSampleScene();

		// Create Camera
		mMainCamera = Camera::create();
		mMainCamera->setPosition(0, 0, 10);
		mMainCamera->lookAt(0, 0, 0);

		// Create CameraControllers
		mOrbiterCameraController = OrbiterCameraController::create(mMainCamera);
		mFirstPersonCameraController = FirstPersonCameraController::create(mMainCamera);
		mCameraControllerType = CameraControllerType::Orbiter;
	}

	Scene* Application::createSampleScene()
	{	
		// Scene* scene = mSceneManager->loadSceneFromGltf(String(HORIZON_ASSET_PATH) + "glTF/2.0/Cube/glTF/Cube.gltf");
		// Scene* scene = mSceneManager->loadSceneFromGltf(String(HORIZON_ASSET_PATH) + "sphere.gltf");
		// Scene* scene = mSceneManager->loadSceneFromGltf(String(HORIZON_ASSET_PATH) + "color_teapot_spheres.gltf");
		Scene* scene = mSceneManager->loadSceneFromGltf(String(HORIZON_ASSET_PATH) + "teapot.gltf");
		scene->setName("Sample Scene");

		// Add and modify entities here


		// Create Skybox
		// This function is only used for the convenience of testing and may be changed at any time.
		scene->createDefaultSkyBox();
		return scene;
	}

	void Application::run()
	{
		assert(mRenderer != nullptr);
		mWindow->messageLoop();
	}

	void Application::shutdown()
	{
		mRenderer->shutdown();
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
		mRenderer->onWindowResize(e.getWidth(), e.getHeight());
		mMainCamera->setAspectRatio((float)width / height);
		return true;
	}

	bool Application::onKeyPress(const KeyboardEvent& e)
	{
		bool handled = false;
		switch (e.getKeyID())
		{
		case KeyID::T:
		{
			mCameraControllerType = (CameraControllerType)(((int)mCameraControllerType + 1) % 2);
			handled = true;
			break;
		}
		case KeyID::Esc:
		{
			mWindow->shutdown();
			handled = true;
			break;
		}
		default:
			break;
		}
		return handled;
	}

	void Application::frameStarted()
	{
		switch (mCameraControllerType)
		{
		case CameraControllerType::Orbiter:
		{
			mOrbiterCameraController->update();
			break;
		}
		case CameraControllerType::FirstPerson:
		{
			mFirstPersonCameraController->update();
			break;
		}
		default:
			break;
		}

		mRenderer->beginScene(mActiveScene, mMainCamera.get());
	}

	void Application::frameCommandsQueued()
	{

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
		dispatcher.dispatch<KeyPressEvent>(HORIZON_BIND_FUNCTION(Application::onKeyPress));
		if (!e.isHandled())
		{
			switch (mCameraControllerType)
			{
			case CameraControllerType::Orbiter:
			{
				mOrbiterCameraController->onEvent(e);
				break;
			}
			case CameraControllerType::FirstPerson:
			{
				mFirstPersonCameraController->onEvent(e);
				break;
			}
			default:
				break;
			}
		}
	}
}