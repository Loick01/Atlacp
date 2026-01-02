#include "tileset.hpp"

Tileset::Tileset(TextureController& texture_controller, const FileReader& file_reader):
    ScreenDrawable(texture_controller, ScreenPosition{0,0}), m_file_reader(file_reader)
{
    m_index_tileset = -1; // No tileset is loaded
}

Tileset::~Tileset()
{
    CleanTilesets(); // Delete every textures whose keys are in m_tilesets
}

// Should be an override of Drawable::LoadTexture ?
void Tileset::LoadTileset(const std::string& path)
{
    TilesetData data = m_file_reader.GetTilesetFromFile(path);

    if (m_tilesets.empty()) m_tile_size = data.tile_size; // Use data.tile_size for m_tile_size only when loading the first tileset
    else if (data.tile_size != m_tile_size) std::cout << "Try to load a tileset with a different tile_size, this should not happen\n";

    m_texture_key = path; // Use hash function to get a key from the filepath (unless TextureKey is already std::string)
    const std::string tileset_filepath = "../assets/tilesets/" + path + ".png"; // Create a function in File
    m_texture_controller.LoadImageFromFile(tileset_filepath, m_texture_key, m_texture_width, m_texture_height);
    data.tileset_key = m_texture_key;
    if (m_tilesets.empty()){
        m_index_tileset = 0; // This index was initialized with -1, it needs to be 0 once the first tileset is loaded
        m_normalization_info.last_lower_bound = 0;
        m_normalization_info.last_upper_bound = data.size.x*data.size.y-1;
    }
    m_tilesets.push_back(data);
}

TextureKey Tileset::GetTextureKey() const
{
    return m_tilesets[m_index_tileset].tileset_key;
}

void Tileset::SetDisplayedTileset(const int selected_tileset)
{
    // Drawable::m_texture_key/m_texture_width/m_texture_height is not used in Tileset class, except in editor mode (when displaying the tileset) 
    m_texture_key = m_tilesets[selected_tileset].tileset_key;
    m_texture_width = m_tilesets[selected_tileset].size.x*m_tile_size;
    m_texture_height = m_tilesets[selected_tileset].size.y*m_tile_size;
}

int Tileset::GetTilesetsSize() const
{
    return m_tilesets.size();
}

int Tileset::GetTileSize() const
{
    return m_tile_size;
}

int Tileset::GetTilesetWidth() const
{
    return m_tilesets[m_index_tileset].size.x;
}

int Tileset::GetTilesetWidth(const int selected_tileset) const
{
    return m_tilesets[selected_tileset].size.x;
}

int Tileset::GetTilesetHeight() const
{
    return m_tilesets[m_index_tileset].size.y;
}

bool Tileset::IsEmptyTile(const Tile tile)
{
    Tile t = GetNormalizedTile(tile); 
    return m_tilesets[m_index_tileset].solid_tiles.find(t) == m_tilesets[m_index_tileset].solid_tiles.end();
}

void Tileset::UpdateSelectedTile(const ScreenPosition sp, const int selected_tileset, Tile& tile) const
{
    if (IsPositionInTexture(sp)){ // sp must be normalized (with screen position)
        int c = sp.x/m_tile_size;
        int l = sp.y/m_tile_size;
        int offset = 0;
        for (size_t i = 0 ; i < selected_tileset ; i++){ // Should store the offset for each tileset in TilesetData ?
            TilesetData t = m_tilesets[i];
            offset += t.size.x*t.size.y;
        }
        tile = offset + l*GetTilesetWidth(selected_tileset)+c;
    }
}

TilesetData Tileset::GetTilesetData() const // Use this function everytime I try to get a tileset from m_index_tileset ?
{
    return m_tilesets[m_index_tileset];
}

Tile Tileset::GetNormalizedTile(const Tile tile)
{
    int t = tile;
    if (t >= m_normalization_info.last_lower_bound && t <= m_normalization_info.last_upper_bound)
        return t-m_normalization_info.last_lower_bound;

    if (t > m_normalization_info.last_upper_bound){
        t -= m_normalization_info.last_upper_bound+1;
        for (size_t i = m_index_tileset+1 ; i < m_tilesets.size() ; i++){
            const TilesetData data = m_tilesets[i];
            int size = data.size.x*data.size.y;
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
        for (size_t i = m_index_tileset-1 ; i >= 0 ; i--){
            const TilesetData data = m_tilesets[i];
            int size = data.size.x*data.size.y;
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

void Tileset::CleanTilesets()
{
    for (const TilesetData& e : m_tilesets){
        m_texture_controller.DeleteTexture(e.tileset_key);
    }
    m_tilesets.clear();
}