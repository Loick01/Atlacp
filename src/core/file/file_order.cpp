#include "core/file/file.hpp"

FrameTextOrder FileReader::ReadFrameTextOrder(std::ifstream& input) const
{
    std::vector<std::string> texts;
    std::string s;
    std::getline(input >> std::ws, s, TEXT_DELIMITER);
    while (s != ORDER_DELIMITER){
        texts.push_back(s);
        std::getline(input >> std::ws, s, TEXT_DELIMITER);
    }
    return FrameTextOrder{texts};
}

DialogTextOrder FileReader::ReadDialogTextOrder(std::ifstream& input) const
{
    std::string facePath;
    input >> facePath;
    
    std::vector<std::string> texts;
    std::string s;
    std::getline(input >> std::ws, s, TEXT_DELIMITER);
    while (s != ORDER_DELIMITER){
        texts.push_back(s);
        std::getline(input >> std::ws, s, TEXT_DELIMITER);
    }
    
    return DialogTextOrder{texts, facePath};
}

NpcGoToOrder FileReader::ReadNpcGoToOrder(std::ifstream& input) const
{
    MapPosition targetPosition;
    unsigned int id;
    input >> targetPosition.x;
    input >> targetPosition.y;
    input >> id;
    return NpcGoToOrder{targetPosition, id, false};
}