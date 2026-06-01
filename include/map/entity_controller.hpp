#pragma once

#include "map/interaction.hpp"
#include "map/player.hpp"

class Camera;
class Entity;
class FileReader;
class Tilemap;

class EntityController
{
    private:
        const FileReader& m_fileReader;
        Player m_player; // ?
        
        // Will use unique_ptr<Entity> ?
        std::vector<Entity*> m_renderedEntities; // Sorted by y position
        // A specific order could be necessary for Entity updating (for example with FollowEntityBehaviour), I use a second vector of Entity*
        std::vector<Entity*> m_updatedEntities;

        InteractionController m_interactionController;
    
    public:
        EntityController(const FileReader& fileReader, UiController& uiController, TextureController& textureController,
            Camera& camera, Tilemap& tilemap);
        ~EntityController();

        void Draw() const;
        void Update(const GameplayEventState& playerEventState, const float deltaTime); // GameplayEventState or call SetEventState in GameplayScene (before EntityController::Update()) 
        void SortRenderedEntities();
        void HandleEntityEvent(const EntityEvent e);

        void DeleteNPCs(); // Rename DeleteNPCs
        void LoadNPCs(TextureController& textureController, Camera& camera, Tilemap& tilemap, // These 3 paramaters should not be here ?
            const std::string& filepath, const unsigned int mapIndex); // Rename LoadNPCs
};