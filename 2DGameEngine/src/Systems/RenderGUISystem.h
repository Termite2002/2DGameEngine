#pragma once

#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"

class RenderGUISystem : public System
{	
	public:
		RenderGUISystem() = default;

		void Update(const std::unique_ptr<Registry>& registry, const SDL_Rect& camera) {
			ImGui::NewFrame();

			// Display a window to customize and create new enemies
			if (ImGui::Begin("Spawn enemies")) {
				
				// Static variables to store value of the enemy
				static int XPos = 0;
				static int YPos = 0;
				static int scaleX = 0;
				static int scaleY = 0;
				static int velX = 0;
				static int velY = 0;
				static int health = 0;
				static float rotation = 0.0f;
				static float projAngle = 0.0f;
				static float projSpeed = 100.0f;
				static int projRepeat = 10;
				static int projDuration = 10;
				const char* sprites[] = { "tank-image", "truck-image" };
				static int selectedSpriteIndex = 0;


				// Section to input sprite
				if (ImGui::CollapsingHeader("Sprite"), ImGuiTreeNodeFlags_DefaultOpen) {
					ImGui::Combo("texture id", &selectedSpriteIndex, sprites, IM_ARRAYSIZE(sprites));
				}
				ImGui::Spacing();

				// Section to input transform
				if (ImGui::CollapsingHeader("Transform"), ImGuiTreeNodeFlags_DefaultOpen) {
					ImGui::InputInt("x position", &XPos);
					ImGui::InputInt("y position", &YPos);
					ImGui::SliderInt("scale x", &scaleX, 1, 10);
					ImGui::SliderInt("scale y", &scaleY, 1, 10);
					ImGui::SliderAngle("rotation", &rotation, 0, 360);
				}
				ImGui::Spacing();

				// Section to input rigid body
				if (ImGui::CollapsingHeader("Rigid Body"), ImGuiTreeNodeFlags_DefaultOpen) {
					ImGui::InputInt("velocity x", &velX);
					ImGui::InputInt("velocity y", &velY);
				}
				ImGui::Spacing();

				// Section to input projectile emitter
				if (ImGui::CollapsingHeader("Projectile Emitter"), ImGuiTreeNodeFlags_DefaultOpen) {
					ImGui::SliderAngle("angle", &projAngle, 0, 360);
					ImGui::SliderFloat("speed", &projSpeed, 10, 500);
					ImGui::InputInt("repeat (sec)", &projRepeat);
					ImGui::InputInt("duration (sec)", &projDuration);
				}
				ImGui::Spacing();

				// Section to input health
				if (ImGui::CollapsingHeader("Health"), ImGuiTreeNodeFlags_DefaultOpen) {
					ImGui::SliderInt("health", &health, 1, 100);
				}
				ImGui::Spacing();

				ImGui::Separator();
				ImGui::Spacing();

				if(ImGui::Button("Spawn new enemy")) {
					Entity enemy = registry->CreateEntity();
					enemy.Group("enemies");
					enemy.AddComponent<TransformComponent>(glm::vec2(XPos, YPos), glm::vec2(scaleX, scaleY), glm::degrees(rotation));
					enemy.AddComponent<RigidBodyComponent>(glm::vec2(velX, velY));
					enemy.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, 2);
					enemy.AddComponent<BoxColliderComponent>(25, 20, glm::vec2(5, 5));
					double projVelX = cos(projAngle) * projSpeed;
					double projVelY = sin(projAngle) * projSpeed;
					enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projVelX, projVelY), projRepeat * 1000, projDuration * 1000, 10, false);
					enemy.AddComponent<HealthComponent>(health);

					// Reset all input values after spawning enemy
					XPos = YPos = scaleX = scaleY = rotation = projAngle = 0;
					scaleX = scaleY = 1;
					projRepeat = projDuration = 10;
					projSpeed = 100;
					health = 100;
				}
			}
			ImGui::End();

			// Displau a small overlay window to display the map position using the mouse	
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |  ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
			ImGui::SetNextWindowBgAlpha(0.9f);
			if (ImGui::Begin("Map coordinates", NULL, window_flags)) {
				ImGui::Text("Map coordinates: (x= %.1f, y= %.1f)",
							ImGui::GetIO().MousePos.x + camera.x, 
							ImGui::GetIO().MousePos.y + camera.y
				);
			}
			ImGui::End();

			ImGui::Render();
			ImGuiSDL::Render(ImGui::GetDrawData());
		}
};