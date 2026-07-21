# Documentation — Moteur ECS Data-Oriented & Couche Réseau

> Périmètre de ce document : `Engine/` (ECS + boucle moteur), `Network/` (transport UDP + sérialisation), `NetworkBridge/` (réplication ECS ↔ réseau). Le renderer D3D12 et les utilitaires `Core/` ne sont pas détaillés ici, sauf mention indispensable à la compréhension.

## Sommaire

1. [Vue d'ensemble](#1-vue-densemble)
2. [Le cœur ECS](#2-le-cœur-ecs)
   - 2.1 [Entités : `EntityId` et `EntityManager`](#21-entités--entityid-et-entitymanager)
   - 2.2 [Composants : `ComponentRegister`](#22-composants--componentregister)
   - 2.3 [Stockage : `Column`](#23-stockage--column)
   - 2.4 [Archétypes : `Archetype`, `ArchetypeSystem`, `ArchetypeManager`](#24-archétypes--archetype-archetypesystem-archetypemanager)
   - 2.5 [`World` : la façade](#25-world--la-façade)
   - 2.6 [Systèmes : `System` et `SystemManager`](#26-systèmes--system-et-systemmanager)
   - 2.7 [Événements : `EventDispatcher`](#27-événements--eventdispatcher)
   - 2.8 [Scripts : `Script`, `ScriptCollection`, `ScriptManager`](#28-scripts--script-scriptcollection-scriptmanager)
3. [Boucle moteur](#3-boucle-moteur)
   - 3.1 [`EngineManager`](#31-enginemanager)
   - 3.2 [`Scene` / `SceneManager`](#32-scene--scenemanager)
4. [Couche réseau bas niveau (`Network/`)](#4-couche-réseau-bas-niveau-network)
   - 4.1 [`NetworkSocket`](#41-networksocket)
   - 4.2 [`NetworkManager` et `NetworkQueue`](#42-networkmanager-et-networkqueue)
   - 4.3 [Sérialisation binaire](#43-sérialisation-binaire)
5. [`NetworkBridge` : réplication ECS ↔ réseau](#5-networkbridge--réplication-ecs--réseau)
   - 5.1 [Rôle, contexte et lancement](#51-rôle-contexte-et-lancement)
   - 5.2 [Identité réseau des entités](#52-identité-réseau-des-entités)
   - 5.3 [Protocole applicatif : `PacketType` et `PacketHandler`](#53-protocole-applicatif--packettype-et-packethandler)
   - 5.4 [Réplication sortante : `DirtyFlag` et `NetworkSyncSystem`](#54-réplication-sortante--dirtyflag-et-networksyncsystem)
   - 5.5 [Lissage côté client : `NetworkInterpolator` et `InterpolationSystem`](#55-lissage-côté-client--networkinterpolator-et-interpolationsystem)
   - 5.6 [Ping / latence : `PingManager`](#56-ping--latence--pingmanager)
   - 5.7 [Cycle de vie complet client/serveur](#57-cycle-de-vie-complet-clientserveur)
6. [Exemples d'utilisation](#6-exemples-dutilisation)
   - 6.1 [Usage ECS pur — `TestWorld`](#61-usage-ecs-pur--testworld)
   - 6.2 [Scripting — `TestScript`](#62-scripting--testscript)
   - 6.3 [Session réseau complète — `TestNetwork`](#63-session-réseau-complète--testnetwork)
7. [Limites connues, dette technique et pistes d'amélioration](#7-limites-connues-dette-technique-et-pistes-damélioration)
8. [Annexe : types de base](#8-annexe--types-de-base)

---

## 1. Vue d'ensemble

Le moteur repose sur un **ECS archétype-based** (proche de Flecs / Unity DOTS) : les composants d'entités partageant la même "signature" (le même ensemble de types de composants) sont stockés contigus en mémoire dans un même **archétype**, sous forme de tableaux (**Structure of Arrays**). Les entités changent d'archétype dynamiquement à chaque `AddComponent` / `RemoveComponent`, via un **graphe d'archétypes** relié par des arêtes "add"/"remove" qui évitent de recalculer une signature à chaque appel.

Par-dessus cet ECS, une couche **réseau applicative** (`NetworkBridge`) réplique un sous-ensemble de l'état du monde (essentiellement `TransformComponent`) entre un serveur autoritaire et des clients, au-dessus d'un transport **UDP** fait maison (`Network/`), avec sa propre sérialisation binaire portable (big-endian réseau).

```
┌─────────────────────────────────────────────────────────────────┐
│                          EngineManager                          │
│   boucle de jeu (fixe côté serveur, liée au framerate côté       │
│   client) → Scene::Update → World::Update → SystemManager       │
└───────────────┬───────────────────────────────┬─────────────────┘
                │                               │
                ▼                               ▼
┌───────────────────────────┐      ┌─────────────────────────────┐
│           ECS              │      │         NetworkBridge        │
│  EntityManager              │◄────►│  PacketHandler / PacketBuilder│
│  ArchetypeManager/System     │      │  NetworkSyncSystem            │
│  Column (SoA)                │      │  InterpolationSystem          │
│  SystemManager / World       │      │  NetworkRegistry / PlayerReg. │
└───────────────────────────┘      └───────────────┬─────────────┘
                                                     ▼
                                     ┌─────────────────────────────┐
                                     │       Network (transport)     │
                                     │  NetworkManager (thread I/O)  │
                                     │  NetworkSocket (UDP)          │
                                     │  Serialization/Deserialization│
                                     └─────────────────────────────┘
```

**Principes clés :**

- Les composants doivent être **`trivially_copyable`** (vérifié par un `static_assert` dans `World::AddComponent`) : pas de `std::string`, `std::vector`, `std::function` à l'intérieur — le stockage `Column` utilise `memcpy` brut.
- Toute la mémoire des composants est allouée par `Column` (malloc/realloc), pas de `new` par composant : c'est le cœur du côté *data-oriented* (cache-friendly, itération séquentielle).
- Le réseau est **UDP non fiable** : aucune retransmission, aucun ordonnancement garanti. La fiabilité perçue vient de l'envoi périodique de deltas (dirty flags) et de snapshots complets à la connexion.
- Le serveur est **autoritaire** : c'est lui qui applique les inputs clients sur les `TransformComponent`, et qui diffuse l'état résultant.

---

## 2. Le cœur ECS

Fichiers : `src/Engine/ECS/`

### 2.1 Entités : `EntityId` et `EntityManager`

Une entité n'est qu'un entier 64 bits combinant un **index** (32 bits hauts) et une **version** (32 bits bas) :

```cpp
using EntityId = uint64_t;
inline uint32_t GetEntityIndex  (EntityId id) { return (uint32_t)(id >> 32); }
inline uint32_t GetEntityVersion(EntityId id) { return (uint32_t)(id & 0xFFFFFFFF); }
inline EntityId MakeEntityId(uint32_t index, uint32_t version);
```

`EntityManager` gère un tableau de slots (`EntitySlot { EntityRecord record; uint32_t version; bool alive; }`) avec une **free-list** : à la destruction, l'index est recyclé mais la version est incrémentée. Cela permet de détecter en O(1) si un `EntityId` périmé est encore valide (`IsAlive`), sans avoir besoin de "nettoyer" les références pendantes ailleurs dans le code — un ancien handle réutilise le même index mais échoue la comparaison de version.

Chaque slot vivant contient un `EntityRecord` (défini dans `EntityRecord.hpp`) :

```cpp
struct EntityRecord
{
    Archetype* archetype = nullptr; // dans quel archétype se trouve l'entité
    size_t     rowId     = 0;       // sa ligne dans les colonnes de cet archétype
};
```

C'est cette indirection (`EntityId → EntityRecord → Archetype + rowId`) qui permet de retrouver les composants d'une entité en O(1).

### 2.2 Composants : `ComponentRegister`

Chaque type de composant C++ se voit attribuer un `ComponentId` (`uint32_t`) unique et stable, généré paresseusement via un compteur statique par instanciation de template :

```cpp
template<typename T>
ComponentId ComponentRegister::GetComponentId()
{
    static ComponentId id = m_nextId++;   // un seul id par T, quel que soit l'appelant
    if (m_sizes.count(id) == 0)
        m_sizes[id] = sizeof(T);
    return id;
}
```

`GetComponentSize(id)` sert à `ArchetypeSystem` pour dimensionner les `Column` lors de la création d'un archétype. `MAX_COMPONENTS` (256, dans `Engine/define.h`) borne la taille du `ComponentMask` (`std::bitset<256>`) et des tableaux d'arêtes d'archétype.

### 2.3 Stockage : `Column`

`Column` est un tableau brut typé dynamiquement par sa `stride` (taille d'un élément), le tout géré en C pur (`malloc` / `realloc` / `memcpy`) :

```cpp
struct Column {
    void*  data;
    size_t stride;    // taille d'un composant
    size_t size;      // nb d'éléments occupés
    size_t capacity;   // capacité allouée
};
```

- `PushBack` : copie l'élément en fin de tableau, double la capacité (`Grow`) si nécessaire (capacité initiale = 8).
- `SwapRemove(index)` : supprime un élément en le remplaçant par le dernier (pas de trou, O(1), mais ne préserve pas l'ordre — c'est pourquoi `ArchetypeManager` doit répercuter le swap sur l'`EntityId` déplacé).

C'est la brique SoA de base : une `Column` par type de composant présent dans un archétype, toutes de même longueur (`entityCount`), indexées par la même `rowId`.

### 2.4 Archétypes : `Archetype`, `ArchetypeSystem`, `ArchetypeManager`

**`Archetype`** représente un ensemble unique de types de composants (une "signature") :

```cpp
struct Archetype {
    ComponentMask        signature;       // bitset des ComponentId présents
    Vector<ComponentId>  componentIds;
    Vector<Column>       columns;         // une Column par composant, même ordre que componentIds
    Vector<EntityId>     entities;        // entities[row] = EntityId à la ligne row
    size_t               entityCount;

    Archetype* addEdges[MAX_COMPONENTS];    // graphe : ajouter le composant i → cet archétype
    Archetype* removeEdges[MAX_COMPONENTS]; // graphe : retirer le composant i → cet archétype
};
```

**`ArchetypeSystem`** est la fabrique/registre : il indexe tous les archétypes existants dans une `unordered_map<ComponentMask, Archetype*>` et construit le **graphe d'archétypes** à la volée :

- `GetOrCreateAddEdge(archCourant, compId)` : si l'arête existe déjà, la réutilise. Sinon calcule la nouvelle signature (`set(compId)`), trouve ou crée l'archétype cible, et **mémorise les deux arêtes** (add depuis la source, remove depuis la cible) pour que le prochain aller-retour soit gratuit.
- Idem pour `GetOrCreateRemoveEdge`.
- `GetMatchingArchetypes(mask)` : parcourt tous les archétypes et retient ceux dont la signature contient le masque demandé (`(sig & mask) == mask`) — c'est la base des `Query`.

C'est le classique **graphe d'archétypes** popularisé par Flecs/EnTT-archetype/Bevy : ajouter/retirer un composant sur une entité déjà "connue" dans cette configuration devient une simple lecture de pointeur au lieu d'un hash + recherche.

**`ArchetypeManager`** exécute le déplacement effectif d'une entité d'un archétype à un autre :

```cpp
void ArchetypeManager::MoveEntity(EntityId entity, Archetype* src, Archetype* dst, ...)
{
    // 1. copie chaque colonne commune à src et dst (les composants inchangés survivent au déplacement)
    // 2. pousse l'entité dans dst->entities, incrémente dst->entityCount
    // 3. si src avait des données, la retire de src (RemoveFromArchetype = swap-remove)
    // 4. met à jour l'EntityRecord (nouvel archetype + rowId)
}
```

Le composant ajouté/retiré n'est pas dans l'intersection src/dst : sa colonne dans `dst` reste donc "vide" pour cette ligne — c'est `World::AddComponent` qui pousse ensuite la valeur initiale (voir 2.5).

### 2.5 `World` : la façade

`World` (`World.h` / `World.inl`) agrège tous les managers (`EntityManager`, `ArchetypeManager`, `ComponentRegister`, `SystemManager`, `ScriptManager`, `EventDispatcher`) et expose l'API publique utilisée par le reste du moteur.

**Cycle de vie d'une entité**

```cpp
EntityId World::CreateEntity()
{
    EntityId id = m_entityManager.CreateEntity();
    record->archetype = m_archetypeManager.archetypeSystem.GetEmptyArchetype();
    m_eventDispatcher.DispatchEntityCreated(*this, id);
    AddComponent<TransformComponent>(id);   // toute entité a un TransformComponent par défaut
    return id;
}
```

> ⚠️ Toute entité créée via `World::CreateEntity()` reçoit automatiquement un `TransformComponent`. C'est un choix architectural implicite à connaître : impossible de créer une entité "pure donnée" sans transform sans passer par un chemin bas niveau.

**`AddComponent<T>`** (dans `World.inl`) :

1. `static_assert(is_trivially_copyable_v<T>)`.
2. Anti-réentrance : si appelé pendant une `Query` (`m_isQuerying == true`), se protège pour ne pas invalider les itérateurs de la query en cours (voir plus bas).
3. Si l'entité a déjà le composant, retourne directement une référence dessus (idempotent).
4. Sinon, résout l'archétype cible via `GetOrCreateAddEdge`, déplace l'entité (`ArchetypeManager::MoveEntity`) si besoin, initialise le composant à zéro (`T zero{}; column.PushBack(&zero)`), déclenche `DispatchComponentAdded`, et retourne la référence.

**`RemoveComponent<T>`** suit le même schéma via `GetOrCreateRemoveEdge`, avec `DispatchComponentRemoved`.

**`GetComponent<T>`** : résolution directe `EntityRecord → Archetype → colonne → élément`, retourne `nullptr` si l'entité ou le composant n'existe pas.

**Queries** — `Query<Components...>(callback)` et `QueryWithEntity<Components...>(callback)` :

```cpp
template<typename... Components, typename Callback>
void World::Query(Callback&& callback)
{
    ComponentMask mask = /* OR de tous les ComponentId<Components> */;
    Vector<Archetype*> archetypes = m_archetypeManager.archetypeSystem.GetMatchingArchetypes(mask);

    // snapshot des EntityId concernées AVANT d'itérer
    Vector<EntityId> snapshot;
    for (archetype : archetypes) for (row : archetype) snapshot.push_back(entity);

    m_isQuerying = true;
    for (EntityId id : snapshot)
    {
        if (!IsAlive(id)) continue;          // l'entité a pu être détruite pendant la query
        if (archetype signature ne matche plus) continue;  // ou avoir changé d'archétype
        callback(composants...);
    }
    m_isQuerying = false;
}
```

Point important : la query prend un **snapshot des `EntityId`** avant d'itérer, puis revalide chaque entité (vivante + toujours conforme au masque) au moment de l'appel du callback. C'est ce qui rend "sûr" le fait d'ajouter/retirer des composants ou de créer/détruire des entités *depuis l'intérieur* d'un callback de query — contrairement à une itération directe sur les vecteurs de colonnes, qui serait invalidée par un `SwapRemove` en cours de boucle. Le flag `m_isQuerying` sert de garde-fou dans `AddComponent` pour éviter une récursion incohérente lors d'un ajout de composant en plein `Query`.

`QueryWithEntity` est identique mais fournit en plus l'`EntityId` au callback (utile pour appeler `world.DestroyEntity(id)` ou accéder à d'autres composants non capturés par la query).

**Autres responsabilités de `World`** :
- Enregistrement de systèmes (`RegisterSystem<T>`, délègue à `SystemManager`).
- Observers (`OnComponentAdded<T>`, `OnComponentRemoved<T>`, `OnEntityCreated`, `OnEntityDestroyed`) — délèguent à `EventDispatcher`.
- Scripts (`AddScript<T>`, `GetScript<T>`, `RemoveScript<T>`, `SetScriptActive<T>`) — délèguent à `ScriptManager`.
- `Update(deltaTime)` : appelle simplement `m_systemManager.Update(*this, deltaTime)`.

**Systèmes enregistrés par défaut** (constructeur de `World`, `World.cpp`) :

| Priorité | Système | Condition |
|---|---|---|
| -1 | `ScriptSystem` | toujours |
| 0 | `TransformSystem` | toujours |
| 1 | `MeshRendererSystem` | si pas serveur |
| 2 | `CameraSystem` | si pas serveur |
| 3 | `LightSystem` | si pas serveur |
| 4 | `TextSystem` | si pas serveur |
| 9 | `InterpolationSystem` | toujours |

Un serveur "headless" (`SceneManager::GetIsServer() == true`) n'instancie donc aucun système de rendu — seuls les systèmes de logique/réseau tournent, ce qui garde le tick serveur léger.

### 2.6 Systèmes : `System` et `SystemManager`

```cpp
class System {
public:
    virtual void Update(World& world, float deltaTime) = 0;
    virtual void OnRegister(World& world)   { m_pWorld = &world; }
    virtual void OnUnregister(World& world) {}
    bool IsActive() const; void SetActive(bool);
    int  GetPriority() const;
protected:
    World* m_pWorld;
private:
    bool m_active = true;
    int  m_priority = 0;
};
```

`SystemManager` stocke les systèmes dans un `Vector<SystemEntry>` (`unique_ptr<System>` + `type_index` + `priority`). `RegisterSystem<T>` est idempotent (retourne l'instance existante si `T` est déjà enregistré), marque `m_needsSort = true`, appelle `OnRegister`.

`Update(world, dt)` trie paresseusement (`std::stable_sort` par priorité croissante, seulement si `m_needsSort`) puis exécute chaque système actif dans l'ordre. Le tri stable garantit que deux systèmes de même priorité s'exécutent dans leur ordre d'enregistrement.

Cette architecture permet de garantir un ordre déterministe simple (ex : `TransformSystem` doit tourner avant les systèmes de rendu qui lisent les matrices monde, `NetworkSyncSystem` tourne en fin de frame après que la logique de jeu a marqué les composants "dirty").

### 2.7 Événements : `EventDispatcher`

Système d'observateurs pour 4 catégories d'événements : ajout/retrait de composant (indexé par `ComponentId`) et création/destruction d'entité (globaux). Chaque abonnement retourne un `ObserverId` (compteur global), stocké avec un `ObserverLocation` (type + éventuel `ComponentId`) dans `m_observerLocations` pour permettre un `Unsubscribe(id)` en O(1) amorti (recherche + swap-remove dans la bonne liste).

Utilisé en interne par exemple par `ScriptSystem::OnRegister` (voir 2.8), et exposé publiquement via `World::OnComponentAdded<T>` etc.

### 2.8 Scripts : `Script`, `ScriptCollection`, `ScriptManager`

En parallèle des `System` (qui opèrent par requête sur tous les composants d'un type), le moteur propose un modèle de **scripts attachés à une entité précise**, façon MonoBehaviour :

```cpp
class Script {
public:
    virtual void Start    (World&, EntityId self) {}
    virtual void Update   (World&, EntityId self, float dt) {}
    virtual void OnDestroy(World&, EntityId self) {}
    bool IsActive() const; void SetActive(bool);
    bool IsStarted() const;
};
```

`ScriptCollection` est un conteneur de scripts polymorphes pour **une** entité (`Vector<{unique_ptr<Script>, type_index}>`), avec `Add<T>`, `Get<T>`, `Remove<T>`, `SetActive<T>`. `ScriptManager` associe chaque `EntityId` à sa `ScriptCollection` (`UnorderedMap<EntityId, ScriptCollection>`).

`ScriptSystem` (priorité -1, donc exécuté avant tout le reste) parcourt tous les scripts vivants chaque frame : appelle `Start` une seule fois (`IsStarted`), puis `Update` à chaque frame si le script est actif. Il s'abonne à `OnEntityDestroyed` pour appeler `OnDestroy` sur tous les scripts de l'entité avant de purger sa collection.

> Les scripts ne sont **pas** des composants ECS (pas de colonne, pas de query possible dessus) : c'est une couche complémentaire pour du code de gameplay ad hoc par entité, sans se plier à la contrainte `trivially_copyable`.

---

## 3. Boucle moteur

### 3.1 `EngineManager`

Singleton (`EngineManager::GetInstance()`) qui orchestre l'initialisation et la boucle principale. Point d'entrée réseau + boucle de jeu.

**`Initialize(...)`** :
1. Parse les arguments CLI réseau (`NetworkLaunchArgs::Parse`) et initialise `NetworkContext` (démarre le socket UDP, ajoute le serveur comme peer si rôle client).
2. Crée le `PacketHandler` et le relie au `NetworkManager`.
3. Crée le `SceneManager` ; si le rôle réseau est `Server`, le marque comme tel (`SetIsServer(true)`).
4. **Si serveur** : alloue une console, redirige `stdout`, et retourne immédiatement — pas de fenêtre, pas de device de rendu.
5. **Sinon** (client / mode local) : crée la fenêtre, initialise le device D3D12, charge les ressources de base (géométries, shaders, matériaux par défaut), initialise l'`InputManager`.

**`Run()`** — deux boucles radicalement différentes :

- **Serveur** : boucle **à tick fixe** (60 Hz, `TICK_DELAY = 1/60`) avec accumulateur de temps (pattern "fixed timestep" classique) :
  ```cpp
  while (true) {
      accumulator += chrono.Reset();
      // draine tous les paquets reçus, enregistre les nouveaux clients, les dispatch au PacketHandler
      while (accumulator >= TICK_DELAY) {
          scene->Update(TICK_DELAY);
          accumulator -= TICK_DELAY;
      }
  }
  ```
- **Client** : boucle liée au framerate de la fenêtre (`while (window->IsOpen())`), qui en plus de la logique/rendu habituels : met à jour `PingManager`, draine les paquets reçus et les envoie au `PacketHandler`.

**`TryRegisterNewClients`** (uniquement appelé côté serveur) : détecte un paquet `PacketType::Connect` venant d'une adresse inconnue de `PlayerRegistry`, crée l'entité serveur correspondante (`NetworkIdentity` + `DirtyFlag` + `MeshRenderer` avec la géométrie/matériau demandés par le client), lui attribue un `networkId` via `NetworkRegistry::GenerateNetworkId()`, diffuse un paquet `EntityCreated` à tous les peers existants, puis envoie un **snapshot complet** au nouveau venu (voir §5.7 pour le détail du flux).

### 3.2 `Scene` / `SceneManager`

`Scene` encapsule un `World` et un cycle de vie (`OnInit`, `OnStart`, `OnUpdate`, `OnEnd`, `LoadRessources`), le tout piloté par `SceneManager` (singleton statique). `Scene::Update(dt)` appelle `OnUpdate(dt)`, gère l'affichage (`Clear`/`Display` de la fenêtre s'il y en a une — absent côté serveur), puis `world.Update(dt)`.

`SceneManager::CreateSceneType<T>` permet de créer des scènes typées dérivant de `Scene` (voir `TestNetworkScene` en §6.3), utile pour surcharger `OnInit`/`OnUpdate` avec de la logique spécifique à un test/niveau.

---

## 4. Couche réseau bas niveau (`Network/`)

Fichiers : `src/Network/`. Cette couche ne connaît **rien** de l'ECS ni du protocole applicatif : c'est un transport UDP générique + une boîte à outils de sérialisation binaire.

### 4.1 `NetworkSocket`

Fine enveloppe autour des sockets BSD/Winsock, abstraite par `#ifdef _WIN32` (Winsock2) vs POSIX. UDP uniquement (`SOCK_DGRAM`/`IPPROTO_UDP`), socket mise en **non-bloquant** côté Windows (`ioctlsocket(FIONBIO)`). Expose `Bind(port)`, `SendTo(data, dest)`, `RecvFrom(buffer, size, from)`, `Close()`, et un helper statique `MakeAddress(ip, port)`.

> Le code log abondamment sur `stdout` (`[SOCKET] ...`) à chaque bind/send — pratique en développement, à retirer/conditionner (macro debug) avant un build de production à fort débit de paquets.

### 4.2 `NetworkManager` et `NetworkQueue`

`NetworkManager` fait tourner un **thread dédié** (`NetworkLoop`) qui, en boucle serrée tant que `m_running` :
1. Vide la file sortante (`m_outQueue`, remplie par `SendTo` appelé depuis le thread principal) et envoie chaque paquet via `NetworkSocket::SendTo`.
2. Fait un `RecvFrom` non bloquant ; si des données arrivent, les empile dans `m_inQueue`.

`NetworkQueue` est une file thread-safe minimaliste (`std::queue` + `std::mutex`), utilisée pour les deux sens (in/out) — c'est le pont entre le thread réseau et le thread logique. Le thread logique consomme les paquets entrants via `PopReceived` (non bloquant, retourne `false` si vide) et pousse les sortants via `SendTo` (qui ne fait qu'empiler dans `m_outQueue`, l'envoi effectif étant fait par le thread réseau).

```
Thread logique                    Thread réseau (NetworkLoop)
──────────────                    ───────────────────────────
SendTo(data, dest)  ──push──►  m_outQueue ──pop──► socket.SendTo()
PopReceived(packet) ◄──pop───  m_inQueue  ◄──push── socket.RecvFrom()
```

`NetworkPacket` = `{ Vector<uint8> data; sockaddr_in address; }` — l'unité de base transportée entre les deux mondes.

`AddPeer(ip, port)` / `AddPeerAddress(sockaddr_in)` / `RemovePeer` / `GetPeers()` gèrent la liste des correspondants connus (côté serveur : tous les clients connectés ; côté client : le serveur, potentiellement d'autres si le modèle évolue en pair-à-pair).

### 4.3 Sérialisation binaire

Namespace `Serialization` (`Serializer` / `Deserializeration` — orthographe telle quelle dans le code). Format **binaire compact, big-endian réseau** pour les types multi-octets (via `htons`/`htonl`/`ntohs`/`ntohl` dans `Conversion.cpp`), garantissant l'interopérabilité si client/serveur tournent sur des architectures d'endianness différente.

**`Serializer`** — accumule dans un `Vector<uint8>` interne (`mBuffer`), API `write(T)` surchargée pour `uint8/16/32`, `int8/16/32`, `bool` (encodé sur 1 octet, `0x01`/`0x00`), `float32` (bits réinterprétés en `uint32` avant conversion réseau), `XMFLOAT4X4` (16 floats), et via template `writeContainer` pour `std::vector<T>` et `std::string` — préfixés par leur taille sur **1 octet** (`uint8`) :

```cpp
template <class CONTAINER>
bool Serializer::writeContainer(const CONTAINER& container)
{
    write(static_cast<uint8>(container.size()));   // ⚠️ borne max = 255 éléments
    for (const auto& element : container) write(element);
}
```

> ⚠️ Limite implicite : tout conteneur (vecteur, string) sérialisé via `writeContainer` est plafonné à **255 éléments/caractères**, le préfixe de taille étant un `uint8`. À surveiller si des chaînes plus longues ou de grosses listes doivent un jour transiter.

**`Deserializeration`** est le pendant en lecture, opérant sur un buffer externe non possédé (`const uint8* mBuffer`), avec suivi de position (`mBytesRead`) et vérification de bornes (`remainingBytes()`) avant chaque lecture — toute lecture hors bornes échoue proprement (retourne `false`) plutôt que de lire hors tampon.

Chaque type applicatif peut définir ses propres méthodes `serialize(Serializer&) const` / `deserialize(Deserializeration&)` en composant ces primitives (voir l'exemple `MyFirstMessage` dans `Network/main.cpp`, qui sert de test/démo autonome du module de sérialisation, indépendant de l'ECS).

---

## 5. `NetworkBridge` : réplication ECS ↔ réseau

Fichiers : `src/NetworkBridge/`. C'est la couche "métier réseau" qui connecte l'ECS (`World`) au transport (`Network/`). Elle définit *quoi* répliquer, *quand*, et *comment* traduire ça en composants ECS des deux côtés.

### 5.1 Rôle, contexte et lancement

`NetworkRole` : `None | Server | Client`. `NetworkLaunchArgs::Parse(argc, argv)` lit les arguments `--server`, `--client`, `--ip <ip>`, `--port <port>`, `--player <n>` en ligne de commande.

`NetworkContext` (singleton `Get()`) est le point d'accès global à l'état réseau courant :
- `Initialize(args)` : démarre le `NetworkManager` (`Start(localPort, isServer)`) ; si rôle client, ajoute le serveur comme peer (`AddPeer(serverIp, serverPort)`).
- `Disconnect()` : envoie un paquet `Disconnect` à tous les peers puis arrête proprement le `NetworkManager` (uniquement côté client — voir §7 pour la nuance à corriger côté serveur).
- Accesseurs : `GetRole()`, `GetManager()`, `IsServer()`, `IsClient()`.

### 5.2 Identité réseau des entités

Chaque entité répliquée porte deux composants dédiés :

```cpp
struct NetworkIdentity { UINT32 networkId = 0; };   // id stable, partagé entre client et serveur

struct DirtyFlag {
    std::bitset<32> bits;                            // 1 bit par "champ réplicable" (32 max)
    void Mark(uint32 idx); void Clear(uint32 idx); void ClearAll();
    bool IsDirty(uint32 idx) const; bool AnyDirty() const;
};
```

Le mapping `EntityId` (local, propre à chaque process) ↔ `networkId` (partagé sur le réseau) est tenu par **`NetworkRegistry`** (singleton) : `Register(networkId, localId)`, `GetLocalId(networkId)`, `HasNetworkId`, et `GenerateNetworkId()` (compteur auto-incrémenté côté serveur, démarre à 1).

Côté serveur uniquement, **`PlayerRegistry`** associe en plus chaque `sockaddr_in` (adresse IP:port du client) à son `EntityId` local et à un timestamp de dernière activité (`Chrono lastSeen`), avec `GetTimedOut(timeoutSeconds)` pour identifier les clients silencieux (mécanisme de détection de déconnexion, présent mais pas branché sur une purge automatique dans le code actuel — voir §7).

`NetworkComponentIndex` est un registre séparé qui attribue un index compact à des `ComponentId` "réplicables" — présent dans le code mais actuellement peu utilisé (le mapping composant → index de sérialisation est en pratique fait "à la main" avec des constantes `0x01` = Transform, `0x02` = Health *(non implémenté)* dans `PacketHandler`/`NetworkSyncSystem`). `NetworkBridgeInit.h` contient une ébauche commentée d'un système de dispatch générique par composant (`ComponentDispatcher`) qui n'est pas encore active.

`INetworkComponent` définit l'interface `Serialize`/`Deserialize` qu'un composant réplicable *pourrait* implémenter pour s'auto-sérialiser — utilisée comme type de paramètre dans `PacketBuilder::EntityCreated`/`ComponentUpdate`, bien qu'aucun composant concret ne l'implémente actuellement dans le code fourni (la sérialisation du `TransformComponent` est faite champ par champ, directement dans `PacketBuilder`/`PacketHandler`).

### 5.3 Protocole applicatif : `PacketType` et `PacketHandler`

Tous les paquets commencent par un octet **`PacketType`** :

| Valeur | Type | Émetteur → Récepteur | Contenu |
|---|---|---|---|
| `0x01` | `EntityCreated` | Serveur → tous les clients | `networkId`, `geoId`, `matId` |
| `0x02` | `EntityDestroyed` | Serveur → tous les clients (sauf origine si déco) | `networkId` |
| `0x03` | `ComponentUpdate` | Serveur → clients | `networkId`, `componentIndex`, données du composant |
| `0x04` | `Snapshot` | Serveur → nouveau client | `count`, puis pour chaque entité : id + transform complet + mesh |
| `0x05` | `Input` | Client → serveur | flags de mouvement + delta souris |
| `0x06` | `Ping` | Client → serveur | (vide) |
| `0x07` | `Pong` | Serveur → client | (vide) |
| `0x08` | `Connect` | Client → serveur | `geoId`, `matId` souhaités |
| `0x09` | `Disconnect` | Client → serveur | (vide) |

**`PacketHandler::Handle(packet, world)`** est le point d'entrée unique de désérialisation, appelé par `EngineManager::Run()` pour chaque paquet dépilé de la queue réseau. Il lit le premier octet et dispatch vers la méthode `Handle*` correspondante (switch exhaustif sur `PacketType`).

Points notables par handler :

- **`HandleEntityCreated`** (côté client, en pratique) : crée localement l'entité, lui ajoute `NetworkInterpolator` + `DirtyFlag` + `MeshRenderer` (avec fallback sur la géométrie "Cube" si non fournie) + `NetworkIdentity`, et l'enregistre dans `NetworkRegistry`.
- **`HandleInput`** (côté serveur uniquement) : retrouve l'entité du joueur via `PlayerRegistry::GetEntity(from)`, applique un déplacement simple (`speed = 10 * deltaTime`, orienté par `forward`/`right` du `Transform`), puis **marque le `DirtyFlag` bit 0** (transform modifié) — c'est ce flag que `NetworkSyncSystem` lira pour décider de répliquer.
- **`HandleComponentUpdate`** (côté client) : lit `networkId` + `componentId`, puis un `switch` sur `componentId` — actuellement seul `0x01` (Transform) est implémenté : si l'entité possède un `NetworkInterpolator`, la donnée est **empilée comme snapshot temporisé** (`interp.AddSnapshot(pos, scale, quat, now)`) plutôt qu'appliquée directement, pour permettre l'interpolation (§5.5) ; sinon (pas d'interpolateur), le transform est appliqué immédiatement.
- **`HandleSnapshot`** : reçoit un état complet du monde à la connexion — crée toutes les entités distantes d'un coup avec leur transform et mesh initiaux.
- **`HandlePing`/`HandlePong`** : mesure de latence aller-retour (voir §5.6).
- **`HandleConnected`/`HandleDisconnect`** : gestion (partielle, voir §7) du cycle de connexion côté serveur.

**`PacketBuilder`** est le pendant en écriture (méthodes statiques : `EntityCreated`, `EntityDestroyed`, `ComponentUpdate`, `Snapshot`) — construit un `Vector<uint8>` prêt à envoyer via `NetworkManager::SendTo`.

**`PacketInput`** encapsule l'état des touches/souris d'un client en un struct sérialisable dédié (`moveForward/Backward/Left/Right`, `jump`, `mouseDeltaX/Y`, `mouseLeft/Right`), avec ses propres `Serialize`/`Deserialize` statiques.

### 5.4 Réplication sortante : `DirtyFlag` et `NetworkSyncSystem`

**`NetworkSyncSystem`** (système ECS, priorité 10, actif uniquement côté serveur dans `TestNetworkScene::InitServer`) tourne à une **fréquence de réplication fixe et découplée du tick serveur** :

```cpp
void NetworkSyncSystem::Update(World& world, float deltaTime)
{
    m_timer += deltaTime;
    if (m_timer < 1.f / 20.f) return;   // 20 Hz de réplication, indépendamment des 60 Hz de simulation
    m_timer = 0.f;

    world.QueryWithEntity<NetworkIdentity, DirtyFlag>(
        [&](EntityId id, NetworkIdentity& identity, DirtyFlag& dirty) {
            if (!dirty.AnyDirty()) return;
            SendDirtyComponents(world, id, identity.networkId, dirty);
            dirty.ClearAll();
        });
}
```

C'est un modèle de **réplication delta basée sur des dirty flags** : seules les entités dont au moins un bit est marqué (par exemple, par `PacketHandler::HandleInput` suite à un mouvement) génèrent du trafic, et uniquement pour les champs effectivement modifiés (actuellement, seul le bit 0 = Transform est câblé ; un bit 1 "Health" est prévu en commentaire mais non implémenté). Cela réduit la bande passante par rapport à une diffusion systématique de l'état complet à chaque tick.

### 5.5 Lissage côté client : `NetworkInterpolator` et `InterpolationSystem`

Le réseau étant intrinsèquement irrégulier (paquets UDP à intervalles non garantis, ~20 Hz côté serveur), le client ne peut pas se contenter d'appliquer chaque mise à jour de transform telle quelle sous peine de saccades. `NetworkInterpolator` maintient un **buffer circulaire de snapshots datés** :

```cpp
struct TransformSnapshot { float timestamp; XMFLOAT3 pos; XMFLOAT3 scale; XMFLOAT4 quat; };

struct NetworkInterpolator {
    static constexpr size_t MAX_BUFFER_SIZE = 32;
    TransformSnapshot buffer[MAX_BUFFER_SIZE];
    size_t count = 0, head = 0;

    float GetDelay() const {   // délai de rendu adaptatif basé sur la latence mesurée
        float latency = PingManager::Get().GetLatency() / 1000.f;
        return std::max(0.1f, latency * 2.f);
    }
    void AddSnapshot(pos, scale, quat, timestamp);   // buffer circulaire, écrase le plus ancien si plein
};
```

**`InterpolationSystem`** (priorité 9, toujours actif) calcule, pour chaque entité possédant `NetworkIdentity + NetworkInterpolator + TransformComponent` :

1. `renderTime = now - interp.GetDelay()` — on affiche volontairement le monde **en retard** d'une durée proportionnelle au RTT mesuré (technique classique dite *"interpolation delay"* / *entity interpolation*), pour toujours disposer de deux snapshots encadrant l'instant affiché.
2. Recherche linéaire dans le buffer du couple `(prev, next)` tel que `prev.timestamp ≤ renderTime ≤ next.timestamp`.
3. Interpolation linéaire (`lerp`) pour position/échelle, **Slerp** (`XMQuaternionSlerp`) pour la rotation — évite les artefacts de rotation qu'un lerp naïf sur quaternion produirait.
4. Applique le résultat au `TransformComponent.local`, recalcule la matrice monde.

Ce découplage *écriture asynchrone par paquet réseau* (`HandleComponentUpdate`) / *lecture lissée par frame de rendu* (`InterpolationSystem`) est le pattern standard pour masquer la latence et la gigue réseau côté client, au prix d'un délai d'affichage volontaire.

### 5.6 Ping / latence : `PingManager`

Singleton simple : envoie un `Ping` toutes les `PING_INTERVAL` (1s) via `Update(dt, net)`, mesure le round-trip (`Chrono m_pingTime`) au `Pong` reçu (`OnPongReceived`), stocke une latence en millisecondes (`m_latency = rtt * 0.5`, donc latence *aller simple* estimée). Cette valeur alimente directement `NetworkInterpolator::GetDelay()` — plus la latence est haute, plus le client retarde son affichage pour absorber la gigue.

### 5.7 Cycle de vie complet client/serveur

```
CLIENT                                          SERVEUR
  │                                                │
  │──── Connect{geoId, matId} ───────────────────►│
  │                                                │ TryRegisterNewClients:
  │                                                │  - crée entité (NetworkIdentity+DirtyFlag+MeshRenderer)
  │                                                │  - PlayerRegistry.Register(addr, entity)
  │◄─── EntityCreated{netId, geoId, matId} ───────│  (broadcast à TOUS les peers, y compris le nouveau)
  │                                                │
  │◄─── Snapshot{count, [entités...]} ────────────│  (envoyé uniquement au nouveau client)
  │  crée localement toutes les entités distantes  │
  │  avec NetworkInterpolator                      │
  │                                                │
  │──── Input{moveForward, ...} (20 Hz) ──────────►│ HandleInput:
  │                                                │  applique le déplacement au Transform serveur
  │                                                │  Mark(DirtyFlag, 0)
  │                                                │
  │                                                │ NetworkSyncSystem (20 Hz):
  │◄─── ComponentUpdate{netId, 0x01, transform} ──│  pour chaque entité dirty → tous les peers
  │  AddSnapshot dans NetworkInterpolator          │
  │  (InterpolationSystem lisse l'affichage)       │
  │                                                │
  │──── Ping ──────────────────────────────────────►│
  │◄─── Pong ───────────────────────────────────────│
  │  PingManager calcule la latence                │
  │                                                │
  │──── Disconnect ────────────────────────────────►│ HandleDisconnect:
  │                                                │  broadcast EntityDestroyed aux autres clients
  │                                                │  PlayerRegistry.Unregister + world.DestroyEntity
```

---

## 6. Exemples d'utilisation

Fichiers : `src/Sample/`. Ce dossier contient les points d'entrée exécutables (`main.cpp` = `WinMain`) et une série de "tests" qui servent aussi de documentation vivante de l'API.

### 6.1 Usage ECS pur — `TestWorld`

`src/Sample/Tests/TestWorld.hpp` déclare des composants triviaux (`Position`, `Velocity`, `Health`) et trois systèmes de démonstration :

```cpp
struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Health   { int hp; };

class MovementSystem : public System {
    void Update(World& world, float dt) override {
        world.Query<Position, Velocity>([&](Position& pos, Velocity& vel) {
            pos.x += vel.dx * dt;
            pos.y += vel.dy * dt;
        });
    }
};

class CombatSystem : public System {
    void Update(World& world, float dt) override {
        world.QueryWithEntity<Health>([&](EntityId id, Health& hp) {
            hp.hp -= 10;
            if (hp.hp <= 0) world.DestroyEntity(id);   // destruction depuis l'intérieur d'une query
        });
    }
};
```

Utilisation typique du `World` en dehors du moteur graphique (aucune fenêtre requise) :

```cpp
World world;
world.RegisterSystem<MovementSystem>(0);
world.RegisterSystem<CombatSystem>(1);
SpawnSystem* spawnSys = world.RegisterSystem<SpawnSystem>(2);

// Observers
ObserverId obsAdd = world.OnComponentAdded<Position>([&](World&, EntityId) { /* ... */ });
ObserverId obsCreate = world.OnEntityCreated([&](World&, EntityId) { /* ... */ });

// Création d'entités
EntityId e1 = world.CreateEntity();
world.AddComponent<Position>(e1) = { 0.0f, 0.0f };
world.AddComponent<Velocity>(e1) = { 1.0f, 0.5f };
world.AddComponent<Health>(e1)   = { 30 };

// Boucle de simulation manuelle
for (int i = 0; i < 3; i++)
    world.Update(0.016f);

// Le recyclage d'index est vérifiable explicitement :
EntityId e4 = world.CreateEntity();
assert(GetEntityIndex(e4) == GetEntityIndex(e1));      // même slot recyclé
assert(GetEntityVersion(e4) != GetEntityVersion(e1));  // mais version différente → e1 est bien "mort"

world.RemoveComponent<Health>(e2);
world.SetSystemActive<MovementSystem>(false);   // désactiver un système sans le désenregistrer
```

Ce test illustre aussi bien la **survie des composants au changement d'archétype** (créer une entité avec `Position` seul, puis lui ajouter `Velocity` et `Health` un par un, conserve la valeur de `Position`) que la **sécurité des queries** vis-à-vis des mutations concurrentes (`CombatSystem` détruit des entités *pendant* que `world.Query` itère dessus, sans crash ni comportement indéfini, grâce au mécanisme de snapshot décrit en §2.5).

### 6.2 Scripting — `TestScript`

`src/Sample/Tests/TestScript.hpp` montre l'usage du système de scripts (complémentaire aux systèmes ECS) :

```cpp
class PlayerController : public Script {
    void Start(World& world, EntityId self) override {
        world.AddComponent<Velocity>(self) = { 1.0f, 0.0f };   // peut modifier l'ECS depuis un script
    }
    void Update(World& world, EntityId self, float dt) override { /* ... */ }
};

class HealthRegen : public Script {
    void Update(World& world, EntityId self, float dt) override {
        Health* hp = world.GetComponent<Health>(self);
        if (hp) hp->hp += 1;
    }
};

// Utilisation :
EntityId e1 = world.CreateEntity();
world.AddComponent<Health>(e1) = { 50 };
world.AddScript<PlayerController>(e1);
world.AddScript<HealthRegen>(e1);

world.Update(0.016f);   // Start() appelé une fois puis Update()
world.Update(0.016f);

world.SetScriptActive<HealthRegen>(e1, false);   // pause un script précis sans le retirer
world.Update(0.016f);   // HealthRegen ne s'exécute pas cette frame

Health* hp = world.GetComponent<Health>(e1);
assert(hp->hp == 52);   // +1 à chaque Update actif, donc 50 + 1 + 1 (pas de 3e incrément, désactivé)
```

### 6.3 Session réseau complète — `TestNetwork`

`src/Sample/Tests/TestNetwork.hpp` est l'exemple le plus complet : il définit une `Scene` dédiée (`TestNetworkScene`) qui bifurque son initialisation selon le rôle réseau, et un point d'entrée `TestNetwork::Run(argc, argv)` — c'est celui utilisé par `Sample/main.cpp`.

**Côté serveur** (`InitServer`) :

```cpp
void InitServer()
{
    auto* syncSystem = world.RegisterSystem<NetworkSyncSystem>(10);
    syncSystem->SetNetworkManager(&NetworkContext::Get().GetManager());
    MapBuilder::Build(world, false);   // construction du décor, sans rendu
}
```

**Côté client** (`InitClient`) : envoie le paquet `Connect` (avec la géométrie/matériau souhaités pour son avatar), crée une entité caméra locale (`TransformComponent` + `CameraComponent`), crée un texte HUD affichant le ping, puis construit la même carte (avec rendu cette fois : `MapBuilder::Build(world, true)`).

**Boucle applicative côté client** (`OnUpdate`) :

```cpp
void OnUpdate(float dt) override
{
    if (NetworkContext::Get().IsClient())
    {
        m_pText->SetString(std::to_string((int)PingManager::Get().GetLatency()) + " ms");
        SendInputs(dt);
    }
}

void SendInputs(float dt)
{
    static float timer = 0.f;
    timer += dt;
    if (timer < 1.f / 20.f) return;    // throttle client → serveur à 20 Hz, symétrique à NetworkSyncSystem
    timer = 0.f;

    InputPacket input;
    // lecture clavier selon le "slot" joueur (WASD pour player 1, flèches pour player 2 — même process/écran)
    if (player == 1) { input.moveForward = InputManager::IsKeyDown(Z); /* ... */ }
    if (player == 2) { input.moveForward = InputManager::IsKeyDown(UP_ARROW); /* ... */ }

    if (aucune touche pressée) return;   // pas de spam réseau si rien n'a changé

    Serialization::Serializer s;
    input.Serialize(s);
    for (const auto& peer : NetworkContext::Get().GetManager().GetPeers())
        net.SendTo(s.GetBuffer(), peer);
}
```

**Lancement** :

```cpp
static void Run(int argc, char* argv[])
{
    EngineManager::GetInstance().Initialize(1280, 720, L"TestNetwork", false, argc, argv);
    SceneManager::CreateSceneType<TestNetworkScene>("TestNetwork");
    auto* scene = (TestNetworkScene*)SceneManager::SetCurrentScene("TestNetwork");
    scene->player = NetworkLaunchArgs::Parse(argc, argv).player;
    EngineManager::GetInstance().Run();
}
```

En pratique, pour tester en local : lancer un exécutable avec `--server`, puis un ou deux exécutables clients avec `--client --ip 127.0.0.1 --port 7777 --player 1` (respectivement `--player 2`).

---

## 7. Limites connues, dette technique et pistes d'amélioration

À garder en tête pour la suite du développement — ce ne sont pas des bugs bloquants, mais des points explicitement incomplets ou fragiles dans l'état actuel du code :

- **Réplication de composants figée en dur.** `NetworkComponentIndex` et `INetworkComponent` posent les bases d'un système générique de sérialisation par composant, mais le chemin réellement emprunté (`PacketBuilder`, `PacketHandler::HandleComponentUpdate`, `NetworkSyncSystem::SendDirtyComponents`) reste codé en dur pour `TransformComponent` (index `0x01`). Ajouter un nouveau composant réplicable (ex. `Health`, prévu en commentaire) demande aujourd'hui de dupliquer ce câblage à la main à trois endroits.
- **`writeContainer` limité à 255 éléments** (préfixe de taille sur `uint8`) — à surveiller pour toute liste/chaîne potentiellement longue transitant sur le réseau (ex. nom de joueur, inventaire).
- **`PlayerRegistry::GetTimedOut`** existe mais n'est appelé nulle part dans le code fourni : un client qui perd la connexion sans envoyer `Disconnect` (crash, coupure réseau) n'est jamais purgé côté serveur ni déclaré déconnecté aux autres clients.
- **`NetworkContext::Disconnect()`** ne fait rien si le rôle courant n'est pas `Client` — un serveur arrêté (`~EngineManager` → `Exit()` → `Disconnect()`) ne prévient donc pas ses clients connectés.
- **Paquets UDP sans fiabilité applicative** : pas d'accusé de réception, pas de retransmission, pas de détection d'ordre (un `ComponentUpdate` en retard peut arriver après un plus récent). Le mécanisme d'interpolation atténue l'effet visuel pour le transform, mais un `EntityCreated`/`EntityDestroyed` perdu en transit n'est jamais rejoué.
- **Logs de debug intensifs** dans le chemin chaud du réseau (`NetworkSocket`, `NetworkManager`, plusieurs `Handle*`) — à conditionner (macro / niveau de log) avant tout profilage de bande passante ou de débit paquets/seconde réel.
- **`ScriptManager`/scripts non répliqués** : le système de scripts (§2.8) est purement local à un process ; aucune donnée de script ne transite sur le réseau — logique attendue si les scripts servent à du gameplay côté client uniquement (VFX, UI...), à documenter explicitement si un script doit un jour piloter une entité serveur-autoritaire.
- **`Column` ne libère jamais sa mémoire** (pas de destructeur visible appelant `free(data)`) et les `Archetype*` sont alloués avec `new` sans contrepartie `delete` dans le code fourni — a priori acceptable pour la durée de vie d'un process de jeu classique, mais à vérifier si des `World` sont créés/détruits en nombre (tests unitaires en boucle, hot-reload, etc.).

---

## 8. Annexe : types de base

Définis dans `Engine/define.h` (inclut `Core/define.h`, non détaillé ici) :

```cpp
#define MAX_COMPONENTS 256

using ComponentMask = std::bitset<MAX_COMPONENTS>;
using ComponentId   = uint32_t;
using ColumnIndex   = uint32_t;
using ObserverId    = uint32_t;
using EntityId      = uint64_t;   // 32 bits index (haut) | 32 bits version (bas)

constexpr EntityId INVALID_ENTITY = std::numeric_limits<EntityId>::max();
```

Dans `NetworkBridge/NetworkFlag.h` :

```cpp
constexpr size_t MAX_NETWORK_COMPONENTS = 32;   // taille du bitset DirtyFlag
```

Dans `Network/NetworkQueue.h` :

```cpp
struct NetworkPacket { std::vector<uint8_t> data; sockaddr_in address; };
```
