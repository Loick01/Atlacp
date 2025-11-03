#include <iostream>
#include <vector>

#include "file.hpp"
#include "texture.hpp"

class Tilemap
{
    private:
        FileReader* m_file_reader;
        std::vector<unsigned int> m_map;
        unsigned int m_map_width;
        unsigned int m_map_height;
        unsigned int m_tile_size;
        char* m_tileset_filepath;

    public:
        Tilemap(FileReader* file_reader, const char* map_filepath, char* tileset_filepath);
        ~Tilemap();
        void LoadMap(const char* filepath);
        void DrawMap(const TextureController* texture_controller);
};