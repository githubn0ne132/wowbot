// game_actions.cpp
#include "game_actions.h"
#include "offsets.h" // Need offsets for function addresses
#include "globals.h"
#include <windows.h> // For OutputDebugStringA
#include <stdio.h>  // For sprintf_s

// Define function pointer types
typedef char (__cdecl* CastLocalPlayerSpell_t)(int spellId, int unknownIntArg, uint64_t targetGuid, char unknownCharArg);
typedef bool(__thiscall* ClickToMove_t)(void* pPlayer, int clickType, WGUID* interactGuid, WOWPOS* clickPos, float precision);

std::string CastSpell(int spellId, uint64_t targetGuid) {
    // Get function pointer using the KNOWN WORKING address
    CastLocalPlayerSpell_t CastLocalPlayerSpell_ptr = reinterpret_cast<CastLocalPlayerSpell_t>(0x0080DA40); // HARDCODED known working address

    char log_buffer[256];

    if (!CastLocalPlayerSpell_ptr) {
        OutputDebugStringA("[GameActions] Error: CastLocalPlayerSpell function pointer is null.\n");
        return "CAST_RESULT:ERROR:func null";
    }

    try {
        sprintf_s(log_buffer, sizeof(log_buffer), "[GameActions] Attempting cast SpellID: %d, TargetGUID: 0x%llX\n", spellId, targetGuid);
        OutputDebugStringA(log_buffer);

        // Call the function: CastLocalPlayerSpell(spellId, unknownIntArg=0, targetGuid, unknownCharArg=0)
        char result = CastLocalPlayerSpell_ptr(spellId, 0, targetGuid, 0);

        sprintf_s(log_buffer, sizeof(log_buffer), "[GameActions] CastLocalPlayerSpell returned: %d\n", (int)result);
        OutputDebugStringA(log_buffer);

        // Format the response string expected by Python
        char cast_resp_buf[64];
        sprintf_s(cast_resp_buf, sizeof(cast_resp_buf), "CAST_RESULT:%d,%d", spellId, (int)result);
        return std::string(cast_resp_buf);

    } catch (const std::exception& e) {
        std::string errorMsg = "[GameActions] ERROR during CastSpell call (exception): ";
        errorMsg += e.what(); errorMsg += "\n";
        OutputDebugStringA(errorMsg.c_str());
        return "CAST_RESULT:ERROR:crash"; // Match prefix
    } catch (...) {
        OutputDebugStringA("[GameActions] CRITICAL ERROR during CastSpell call: Memory access violation.\n");
        return "CAST_RESULT:ERROR:crash"; // Match prefix
    }
}

std::string MoveTo(float x, float y, float z) {
    ClickToMove_t ClickToMove_ptr = reinterpret_cast<ClickToMove_t>(0x727400);
    char log_buffer[256];

    if (!ClickToMove_ptr) {
        OutputDebugStringA("[GameActions] Error: ClickToMove function pointer is null.\n");
        return "MOVE_TO_RESULT:ERROR:func null";
    }

    __try {
        sprintf_s(log_buffer, sizeof(log_buffer), "[GameActions] Attempting to move to X: %.2f, Y: %.2f, Z: %.2f\n", x, y, z);
        OutputDebugStringA(log_buffer);

        WOWPOS pos = { x, y, z };
        void* pPlayer = (void*)*(DWORD*)0x00C79CE0;

        if (!pPlayer) {
            OutputDebugStringA("[GameActions] Error: Player pointer is null.\n");
            return "MOVE_TO_RESULT:ERROR:player null";
        }

        bool result = ClickToMove_ptr(pPlayer, 0x4, nullptr, &pos, 0.0f);

        sprintf_s(log_buffer, sizeof(log_buffer), "[GameActions] ClickToMove returned: %d\n", result);
        OutputDebugStringA(log_buffer);

        char move_resp_buf[64];
        sprintf_s(move_resp_buf, sizeof(move_resp_buf), "MOVE_TO_RESULT:%d", result);
        return std::string(move_resp_buf);

    } __except (EXCEPTION_EXECUTE_HANDLER) {
        DWORD exception_code = GetExceptionCode();
        sprintf_s(log_buffer, sizeof(log_buffer), "[GameActions] CRITICAL ERROR during MoveTo call: Caught exception 0x%X.\n", exception_code);
        OutputDebugStringA(log_buffer);
        return "MOVE_TO_RESULT:ERROR:exception_0x" + std::to_string(exception_code);
    }
}