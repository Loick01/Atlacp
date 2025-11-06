#pragma once 

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "file.hpp"
#include "type.hpp"

class Tilemap : public Drawable
{
    private:
        const FileReader* m_file_reader;
        MapData m_map;

        unsigned char GetTileFromPosition(const Position p) const;

    public:
        Tilemap(TextureController* texture_controller, const FileReader* file_reader, const std::string& map_filepath, const std::string& tileset_filepath);
        ~Tilemap();

        bool IsPositionEmpty(const Position p) const;
        void LoadTileset(const std::string& tileset_filepath);
        void LoadMap(const std::string& filepath);
        void DrawTexture() const override;
};