#pragma once

namespace FileExtension {
    // Should be constexpr std::string_view ?
    const std::string Image = ".png";
    const std::string Sfx = ".wav";
    const std::string UiFile = ".uif";
    const std::string UiTemplate = ".uit";
}

namespace AssetDirectory {
    // Should be constexpr std::string_view ?
    const std::string Font = "../assets/ui/fonts/"; // Font should not be in UI directory ?
    const std::string Music = "../assets/sound/music/";
    const std::string Root = "../assets/";
    const std::string Sfx = "../assets/sound/sfx/";
    const std::string Spritesheet = "spritesheet/"; // ?
    const std::string Tileset = "../assets/tileset/";
}

namespace DataDirectory {
    // Should be constexpr std::string_view ?
    const std::string Animation = "../data/animation/";
    const std::string Battle = "../data/battle/battles/";
    const std::string Cinematic = "../data/cinematics/";
    const std::string Map = "../data/maps/";
    const std::string Move = "../data/battle/moves/";
    const std::string NPC = "../data/npcs/";
    const std::string Tileset = "../data/tileset/";
    const std::string UiFile = "../data/ui/file/";
    const std::string UiTemplate = "../data/ui/template/";
    const std::string World = "../data/worlds/";

    const std::string BattleMoveDirectory = "battle_move/";
}