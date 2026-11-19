#include "core/file/file.hpp"

#include "core/path.hpp"

std::vector<Order> FileReader::ReadOrders(std::ifstream& input) const
{
    static const std::unordered_map<std::string, OrderReader> orderReaders = {
        {"frame_text", &FileReader::ReadFrameTextOrder},
        {"dialog_text", &FileReader::ReadDialogTextOrder},
        {"npc_goto", &FileReader::ReadNpcGoToOrder},
        {"npc_follow", &FileReader::ReadNpcFollowOrder},
        {"npc_idle", &FileReader::ReadNpcIdleOrder},
        {"play_cinematic", &FileReader::ReadPlayCinematicOrder},
        {"camera_slide_to_position", &FileReader::ReadCameraSlideToPositionOrder},
        {"camera_slide_to_entity", &FileReader::ReadCameraSlideToEntityOrder},
        {"camera_anchor_entity", &FileReader::ReadCameraAnchorEntityOrder},
        {"entity_orientation", &FileReader::ReadEntityOrientationOrder},
        {"entity_create", &FileReader::ReadEntityCreateOrder},
        {"entity_delete", &FileReader::ReadEntityDeleteOrder},
        {"time_delay", &FileReader::ReadTimeDelayOrder},
        {"load_map", &FileReader::ReadLoadMapOrder},
        {"boxing_animation", &FileReader::ReadBoxingAnimationOrder},
    };

    std::vector<Order> orders;
    std::string s;
    const unsigned int nrOrder = ReadInt(input);

    orders.reserve(nrOrder);
    for (unsigned int i = 0 ; i < nrOrder ; i++) {
        input >> s;
        if (std::unordered_map<std::string, OrderReader>::const_iterator it = orderReaders.find(s); it != orderReaders.end())
            orders.push_back((this->*(it->second))(input));
        else {
            Logger::GetInstance().AddLog(Level::L4, "Unknown order type read in FileReader::ReadOrders: " + s); // TODO : Level
            break;
        }
    }

    return orders;
}

std::vector<Order> FileReader::ReadCinematicFile(const std::string& cinematicFilepath) const
{
    std::optional<std::ifstream> optInput = OpenFile(std::string(DataDirectory::Cinematic) + cinematicFilepath);
    if (optInput.has_value())
        return ReadOrders(optInput.value());
    else 
        return {};
}

Order FileReader::ReadFrameTextOrder(std::ifstream& input) const
{
    std::vector<std::string> texts;
    std::string s;
    const unsigned int nrText = ReadInt(input);
    
    for (unsigned int i = 0 ; i < nrText ; i++) {
        std::getline(input >> std::ws, s, MINOR_DELIMITER);
        texts.push_back(s);
    }
    
    return FrameTextOrder{texts};
}

Order FileReader::ReadDialogTextOrder(std::ifstream& input) const
{
    std::vector<std::string> texts;
    std::string s;
    std::string facePath;
    const unsigned int nrText = ReadInt(input);

    input >> facePath;
    
    for (unsigned int i = 0 ; i < nrText ; i++) {
        std::getline(input >> std::ws, s, MINOR_DELIMITER);
        texts.push_back(s);
    }
    
    return DialogTextOrder{texts, facePath};
}

Order FileReader::ReadNpcGoToOrder(std::ifstream& input) const
{
    MapPosition targetPosition;
    targetPosition.x = ReadInt(input);
    targetPosition.y = ReadInt(input);
    const unsigned int id = ReadInt(input);
    return NpcGoToOrder{targetPosition, id};
}

Order FileReader::ReadNpcFollowOrder(std::ifstream& input) const
{
    const unsigned int idLeader = ReadInt(input);
    const unsigned int idFollower = ReadInt(input);
    return NpcFollowOrder{idLeader, idFollower};
}

Order FileReader::ReadNpcIdleOrder(std::ifstream& input) const
{
    Direction direction;
    direction = ReadDirection(input);
    const unsigned int id = ReadInt(input);
    return NpcIdleOrder{direction, id};
}

Order FileReader::ReadPlayCinematicOrder(std::ifstream& input) const
{
    std::string s;
    input >> s;
    return PlayCinematicOrder{s};
}

Order FileReader::ReadCameraSlideToPositionOrder(std::ifstream& input) const
{
    ScenePosition pos;
    pos.x = ReadInt(input);
    pos.y = ReadInt(input);
    return CameraSlideToPositionOrder{pos};
}

Order FileReader::ReadCameraSlideToEntityOrder(std::ifstream& input) const
{
    const unsigned int idEntity = ReadInt(input);
    return CameraSlideToEntityOrder{idEntity};
}

Order FileReader::ReadCameraAnchorEntityOrder(std::ifstream& input) const
{
    const unsigned int idEntity = ReadInt(input);
    return CameraAnchorEntityOrder{idEntity};
}

Order FileReader::ReadEntityOrientationOrder(std::ifstream& input) const
{
    const unsigned int idEntity = ReadInt(input);
    Direction direction = ReadDirection(input);
    return EntityOrientationOrder{idEntity, direction};
}

Order FileReader::ReadEntityCreateOrder(std::ifstream& input) const
{
    const unsigned int idEntity = ReadInt(input);
    MapPosition spawn;
    spawn.x = ReadInt(input);
    spawn.y = ReadInt(input);
    std::string filepath;
    input >> filepath;
    return EntityCreateOrder{idEntity, spawn, filepath};
}

Order FileReader::ReadEntityDeleteOrder(std::ifstream& input) const
{
    const unsigned int idEntity = ReadInt(input);
    return EntityDeleteOrder{idEntity};
}

Order FileReader::ReadTimeDelayOrder(std::ifstream& input) const
{
    const float delay = ReadFloat(input);
    return TimeDelayOrder{delay};
}

Order FileReader::ReadLoadMapOrder(std::ifstream& input) const
{
    const unsigned int mapIndex = ReadInt(input);
    MapPosition spawn;
    spawn.x = ReadInt(input);
    spawn.y = ReadInt(input);
    return LoadMapOrder{mapIndex, PlayerReposition::MapSpawn, spawn};
}

Order FileReader::ReadBoxingAnimationOrder(std::ifstream& input) const
{
    const float speed = ReadFloat(input);
    return BoxingAnimationOrder{speed};
}