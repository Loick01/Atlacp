#include "core/file/file.hpp"

template <typename T>
T FileReader::ReadEnum(std::ifstream& input, const std::unordered_map<std::string,T>& values, const std::string& typeName) const
{
    std::string s;
    input >> s;
    typename std::unordered_map<std::string, T>::const_iterator it = values.find(s);
    if (it != values.end())
        return it->second;

    throw std::runtime_error("Unknown value read as " + typeName);
}

Axis FileReader::ReadAxis(std::ifstream& input) const
{
    static const std::unordered_map<std::string, Axis> axis = {
        {"width", Axis::Width},
        {"height", Axis::Height}
    };

    return ReadEnum(input, axis, "Axis");
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

    return ReadEnum(input, anchors, "Anchor");
}

CommandType FileReader::ReadCommandType(std::ifstream& input) const
{
    static const std::unordered_map<std::string, CommandType> commands = {
        {"attack", CommandType::Attack},
        {"heal", CommandType::Heal}
    };

    return ReadEnum(input, commands, "CommandType");
}

MoveType FileReader::ReadMoveType(std::ifstream& input) const
{
    static const std::unordered_map<std::string, MoveType> moves = {
        {"physical", MoveType::Physical},
        {"magic", MoveType::Magic}
    };

    return ReadEnum(input, moves, "MoveType");
}

MapBehaviour FileReader::ReadMapBehaviour(std::ifstream& input) const
{
    static const std::unordered_map<std::string, MapBehaviour> behaviours = {
        {"random", MapBehaviour::Random},
        {"follow", MapBehaviour::Follow},
        {"goto", MapBehaviour::GoTo}
    };

    return ReadEnum(input, behaviours, "MapBehaviour");
}

FontSize FileReader::ReadFontSize(std::ifstream& input) const
{
    static const std::unordered_map<std::string, FontSize> fontSizes = {
        {"small", FontSize::Small}
    };

    return ReadEnum(input, fontSizes, "FontSize");
}