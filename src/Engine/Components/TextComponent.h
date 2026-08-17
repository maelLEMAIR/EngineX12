#ifndef TEXT_COMPONENT_H_DEFINED
#define TEXT_COMPONENT_H_DEFINED

#include "../define.h"
#include "RessourceManager.h"
#include "../../Core/Transform2D.h"
#include "../../Render/Generic/Render.h"

struct TextComponent
{
    uint32 textId;
    
    Transform2D transform;
    
    void SetText(String const& _text)
    {
        Text* t = RessourceManager::GetText(textId);
        if (t == nullptr) return;
        
        t->SetString(_text);
    }
    
    void SetColor(Vect3f32 _color)
    {
        Text* t = RessourceManager::GetText(textId);
        if (t == nullptr) return;
        
        t->SetColor(_color.x, _color.y, _color.z);
    }
};

#endif
