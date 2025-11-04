#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>

#include "file.hpp"
#include "texture.hpp"

class Tilemap
{
    private:
        const TextureController* m_texture_controller;
        FileReader* m_file_reader;
        std::vector<unsigned char> m_map; // Tile index must not be greater than 255
        unsigned char m_map_width;
        unsigned char m_map_height;
        unsigned char m_tile_size;
        std::string m_tileset_filepath;

    public:
        Tilemap(const TextureController* texture_controller, FileReader* file_reader, const std::string& map_filepath, const std::string& tileset_filepath);
        ~Tilemap();
        void LoadMap(const std::string& filepath);
        void DrawMap() const;
};