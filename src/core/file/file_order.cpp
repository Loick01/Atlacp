#include "core/file/file.hpp"

#include "core/path.hpp"

std::vector<Order> FileReader::ReadOrders(std::ifstream& input) const
{
    std::vector<Order> orders;
    std::string s;
    unsigned int nrOrder;

    input >> nrOrder;
    orders.reserve(nrOrder);
    for (unsigned int i = 0 ; i < nrOrder ; i++) {
        input >> s;
        if (s == "frame_text") {
            orders.push_back(ReadFrameTextOrder(input));
        } else if (s == "dialog_text") {
            orders.push_back(ReadDialogTextOrder(input));
        } else if (s == "npc_goto") {
            orders.push_back(ReadNpcGoToOrder(input));
        } else if (s == "npc_follow") {
            orders.push_back(ReadNpcFollowOrder(input));
        } else if (s == "npc_idle") {
            orders.push_back(ReadNpcIdleOrder(input));
        } else if (s == "play_cinematic") {
            orders.push_back(ReadPlayCinematicOrder(input));
        } else if (s == "camera_slide_to_position") {
            orders.push_back(ReadCameraSlideToPositionOrder(input));
        } else if (s == "camera_slide_to_entity") {
            orders.push_back(ReadCameraSlideToEntityOrder(input));
        } else if (s == "camera_anchor_entity") {
            orders.push_back(ReadCameraAnchorEntityOrder(input));
        } else if (s == "entity_orientation") {
            orders.push_back(ReadEntityOrientationOrder(input));
        } else if (s == "entity_delete") {
            orders.push_back(ReadEntityDeleteOrder(input));
        } else {
            throw std::runtime_error("Unknow order type : " + s);
        }       
    }
    return orders;
}

std::vector<Order> FileReader::ReadCinematicFile(const std::string& cinematicFilepath) const
{
    std::ifstream cinematicInput = OpenFile(DataDirectory::Cinematic + cinematicFilepath);
    return ReadOrders(cinematicInput);
}

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
    return NpcGoToOrder{targetPosition, id};
}

NpcFollowOrder FileReader::ReadNpcFollowOrder(std::ifstream& input) const
{
    unsigned int idLeader;
    unsigned int idFollower;
    input >> idLeader;
    input >> idFollower;
    return NpcFollowOrder{idLeader, idFollower};
}

NpcIdleOrder FileReader::ReadNpcIdleOrder(std::ifstream& input) const
{
    Direction direction;
    unsigned int id;
    direction = ReadDirection(input);
    input >> id;
    return NpcIdleOrder{direction, id};
}

PlayCinematicOrder FileReader::ReadPlayCinematicOrder(std::ifstream& input) const
{
    std::string s;
    input >> s;
    return PlayCinematicOrder{s};
}

CameraSlideToPositionOrder FileReader::ReadCameraSlideToPositionOrder(std::ifstream& input) const
{
    ScenePosition pos;
    input >> pos.x; input >> pos.y;
    return CameraSlideToPositionOrder{pos};
}

CameraSlideToEntityOrder FileReader::ReadCameraSlideToEntityOrder(std::ifstream& input) const
{
    unsigned int idEntity;
    input >> idEntity;
    return CameraSlideToEntityOrder{idEntity};
}

CameraAnchorEntityOrder FileReader::ReadCameraAnchorEntityOrder(std::ifstream& input) const
{
    unsigned int idEntity;
    input >> idEntity;
    return CameraAnchorEntityOrder{idEntity};
}

EntityOrientationOrder FileReader::ReadEntityOrientationOrder(std::ifstream& input) const
{
    unsigned int idEntity;
    input >> idEntity;
    Direction direction = ReadDirection(input);
    return EntityOrientationOrder{idEntity, direction};
}

EntityDeleteOrder FileReader::ReadEntityDeleteOrder(std::ifstream& input) const
{
    unsigned int idEntity;
    input >> idEntity;
    if (idEntity == 0) throw std::runtime_error("FileReader::ReadEntityDeleteOrder -> idEntity should not be 0 (Player)");
    return EntityDeleteOrder{idEntity};
}