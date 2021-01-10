#pragma once

#include "Horizon.h"
#include "Editor.h"

namespace Horizon
{
	class Application : public FrameListener
	{
	public:
		Application();
		~Application();
		void init();
		void run();
		void shutdown();
		void renderOneFrame();
		void update(Timestep timestep);
		void onEvent(Event& e);
		bool onWindowClose(const WindowCloseEvent& e);
		bool onWindowResize(const WindowResizeEvent& e);
		void frameStarted() override;
		void frameCommandsQueued() override;
		void frameEnded() override;
		void initGui();
		Scene* createSampleScene();
		Window* getWindow() const { return mWindow.get(); }
		static Application& getSingleton() { return *kSingleton; }
		static Application* getSingletonPtr() { return kSingleton; }
		Application(const Application&) = delete;
	protected:
		friend class Editor;
		static Application* kSingleton;
		WindowSharedPtr mWindow = nullptr;
		RendererSharedPtr mRenderer = nullptr;
		SceneManager* mSceneManager = nullptr;
		LayerStack* mLayerStack = nullptr;
		Scene* mActiveScene = nullptr;
		Editor* mEditor = nullptr;
		Entity* mLights[6];
	};
}
