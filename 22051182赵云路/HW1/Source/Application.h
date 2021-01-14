#pragma once

#include "Horizon.h"

namespace Horizon
{
	enum class CameraControllerType
	{
		Orbiter = 0,
		FirstPerson = 1,
		Undefined
	};

	class Application : public FrameListener
	{
	public:
		Application();
		~Application();
		void init();
		void run();
		void shutdown();
		void renderOneFrame();
		void onEvent(Event& e);
		bool onWindowClose(const WindowCloseEvent& e);
		bool onWindowResize(const WindowResizeEvent& e);
		bool onKeyPress(const KeyboardEvent& e);
		void frameStarted() override;
		void frameCommandsQueued() override;
		void frameEnded() override;
		Scene* createSampleScene();
		Window* getWindow() const { return mWindow.get(); }
		static Application& getSingleton() { return *kSingleton; }
		static Application* getSingletonPtr() { return kSingleton; }
	protected:
		static Application* kSingleton;
		WindowSharedPtr mWindow = nullptr;
		RendererSharedPtr mRenderer = nullptr;
		SceneManagerSharedPtr mSceneManager = nullptr;
		Scene* mActiveScene = nullptr;
		CameraSharedPtr mMainCamera = nullptr;
		OrbiterCameraControllerSharedPtr mOrbiterCameraController = nullptr;
		FirstPersonCameraControllerSharedPtr mFirstPersonCameraController = nullptr;
		CameraControllerType mCameraControllerType = CameraControllerType::Undefined;
	};
}
