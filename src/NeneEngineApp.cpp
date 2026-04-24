// NeneEngineApp.cpp

#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/Tag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Systems/RenderSystem.h"
#include "NeneEngineApp.h"
#include "RenderAdapters/DiligentDX12Adapter.h"
#include "States/PlayState.h"
#include "Win32Window.h"
#include "CustomLogger.h"

#include <stdexcept>

namespace NeneEngine
{
	namespace {

		ECS::Entity CreatePrimitiveEntity(
			ECS::World& world,
			std::string_view name,
			PrimitiveType primitiveType,
			const glm::vec3& position,
			const glm::vec3& scale,
			const glm::vec4& tint,
			MeshId meshId,
			MaterialId materialId,
			ShaderId shaderId)
		{
			const ECS::Entity entity = world.CreateEntity(std::string(name));
			auto& transform = world.AddComponent<ECS::Transform>(entity);
			transform.position = position;
			transform.scale = scale;

			auto& renderer = world.AddComponent<ECS::MeshRenderer>(entity);
			renderer.primitiveType = primitiveType;
			renderer.meshId = meshId;
			renderer.material.materialId = materialId;
			renderer.material.shaderId = shaderId;
			renderer.material.tint = tint;

			return entity;
		}

		void CreateTestScene(ECS::World& world, uint32_t width, uint32_t height)
		{
			const ECS::Entity cameraEntity = world.CreateEntity("MainCamera");
			auto& cameraTransform = world.AddComponent<ECS::Transform>(cameraEntity);
			cameraTransform.position = { 0.0f, 0.0f, 8.0f };

			auto& camera = world.AddComponent<ECS::CameraComponent>(cameraEntity);
			camera.aspectRatio = height == 0 ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
			camera.fovDegrees = 60.0f;
			camera.nearPlane = 0.1f;
			camera.farPlane = 100.0f;
			camera.isPrimary = true;

			CreatePrimitiveEntity(
				world,
				"SceneLine",
				PrimitiveType::Line,
				{ -3.5f, 1.8f, 0.0f },
				{ 2.5f, 1.0f, 1.0f },
				{ 1.0f, 0.35f, 0.35f, 1.0f },
				MeshId{},
				MaterialId{ 1u },
				ShaderId{ 1u });

			CreatePrimitiveEntity(
				world,
				"SceneTriangle",
				PrimitiveType::Triangle,
				{ -1.2f, -1.4f, 0.0f },
				{ 1.4f, 1.4f, 1.0f },
				{ 0.35f, 1.0f, 0.45f, 1.0f },
				MeshId{},
				MaterialId{ 2u },
				ShaderId{ 1u });

			CreatePrimitiveEntity(
				world,
				"SceneQuad",
				PrimitiveType::Quad,
				{ 1.4f, 1.0f, 0.0f },
				{ 2.1f, 1.2f, 1.0f },
				{ 0.25f, 0.75f, 1.0f, 1.0f },
				MeshId{},
				MaterialId{ 3u },
				ShaderId{ 1u });

			CreatePrimitiveEntity(
				world,
				"SceneCube",
				PrimitiveType::Cube,
				{ 3.6f, -0.7f, 0.0f },
				{ 1.6f, 1.6f, 1.6f },
				{ 1.0f, 0.85f, 0.3f, 1.0f },
				MeshId{},
				MaterialId{ 4u },
				ShaderId{ 1u });
		}

	} // namespace
	
	NeneEngineApp::NeneEngineApp() = default;

	NeneEngineApp::~NeneEngineApp()
	{
		if (m_running) RequestShutdown();

		if (spdlog::default_logger())
			spdlog::default_logger()->flush();

		spdlog::shutdown();
	}

	bool NeneEngineApp::Init(uint32_t width, uint32_t height, const std::string& title) 
	{
		try
		{
			// 1. Logger
			CustomLogger::GetInstance().Initialize("../../../../logs/nene_engine.log", true, spdlog::level::info, true);
			LOG_INFO("===== NeneEngine v0.2 starting =====");

			// 2. Window
			m_window = eastl::make_unique<Win32Window>();
			if (!m_window->Create(width, height, title))
				return false;

			// 3. Renderer
			m_renderer = eastl::make_unique<DiligentDX12Adapter>();
			if (!m_renderer->Init(m_window->GetHWND(), width, height))
				return false;
			
			// 4. States
			m_gameStateMachine.PushState(eastl::make_unique<PlayState>());

			// 5. ECS
			m_world.AddSystem(std::make_unique<ECS::RenderSystem>(m_renderer.get()));
			CreateTestScene(m_world, width, height);

			LOG_INFO("Application initialized successfully ({}x{})", width, height);

			return true;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Init failed: {}", e.what());

			return false;
		}
	}

	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

	void NeneEngineApp::CalculateFrameStats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.

		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;
		// Compute averages over one-second period.
		if ((m_timer.TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstringstream wss;
			wss << std::fixed << std::setprecision(0);
			wss << fps;
			std::wstring fpsStr = wss.str();
			wss.str(L""); // Reset wstringstream
			wss << std::setprecision(6);
			wss << mspf;
			std::wstring mspfStr = wss.str();

			std::wstring windowText = AnsiToWString(m_window->GetTitle()) +
				L"   fps: " + fpsStr +
				L"   mspf: " + mspfStr;

			SetWindowTextW(m_window->GetHWND(), windowText.c_str());

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}


	void NeneEngineApp::Run() 
	{
		m_running = true;
		m_timer.Reset();

		while (m_running && !m_window->ShouldClose())
		{
			m_window->PumpMessages();

			m_timer.Tick();
			float deltaTime = m_timer.DeltaTime();

			if (!m_isPaused)
			{
				m_gameStateMachine.HandleInput();
				m_gameStateMachine.Update(deltaTime);
				m_world.Update(deltaTime);

				m_renderer->BeginFrame();
				m_world.Render();
				m_renderer->EndFrame();
				m_renderer->Present();

				CalculateFrameStats();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	void NeneEngineApp::RequestShutdown()
	{
		m_running = false;
	}

} // namespace NeneEngine
