#include "core/file/file.hpp"

Axis FileReader::ReadAxis(std::ifstream& input) const
{
    static const std::unordered_map<std::string, Axis> axis = {
        {"width", Axis::Width},
        {"height", Axis::Height}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, Axis>::const_iterator it = axis.find(s);
    if (it != axis.end())
        return it->second;

    throw std::runtime_error("Unknown value read as Axis");
}

Anchor FileReader::ReadAnchor(std::ifstream& input) const
{
    static const std::unordered_map<std::string, Anchor> anchors = {
        {"left_in", Anchor::LeftIn},
        {"left_out", Anchor::LeftOut},
        {"right_in", Anchor::RightIn},
        {"right_out", Anchor::RightOut},
        {"center", Anchor::Center},
        {"top_in", Anchor::TopIn},
        {"top_out", Anchor::TopOut},
        {"bottom_in", Anchor::BottomIn},
        {"bottom_out", Anchor::BottomOut}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, Anchor>::const_iterator it = anchors.find(s);
    if (it != anchors.end())
        return it->second;

    throw std::runtime_error("Unknown value read as Anchor");
}

CommandType FileReader::ReadCommandType(std::ifstream& input) const
{
    static const std::unordered_map<std::string, CommandType> commands = {
        {"attack", CommandType::Attack},
        {"heal", CommandType::Heal}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, CommandType>::const_iterator it = commands.find(s);
    if (it != commands.end())
        return it->second;

    throw std::runtime_error("Unknown value read as CommandType");
}

MoveType FileReader::ReadMoveType(std::ifstream& input) const
{
    static const std::unordered_map<std::string, MoveType> moves = {
        {"physical", MoveType::Physical},
        {"magic", MoveType::Magic}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, MoveType>::const_iterator it = moves.find(s);
    if (it != moves.end())
        return it->second;

    throw std::runtime_error("Unknown value read as MoveType");
}

MapBehaviour FileReader::ReadMapBehaviour(std::ifstream& input) const
{
    static const std::unordered_map<std::string, MapBehaviour> behaviours = {
        {"random", MapBehaviour::Random},
        {"follow", MapBehaviour::Follow},
        {"goto", MapBehaviour::GoTo}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, MapBehaviour>::const_iterator it = behaviours.find(s);
    if (it != behaviours.end())
        return it->second;

    throw std::runtime_error("Unknown value read as MapBehaviour");
}

FontSize FileReader::ReadFontSize(std::ifstream& input) const
{
    static const std::unordered_map<std::string, FontSize> fontSizes = {
        {"small", FontSize::Small}
    };

    std::string s;
    input >> s;
    std::unordered_map<std::string, FontSize>::const_iterator it = fontSizes.find(s);
    if (it != fontSizes.end())
        return it->second;

    throw std::runtime_error("Unknown value read as FontSize");
}