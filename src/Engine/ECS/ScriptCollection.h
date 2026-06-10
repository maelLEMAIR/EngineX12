#ifndef SCRIPT_COLLECTION_H_INCLUDED
#define SCRIPT_COLLECTION_H_INCLUDED

#include "../define.h"
#include "Script.h"
#include <memory>
#include <typeindex>

class ScriptCollection
{
    
public:
    struct ScriptEntry
    {
        std::unique_ptr<Script> script;
        std::type_index         typeIndex;
    };

    Vector<ScriptEntry> scripts;

    template<typename T>
    T* Add()
    {
        static_assert(std::is_base_of<Script, T>::value, "T doit hériter de Script");

        auto s = std::make_unique<T>();
        T* ptr = s.get();
        scripts.push_back({ std::move(s), std::type_index(typeid(T)) });
        return ptr;
    }

    template<typename T>
    T* Get()
    {
        std::type_index idx(typeid(T));
        for (auto& entry : scripts)
            if (entry.typeIndex == idx)
                return static_cast<T*>(entry.script.get());
        return nullptr;
    }

    template<typename T>
    void Remove()
    {
        std::type_index idx(typeid(T));
        for (auto it = scripts.begin(); it != scripts.end(); ++it)
        {
            if (it->typeIndex == idx)
            {
                scripts.erase(it);
                return;
            }
        }
    }

    template<typename T>
    void SetActive(bool active)
    {
        T* s = Get<T>();
        if (s) s->SetActive(active);
    }
};

#endif