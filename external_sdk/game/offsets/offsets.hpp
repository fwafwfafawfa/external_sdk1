/*
Roblox version: version-f6dd34ecac7b4642 (LIVE)
Dumper version: 4b75797
Dumped at: 20:27 04/11/2025
Total offsets: 174
 _____ _ _ _
| ___| | | | | |
| |__ | |_| |__ ___ _ __ ___ __ _| |
| __|| __| '_ \ / _ \ '__/ _ \/ _` | |
| |___| |_| | | | __/ | | __/ (_| | |
\____/ \__|_| |_|\___|_| \___|\__,_|_|
        discord.gg/etheralrbx
            #1 External
*/
#pragma once
#include <cstdint>
#include <string>

namespace offsets {
    inline std::string ClientVersion = "version-f6dd34ecac7b4642";

    // === Core / Shared ===
    inline constexpr uintptr_t Adornee = 0x108;
    inline constexpr uintptr_t Anchored = 0x2;           // PrimitiveFlags.Anchored
    inline constexpr uintptr_t AnchoredMask = 0x2;
    inline constexpr uintptr_t AnimationId = 0xd0;
    inline constexpr uintptr_t StringLength = 0x10;
    inline constexpr uintptr_t Value = 0xd0;

    // === Instance ===
    inline constexpr uintptr_t AttributeToNext = 0x58;
    inline constexpr uintptr_t AttributeToValue = 0x18;
    inline constexpr uintptr_t Children = 0x70;
    inline constexpr uintptr_t ChildrenEnd = 0x8;
    inline constexpr uintptr_t ClassDescriptor = 0x18;
    inline constexpr uintptr_t ClassDescriptorToClassName = 0x8;
    inline constexpr uintptr_t Name = 0x90;
    inline constexpr uintptr_t Parent = 0x60;
    inline constexpr uintptr_t CanCollide = 0x8;
    inline constexpr uintptr_t CanCollideMask = 0x8;
    inline constexpr uintptr_t CanTouch = 0x10;
    inline constexpr uintptr_t CanTouchMask = 0x10;
    inline constexpr uintptr_t CFrame = 0xf8;            // Rotation + Position
    inline constexpr uintptr_t Position = 0x11c;
    inline constexpr uintptr_t Rotation = 0xf8;
    inline constexpr uintptr_t PartSize = 0x1d0;
    inline constexpr uintptr_t Transparency = 0xf0;
    inline constexpr uintptr_t Velocity = 0x128;
    inline constexpr uintptr_t Primitive = 0x148;
    inline constexpr uintptr_t PrimitiveValidateValue = 0x6;
    inline constexpr uintptr_t PrimitiveFlags = 0x294;
    inline constexpr uintptr_t PrimitiveOwner = 0x210;
    inline constexpr uintptr_t MaterialType = 0x268;
    inline constexpr uintptr_t Color3 = 0x194;

    // === Humanoid ===
    inline constexpr uintptr_t AutoJumpEnabled = 0x1dd;
    inline constexpr uintptr_t Health = 0x194;
    inline constexpr uintptr_t MaxHealth = 0x1b4;
    inline constexpr uintptr_t HipHeight = 0x1a0;
    inline constexpr uintptr_t JumpPower = 0x1b0;
    inline constexpr uintptr_t WalkSpeed = 0x1d4;
    inline constexpr uintptr_t WalkSpeedCheck = 0x3a0;
    inline constexpr uintptr_t HumanoidState = 0x858;
    inline constexpr uintptr_t HumanoidStateId = 0x20;
    inline constexpr uintptr_t RigType = 0x1c8;
    inline constexpr uintptr_t MaxSlopeAngle = 0x1b8;
    inline constexpr uintptr_t Sit = 0x1dc;
    inline constexpr uintptr_t EvaluateStateMachine = 0x1dd;

    // === Camera ===
    inline constexpr uintptr_t Camera = 0x410;            // Workspace.CurrentCamera
    inline constexpr uintptr_t CameraPos = 0x11c;
    inline constexpr uintptr_t CameraRotation = 0xf8;
    inline constexpr uintptr_t CameraSubject = 0xe8;
    inline constexpr uintptr_t CameraType = 0x158;
    inline constexpr uintptr_t FOV = 0x160;

    // === Player ===
    inline constexpr uintptr_t LocalPlayer = 0x130;
    inline constexpr uintptr_t CameraMaxZoomDistance = 0x2f0;
    inline constexpr uintptr_t CameraMinZoomDistance = 0x2f4;
    inline constexpr uintptr_t CameraMode = 0x2f8;
    inline constexpr uintptr_t DisplayName = 0x130;
    inline constexpr uintptr_t UserId = 0x298;
    inline constexpr uintptr_t Team = 0x270;
    inline constexpr uintptr_t ModelInstance = 0x360;
    inline constexpr uintptr_t PlayerMouse = 0xcd0;

    // === DataModel ===
    inline constexpr uintptr_t CreatorId = 0x188;
    inline constexpr uintptr_t GameId = 0x190;
    inline constexpr uintptr_t JobId = 0x138;
    inline constexpr uintptr_t PlaceId = 0x198;
    inline constexpr uintptr_t GameLoaded = 0x5f0;
    inline constexpr uintptr_t ScriptContext = 0x3d0;
    inline constexpr uintptr_t Workspace = 0x178;
    inline constexpr uintptr_t ServerIP = 0x5d8;
    inline constexpr uintptr_t PlaceVersion = 0x1b4;
    inline constexpr uintptr_t PrimitiveCount = 0x1244;

    // === Workspace ===
    inline constexpr uintptr_t Gravity = 0x1d8;          // Main gravity
    inline constexpr uintptr_t PrimitivesPointer1 = 0x398;
    inline constexpr uintptr_t PrimitivesPointer2 = 0x238;

    // === Visual Engine & Rendering ===
    inline constexpr uintptr_t VisualEnginePointer = 0x72fe710;
    inline constexpr uintptr_t VisualEngine = 0x10;
    inline constexpr uintptr_t viewmatrix = 0x4b0;
    inline constexpr uintptr_t Dimensions = 0x720;
    inline constexpr uintptr_t VisualEngineToDataModel1 = 0x700;
    inline constexpr uintptr_t VisualEngineToDataModel2 = 0x1c0;

    // === Fake DataModel ===
    inline constexpr uintptr_t FakeDataModelPointer = 0x759fd28;
    inline constexpr uintptr_t FakeDataModelToDataModel = 0x1c0;

    // === Task Scheduler ===
    inline constexpr uintptr_t TaskSchedulerPointer = 0x76de128;
    inline constexpr uintptr_t TaskSchedulerMaxFPS = 0x1b0;
    inline constexpr uintptr_t JobStart = 0x1d0;
    inline constexpr uintptr_t JobEnd = 0x1d8;
    inline constexpr uintptr_t Job_Name = 0x18;
    inline constexpr uintptr_t RenderJobToFakeDataModel = 0x38;
    inline constexpr uintptr_t RenderJobToRenderView = 0x218;
    inline constexpr uintptr_t RenderJobToDataModel = 0x1b0;

    // === Mouse ===
    inline constexpr uintptr_t MousePosition = 0xec;
    inline constexpr uintptr_t MouseSensitivity = 0x7677810;
    inline constexpr uintptr_t InputObject = 0x100;

    // === GuiObject (ESP-relevant) ===
    inline constexpr uintptr_t FramePositionX = 0x520;
    inline constexpr uintptr_t FramePositionY = 0x528;
    inline constexpr uintptr_t FrameSizeX = 0x540;
    inline constexpr uintptr_t FrameSizeY = 0x544;
    inline constexpr uintptr_t TextLabelText = 0xe40;
    inline constexpr uintptr_t TextLabelVisible = 0x5b9;

    // === Lighting ===
    inline constexpr uintptr_t ClockTime = 0x1b8;
    inline constexpr uintptr_t FogColor = 0xfc;
    inline constexpr uintptr_t FogEnd = 0x134;
    inline constexpr uintptr_t FogStart = 0x138;
    inline constexpr uintptr_t OutdoorAmbient = 0x108;

    // === Mesh & Textures ===
    inline constexpr uintptr_t DecalTexture = 0x198;
    inline constexpr uintptr_t MeshPartTexture = 0x310;

    // === ProximityPrompt ===
    inline constexpr uintptr_t ProximityPromptActionText = 0xd0;
    inline constexpr uintptr_t ProximityPromptEnabled = 0x156;
    inline constexpr uintptr_t ProximityPromptHoldDuraction = 0x140;
    inline constexpr uintptr_t ProximityPromptMaxActivationDistance = 0x148;
    inline constexpr uintptr_t ProximityPromptMaxObjectText = 0xf0;

    // === ClickDetector ===
    inline constexpr uintptr_t ClickDetectorMaxActivationDistance = 0x100;

    // === PlayerConfigurer ===
    inline constexpr uintptr_t PlayerConfigurerPointer = 0x757c588;
    inline constexpr uintptr_t ForceNewAFKDuration = 0x1f8;

    // === RunService ===
    inline constexpr uintptr_t RunContext = 0x148;

    // === Scripts ===
    inline constexpr uintptr_t LocalScriptByteCode = 0x1a8;
    inline constexpr uintptr_t LocalScriptHash = 0x1b8;
    inline constexpr uintptr_t LocalScriptBytecodePointer = 0x10;
    inline constexpr uintptr_t ModuleScriptByteCode = 0x150;
    inline constexpr uintptr_t ModuleScriptHash = 0x168;
    inline constexpr uintptr_t ModuleScriptBytecodePointer = 0x10;

    // === Sky ===
    inline constexpr uintptr_t SkyboxBk = 0x100;
    inline constexpr uintptr_t SkyboxDn = 0x128;
    inline constexpr uintptr_t SkyboxFt = 0x150;
    inline constexpr uintptr_t SkyboxLf = 0x178;
    inline constexpr uintptr_t SkyboxRt = 0x1a0;
    inline constexpr uintptr_t SkyboxUp = 0x1c8;
    inline constexpr uintptr_t MoonTextureId = 0xd8;
    inline constexpr uintptr_t SunTextureId = 0x1f0;
    inline constexpr uintptr_t StarCount = 0x220;

    // === Team ===
    inline constexpr uintptr_t TeamColor = 0xd0;

    // === Stats ===
    inline constexpr uintptr_t Ping = 0xc8;
}