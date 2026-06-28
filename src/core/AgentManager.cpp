#include "core/AgentManager.hpp"

namespace SwarmIDE::Core{

    AgentManager& AgentManager::getInstance() {
        static AgentManager instance;
        return instance;
    }

    void AgentManager::renderAllAgents(){
        for(auto& agent : agents){
            agent->render();
        }
    }

    void AgentManager::drawUIAllAgents(Camera& cam, glm::vec2 winSize){
        for(auto& agent : agents){
            agent->drawUI(cam, winSize);
        }
    }

    void AgentManager::updateAllAgents(){
        for(auto& agent : agents){
            agent->update();
        }
    }

    void AgentManager::addAgent(std::unique_ptr<SwarmIDE::AI::BaseAgent> agent) { 
        agents.push_back(std::move(agent));
    }

    int AgentManager::getCountAgent() { return agents.size(); }
}

