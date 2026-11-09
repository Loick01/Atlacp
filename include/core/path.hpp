#pragma once

namespace FileExtension {
    inline constexpr std::string_view Image = ".png";
    inline constexpr std::string_view Sfx = ".wav";
    inline constexpr std::string_view UiFile = ".uif";
    inline constexpr std::string_view UiTemplate = ".uit";
    inline constexpr std::string_view WorldNpcs = ".worldnpcs";
    inline constexpr std::string_view Npcs = ".npcs";
}

namespace AssetDirectory {
    inline constexpr std::string_view Font = "../assets/ui/fonts/"; // TODO : Font should not be in UI directory ?
    inline constexpr std::string_view Music = "../assets/sound/music/";
    inline constexpr std::string_view Root = "../assets/";
    inline constexpr std::string_view Sfx = "../assets/sound/sfx/";
    inline constexpr std::string_view Spritesheet = "spritesheet/"; // ?
    inline constexpr std::string_view Tileset = "../assets/tileset/";
}

namespace DataDirectory {
    inline constexpr std::string_view SpriteAnimation = "../data/animation/";
    inline constexpr std::string_view Battle = "../data/battle/battles/";
    inline constexpr std::string_view Cinematic = "../data/cinematics/";
    inline constexpr std::string_view Map = "../data/maps/";
    inline constexpr std::string_view Move = "../data/battle/moves/";
    inline constexpr std::string_view NPC = "../data/npcs/";
    inline constexpr std::string_view Tileset = "../data/tileset/";
    inline constexpr std::string_view UiFile = "../data/ui/file/";
    inline constexpr std::string_view UiTemplate = "../data/ui/template/";
    inline constexpr std::string_view World = "../data/worlds/";

    inline constexpr std::string_view BattleMoveDirectory = "battle_move/";
}