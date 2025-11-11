#include "tileset.hpp"

Tileset::Tileset(TextureController* texture_controller, const FileReader* file_reader, const std::string& tileset_filepath, 
    const ScreenPosition position, const bool should_draw) :
    Drawable(texture_controller, tileset_filepath, position, should_draw), m_file_reader(file_reader), m_tileset_filepath(tileset_filepath)
{
    LoadTilesetHeader(GetHeaderForTileset(tileset_filepath));
    m_tilesets.push_back({m_texture_key,m_tileset_data});
}

Tileset::~Tileset()
{
    for (const KeyData& e : m_tilesets){
        m_texture_controller->DeleteTexture(e.first);
    }
}

void Tileset::LoadTileset(const std::string& tileset_filepath)
{
    m_texture_key = tileset_filepath; // Use hash function to get a key from the filepath (unless TextureKey is already std::string)
    m_texture_controller->LoadTextureFromFile(tileset_filepath, m_texture_key, m_texture_width, m_texture_height);
    LoadTilesetHeader(GetHeaderForTileset(tileset_filepath));
    m_tilesets.push_back({m_texture_key,m_tileset_data});
}

unsigned int Tileset::GetTextureWidth() const 
{
    // Check in Editor Tileset need an override of this function from Drawable
    return Drawable::GetTextureWidth();
}

unsigned int Tileset::GetTextureHeight() const 
{
    // Check in Editor Tileset need an override of this function from Drawable
    return Drawable::GetTextureHeight();
}

TextureKey Tileset::GetTilesetKey() const
{
    return m_texture_key;
}

std::string Tileset::GetTilesetFilepath() const
{
    return m_tileset_filepath;
}

unsigned int Tileset::GetTileSize() const
{
    return m_tile_size;
}

unsigned int Tileset::GetTilesetWidth() const
{
    return m_tileset_data.width;
}

unsigned int Tileset::GetTilesetHeight() const
{
    return m_tileset_data.height;
}

bool Tileset::IsEmptyTile(const unsigned char tile) const
{
    return m_tileset_data.solid_tiles.find(tile) == m_tileset_data.solid_tiles.end();
}

std::string Tileset::GetHeaderForTileset(const std::string& tileset_filepath) const
{
    return tileset_filepath.substr(0, tileset_filepath.size() - 3) + "txt";
}

void Tileset::LoadTilesetHeader(const std::string& tileset_header)
{
    m_file_reader->ReadTilesetHeader(tileset_header, m_tileset_data);
    if (m_tilesets.empty()) m_tile_size = m_tileset_data.tile_size; // Use m_tileset_data.tile_size for m_tile_size only when loading the first tileset
    else if (m_tileset_data.tile_size != m_tile_size) std::cout << "Try to load a tileset with a different tile_size, this should not happen\n";
}

void Tileset::UpdateSelectedTile(const ScreenPosition position, unsigned char& tile) const
{
    if (IsPositionInTexture(position)){
        int c = position.x/m_tile_size;
        int l = position.y/m_tile_size;
        tile = l*GetTilesetWidth()+c;
    }
}