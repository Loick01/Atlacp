#include "tile/tileset.hpp"

#include "image/texture.hpp"
// I had : tileset.hpp > file.hpp > layer.hpp > tileset.hpp > ... 
// Because I need a FileReader in Tileset::LoadTileset, I use a forward declaration in tileset.hpp, and I include file.hpp here
#include "core/file/file.hpp"
#include "core/path.hpp"

Tileset::Tileset(TextureController& textureController):
    ScreenDrawable(textureController, ScreenPosition{0,0})
{
    m_indexTileset = -1; // No tileset is loaded
}

Tileset::~Tileset()
{
    CleanTilesets(false); // Delete every textures whose keys are in m_tilesets
}

// TODO : Should be an override of Drawable::LoadTexture ?
void Tileset::LoadTileset(const FileReader& fileReader, const std::string& path)
{
    std::optional<DataTileset> optDataTileset = fileReader.ReadTilesetFile(path);
    if (!optDataTileset.has_value())
        return;
    DataTileset data = optDataTileset.value();

    if (m_tilesets.empty()) m_tileSize = data.tileSize; // Use data.tileSize for m_tileSize only when loading the first tileset
    else if (data.tileSize != m_tileSize) {
        Logger::GetInstance().AddLog(Level::L4, "Try to load a tileset with a different tileSize: " + data.tileSize); // TODO : Log
        return;
    }
    
    m_textureKey = path; // TODO : File extension is not in TextureKey ?
    const std::string tilesetFilepath = std::string(AssetDirectory::Tileset) + path + std::string(FileExtension::Image); 
    m_textureController.LoadTextureFromFile(tilesetFilepath, m_textureKey, m_textureSize);
    data.tilesetKey = m_textureKey;
    if (m_tilesets.empty()){
        m_indexTileset = 0; // This index was initialized with -1, it needs to be 0 once the first tileset is loaded
        m_normalizationInfo.lastLowerBound = 0;
        m_normalizationInfo.lastUpperBound = data.size.x*data.size.y-1;
    }
    m_tilesets.push_back(data);
}

TextureKey Tileset::GetTextureKey() const
{
    return m_tilesets[m_indexTileset].tilesetKey;
}

void Tileset::SetDisplayedTileset(const int tilesetIndex)
{
    // Drawable::m_textureKey/m_textureSize is not used in Tileset class, except in editor mode (when displaying the tileset) 
    if (m_tilesets.size() <= tilesetIndex) // TODO : Log ?
        return;
    const DataTileset tileset = m_tilesets.at(tilesetIndex);
    m_textureKey = tileset.tilesetKey;
    m_textureSize = tileset.size*m_tileSize;
}

int Tileset::GetTilesetsSize() const
{
    return m_tilesets.size();
}

int Tileset::GetTileSize() const
{
    return m_tileSize;
}

int Tileset::GetTilesetWidth() const
{
    return m_tilesets[m_indexTileset].size.x;
}

int Tileset::GetTilesetWidth(const int tilesetIndex) const
{
    return m_tilesets[tilesetIndex].size.x;
}

int Tileset::GetTilesetHeight() const
{
    return m_tilesets[m_indexTileset].size.y;
}

bool Tileset::IsEmptyTile(const Tile tile)
{
    Tile t = GetNormalizedTile(tile); 
    return m_tilesets[m_indexTileset].solidTiles.find(t) == m_tilesets[m_indexTileset].solidTiles.end();
}

void Tileset::UpdateSelectedTile(const ScreenPosition sp, const int tilesetIndex, Tile& tile) const
{
    if (IsPositionInTexture(sp)){ // sp must be normalized (with screen position)
        int c = sp.x/m_tileSize;
        int l = sp.y/m_tileSize;
        int offset = 0;
        for (size_t i = 0 ; i < tilesetIndex ; i++){ // TODO : Should store the offset for each tileset in DataTileset ?
            DataTileset t = m_tilesets[i];
            offset += t.size.x*t.size.y;
        }
        tile = offset + l*GetTilesetWidth(tilesetIndex)+c;
    }
}

DataTileset Tileset::GetDataTileset() const // TODO : Use this function everytime I try to get a tileset from m_indexTileset ?
{
    return m_tilesets[m_indexTileset];
}

Tile Tileset::GetNormalizedTile(const Tile tile)
{
    int t = tile; // TODO : Should be Tile instead of int
    if (t >= m_normalizationInfo.lastLowerBound && t <= m_normalizationInfo.lastUpperBound)
        return t-m_normalizationInfo.lastLowerBound;

    if (t > m_normalizationInfo.lastUpperBound){
        t -= m_normalizationInfo.lastUpperBound+1;
        for (size_t i = m_indexTileset+1 ; i < m_tilesets.size() ; i++){
            const DataTileset data = m_tilesets[i];
            int size = data.size.x*data.size.y;
            m_normalizationInfo.lastLowerBound = m_normalizationInfo.lastUpperBound+1;
            m_normalizationInfo.lastUpperBound = m_normalizationInfo.lastUpperBound+size;
            if (t-size < 0){
                m_indexTileset = i;
                return t;
            }
            t -= size;
        }
    }else{ // t < m_normalizationInfo.lastLowerBound
        for (size_t i = m_indexTileset-1 ; i >= 0 ; i--){
            const DataTileset data = m_tilesets[i];
            int size = data.size.x*data.size.y;
            m_normalizationInfo.lastUpperBound = m_normalizationInfo.lastLowerBound-1;
            m_normalizationInfo.lastLowerBound = m_normalizationInfo.lastLowerBound-size;
            if (t-m_normalizationInfo.lastLowerBound >= 0){
                m_indexTileset = i;
                return t-m_normalizationInfo.lastLowerBound;
            }
        }
    }
    // Logger::GetInstance().AddLog(Level::L4, "No tileset can be found to draw this tile: " + tile); // TODO : Log (Remove ?)
    return -1;
}

void Tileset::CleanTilesets(const bool shouldDeleteCurrent)
{
    for (const DataTileset& e : m_tilesets){
        if (shouldDeleteCurrent || e.tilesetKey != m_textureKey) // Tileset with m_textureKey will be deleted with ~Drawable
            m_textureController.DeleteTexture(e.tilesetKey);
    }
    m_tilesets.clear();
}