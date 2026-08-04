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