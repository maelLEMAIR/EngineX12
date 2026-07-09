#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <any>
#include <DirectXCollision.h>
#include <string>
#include <vector>
#include <array>
#include <unordered_map> 
#include <unordered_set> 
#include <functional>

#include <DirectXMath.h>
#include <map>
#include <queue>
#include <mutex>

using namespace DirectX;

using int8      = int8_t;
using int16     = int16_t;
using int32     = int32_t;
using int64     = int64_t;

using uint8      = uint8_t;
using uint16     = uint16_t;
using uint32     = uint32_t;
using uint64     = uint64_t;

using float32 = float;
using float64 = double;

using String  = std::string;
using WString = std::wstring;
using Mutex   = std::mutex;
using Any     = std::any;
using Byte    = std::byte;

template <typename Type>
using Vector  = std::vector<Type>;

template <typename Type, uint64 Count>
using Array  = std::array<Type, Count>;

template <typename Type>
using Queue = std::queue<Type>;

template <typename Type>
using Hash = std::hash<Type>;

template <typename Type>
using LockGuard = std::lock_guard<Type>;

template <typename... Type>
using Tuple   = std::tuple<Type...>;

template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equality = std::equal_to<Key>>
using UnorderedMap = std::unordered_map<Key, Value, Hash, Equality>;

template <typename Type>
using UnorderedSet = std::unordered_set<Type>;

template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equality = std::equal_to<Key>>
using Map = std::map<Key, Value>;

template <typename Type>
using List = std::list<Type>;

template <typename Type1, typename Type2>
using Pair = std::pair<Type1, Type2>;

const WString GetResPath();
XMFLOAT3 ToColor(XMINT3 _color);
XMFLOAT3 ToColor(int _r, int _g, int _b);

#define RES(path) (GetResPath() + L##path)

#endif