#pragma once 

#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>

#include "drawable.hpp"
#include "file.hpp"
//#include "texture.hpp"
#include "type.hpp"

class Tilemap : public Drawable
{
    private:
        const FileReader* m_file_reader;
        MapData m_map;

    public:
        Tilemap(TextureController* texture_controller, const FileReader* file_reader, const std::string& map_filepath, const std::string& tileset_filepath);
        ~Tilemap();

        void LoadTileset(const std::string& tileset_filepath);
        void LoadMap(const std::string& filepath);
        void DrawTexture() const override;
};