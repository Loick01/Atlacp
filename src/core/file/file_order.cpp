#include "core/file/file.hpp"

FrameTextOrder FileReader::ReadFrameTextOrder(std::ifstream& input) const
{
    std::vector<std::string> texts;
    std::string s;
    unsigned int nrText;
    
    input >> nrText;
    for (unsigned int i = 0 ; i < nrText ; i++) {
        std::getline(input >> std::ws, s, TEXT_DELIMITER);
        texts.push_back(s);
    }
    
    return FrameTextOrder{texts};
}

DialogTextOrder FileReader::ReadDialogTextOrder(std::ifstream& input) const
{
    std::vector<std::string> texts;
    std::string s;
    std::string facePath;
    unsigned int nrText;

    input >> nrText;
    input >> facePath;
    
    for (unsigned int i = 0 ; i < nrText ; i++) {
        std::getline(input >> std::ws, s, TEXT_DELIMITER);
        texts.push_back(s);
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