#ifdef __cplusplus
#include <libultraship/libultraship.h>

#include "z64item.h"
#include "z64actor.h"
#include "z64save.h"
#include "./GameInteractor.h"
#include "./Anchor_Windows.h"

typedef struct {
    uint32_t clientId;
    std::string clientVersion;
    std::string name;
    Color_RGB8 color;
    uint32_t seed;
    uint8_t fileNum;
    bool gameComplete;
    uint8_t sceneNum;
    uint8_t roomIndex;
    uint32_t entranceIndex;
    PosRot posRot;
    PlayerData playerData;
} AnchorClient;

class GameInteractorAnchor {
  private:
    bool isEnabled;

    void HandleRemoteJson(nlohmann::json payload);

  public:
    static GameInteractorAnchor* Instance;
    static std::map<uint32_t, AnchorClient> AnchorClients;
    static std::vector<uint32_t> ActorIndexToClientId;
    static std::string clientVersion;

    void Enable();
    void Disable();

    void TransmitJsonToRemote(nlohmann::json payload);
};

struct AnchorMessage {
    uint32_t id = 0;
    const char* itemIcon = nullptr;
    std::string prefix = "";
    ImVec4 prefixColor = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
    std::string message = "";
    ImVec4 messageColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    std::string suffix = "";
    ImVec4 suffixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
    float remainingTime = 10.0f;
};

#endif

#ifdef __cplusplus
extern "C" {
#endif

void Anchor_RegisterHooks();
void Anchor_PushSaveStateToRemote();
void Anchor_RequestSaveStateFromRemote();
uint8_t Anchor_GetClientScene(uint32_t actorIndex);
PosRot Anchor_GetClientPosition(uint32_t actorIndex);
const char* Anchor_GetClientName(uint32_t actorIndex);
uint8_t Anchor_GetClientRoomIndex(uint32_t actorIndex);
Vec3s* Anchor_GetClientJointTable(uint32_t actorIndex);
PlayerData Anchor_GetClientPlayerData(uint32_t actorIndex);
Color_RGB8 Anchor_GetClientColor(uint32_t actorIndex);
void Anchor_RefreshClientActors();
void Anchor_SendClientActors();
void Anchor_RequestTeleport(uint32_t clientId);
void Anchor_TeleportToPlayer(uint32_t clientId);

#ifdef __cplusplus
}
#endif