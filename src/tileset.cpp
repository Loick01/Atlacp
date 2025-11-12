#include "tileset.hpp"

Tileset::Tileset(TextureController* texture_controller, const FileReader* file_reader, const ScreenPosition position, const bool should_draw) :
    Drawable(texture_controller, position, should_draw), m_file_reader(file_reader)
{
    m_index_tileset = -1; // No tileset is loaded
}

Tileset::~Tileset()
{
    for (const TilesetData& e : m_tilesets){
        m_texture_controller->DeleteTexture(e.tileset_key);
    }
}

// Should be an override of Drawable::LoadTexture ?
void Tileset::LoadTileset(const std::string& tileset_filepath)
{
    TilesetData data;
    LoadTilesetHeader(GetHeaderForTileset(tileset_filepath), data);
    m_texture_key = tileset_filepath; // Use hash function to get a key from the filepath (unless TextureKey is already std::string)
    m_texture_controller->LoadTextureFromFile(tileset_filepath, m_texture_key, m_texture_width, m_texture_height);
    data.tileset_key = m_texture_key;
    if (m_tilesets.empty()){
        ++m_index_tileset; // This index was initialized with -1, it needs to be incremented at least once
        m_normalization_info.last_lower_bound = 0;
        m_normalization_info.last_upper_bound = data.width*data.height-1;
    }
    m_tilesets.push_back(data);
}

TextureKey Tileset::GetTextureKey() const
{
    return m_tilesets[m_index_tileset].tileset_key;
}

unsigned int Tileset::GetTileSize() const
{
    return m_tile_size;
}

unsigned int Tileset::GetTilesetWidth() const
{
    return m_tilesets[m_index_tileset].width;
}

unsigned int Tileset::GetTilesetHeight() const
{
    return m_tilesets[m_index_tileset].height;
}

bool Tileset::IsEmptyTile(const unsigned char tile) // Will be const once I remove GetNormalizedTile
{
    int t = GetNormalizedTile(tile); 
    return m_tilesets[m_index_tileset].solid_tiles.find(t) == m_tilesets[m_index_tileset].solid_tiles.end();
}

std::string Tileset::GetHeaderForTileset(const std::string& tileset_filepath) const
{
    return tileset_filepath.substr(0, tileset_filepath.size() - 3) + "txt";
}

void Tileset::LoadTilesetHeader(const std::string& tileset_header, TilesetData& data)
{
    m_file_reader->ReadTilesetHeader(tileset_header, data);
    if (m_tilesets.empty()) m_tile_size = data.tile_size; // Use data.tile_size for m_tile_size only when loading the first tileset
    else if (data.tile_size != m_tile_size) std::cout << "Try to load a tileset with a different tile_size, this should not happen\n";
}

void Tileset::UpdateSelectedTile(const ScreenPosition position, unsigned char& tile) const
{
    if (IsPositionInTexture(position)){
        int c = position.x/m_tile_size;
        int l = position.y/m_tile_size;
        tile = l*GetTilesetWidth()+c;
    }
}

TilesetData Tileset::GetTilesetData() const // Use this function everytime I try to get a tileset from m_index_tileset ?
{
    return m_tilesets[m_index_tileset];
}

void Tileset::NextTileset()
{
    m_index_tileset = ++m_index_tileset%m_tilesets.size();

    TilesetData current_data = GetTilesetData(); // Should be in a specific function, that will be used when drawing the tilemap
    m_texture_width = current_data.width*m_tile_size; 
    m_texture_height = current_data.height*m_tile_size;
    m_texture_key = current_data.tileset_key;
}

unsigned char Tileset::GetNormalizedTile(const unsigned char tile)
{
    int t = tile;
    if (t >= m_normalization_info.last_lower_bound && t <= m_normalization_info.last_upper_bound)
        return t-m_normalization_info.last_lower_bound;

    if (t > m_normalization_info.last_upper_bound){
        t -= m_normalization_info.last_upper_bound+1;
        for (unsigned int i = m_index_tileset+1 ; i < m_tilesets.size() ; i++){
            const TilesetData data = m_tilesets[i];
            int size = static_cast<int>(data.width*data.height);
            m_normalization_info.last_lower_bound = m_normalization_info.last_upper_bound+1;
            m_normalization_info.last_upper_bound = m_normalization_info.last_upper_bound+size;
            if (t-size < 0){
                m_index_tileset = i;
                return t;
            }
            t -= size;
        }
        std::cout << "No tileset can be find to draw this tile, this should not happen (tile = " << tile << ")\n";
    }else{ // t < m_normalization_info.last_lower_bound
        for (unsigned int i = m_index_tileset-1 ; i >= 0 ; i--){
            const TilesetData data = m_tilesets[i];
            int size = static_cast<int>(data.width*data.height);
            m_normalization_info.last_upper_bound = m_normalization_info.last_lower_bound-1;
            m_normalization_info.last_lower_bound = m_normalization_info.last_lower_bound-size;
            if (t-m_normalization_info.last_lower_bound >= 0){
                m_index_tileset = i;
                return t-m_normalization_info.last_lower_bound;
            }
        }
        std::cout << "No tileset can be find to draw this tile, this should not happen (tile = " << tile << ")\n";
    }

    std::cout << "No tileset can be find to draw this tile, this should not happen (tile = " << tile << ")\n";
    return 0;
}
