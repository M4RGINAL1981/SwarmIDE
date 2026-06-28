#pragma once
#include "SwarmIDE.hpp"
#include "ai/BaseAgent.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Colors.hpp"
#include "graphics/Grid.hpp"
#include <iostream>
#include <vector>

namespace SwarmIDE::Core{

    class AgentManager{
    public:
        // Глобальная точка доступа к единственному экземпляру
        static AgentManager& getInstance();

        void renderAllAgents();                                      // Запуск всех агентов
        void drawUIAllAgents(Camera& cam, glm::vec2 winSize);        // Отрисовка всех агентов
        void updateAllAgents();                                      // Обновление всех агентов
        void addAgent(std::unique_ptr<SwarmIDE::AI::BaseAgent> agent);

        int getCountAgent();  // Вернуть кол-во агетов

        AgentManager(const AgentManager&) = delete;
        AgentManager& operator=(const AgentManager&) = delete;
        AgentManager(AgentManager&&) = delete;
        AgentManager& operator=(AgentManager&&) = delete;

    private:
        friend SwarmIDE::UI::Menu;    

        AgentManager() {};
        ~AgentManager() = default;

        std::vector<std::unique_ptr<SwarmIDE::AI::BaseAgent>> agents;
    };

}

