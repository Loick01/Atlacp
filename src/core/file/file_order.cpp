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
    return FrameTextOrder{texts[0]}; // TODO : Text series
}

// TODO : ReadDialogBoxText, etc.