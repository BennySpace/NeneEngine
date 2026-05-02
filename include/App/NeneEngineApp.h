// Application.h

#pragma once

#include "App/AppConfig.h"
#include "App/GameStateMachine.h"
#include "Core/Delegate.h"
#include "Core/GameTimer.h"
#include "ECS/World.h"
#include "Platform/IWindow.h"
#include "ECS/Entity.h"
#include "ECS/Systems/RenderSystem.h"
#include "RenderAdapters/IRenderAdapter.h"

#include <atomic>
#include <cstddef>
#include <filesystem>
#include <EASTL/unique_ptr.h>
#include <memory>
#include <string>
#include <vector>

namespace NeneEngine 
{

	class NeneEngineApp 
	{
	public:
		NeneEngineApp();
		~NeneEngineApp();

		bool Init(uint32_t width = 1280, uint32_t height = 720, const std::string& title = "NeneEngine");
		void Run();
		void RequestShutdown();

	private:
		struct WindowContext
		{
			eastl::unique_ptr<IWindow> window;
			eastl::unique_ptr<IRenderAdapter> renderer;
			std::unique_ptr<ECS::RenderSystem> renderSystem;
			DelegateHandle resizeHandle;
			ECS::Entity cameraEntity = ECS::NullEntity;
			std::string title;
		};

		std::vector<WindowContext>			m_windows;
		
		GameTimer							m_timer;
		GameStateMachine					m_gameStateMachine;
		ECS::World							m_world;
		std::filesystem::path				m_appConfigPath;
		std::filesystem::file_time_type		m_appConfigLastWriteTime{};
		float								m_configReloadAccumulator = 0.0f;

		std::atomic<bool>					m_running{ false };
		std::atomic<bool>					m_isPaused{ false };

		bool CreateWindowContext(uint32_t width, uint32_t height, const std::string& title, ECS::Entity cameraEntity);
		std::vector<ECS::Entity> CreateAdditionalWindowCameras(size_t count, uint32_t width, uint32_t height);
		ECS::Entity FindPrimaryCameraEntity() const;
		bool AreAllWindowsClosed() const;
		void ApplyAppConfig(const AppConfig& config);
		void CalculateFrameStats();
		void HandleWindowResize(size_t windowIndex, uint32_t width, uint32_t height);
		void ReloadAppConfigIfChanged(float deltaTime);
	};

} // namespace NeneEngine
