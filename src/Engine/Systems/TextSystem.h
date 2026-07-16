#ifndef TEXT_SYSTEM_H_DEFINED
#define TEXT_SYSTEM_H_DEFINED

#include "../ECS/System.h"

struct TextComponent; 
class World;

class TextSystem : public System
{
public:
    void Update(World& world, float deltaTime) override;
};

#endif
