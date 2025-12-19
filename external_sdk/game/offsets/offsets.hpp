#pragma once

/*
Roblox version: version-5b077c09380f4fe6 (LIVE)
Dumper version: c7a9678
Dumped at: 21:19 17/12/2025
Total offsets: 175

 _____ _   _                         _
|  ___| | | |                       | |
| |__ | |_| |__   ___ _ __ ___  __ _| |
|  __|| __| '_ \ / _ \ '__/ _ \/ _` | |
| |___| |_| | | |  __/ | |  __/ (_| | |
\____/ \__|_| |_|\___|_|  \___|\__,_|_|


     https://discord.gg/etherealrbx
            My External

------------------------------------------

    My discord for offsets and methods:
    https://discord.gg/GM8rK3uAcF

*/

#include <cstdint>
#include <string>
namespace offsets {
    inline std::string ClientVersion = "version-5b077c09380f4fe6";

    namespace AnimationTrack {
        inline constexpr uintptr_t Animation = 0xd0;
        inline constexpr uintptr_t Animator = 0x118;
        inline constexpr uintptr_t IsPlaying = 0x2bd;
        inline constexpr uintptr_t Looped = 0xf5;
        inline constexpr uintptr_t Speed = 0xe4;
    }

    namespace BasePart {
        inline constexpr uintptr_t AssemblyAngularVelocity = 0xfc;
        inline constexpr uintptr_t AssemblyLinearVelocity = 0xf0;
        inline constexpr uintptr_t Color3 = 0x194;
        inline constexpr uintptr_t Material = 0x228;
        inline constexpr uintptr_t Position = 0x12c;
        inline constexpr uintptr_t Primitive = 0x148;
        inline constexpr uintptr_t PrimitiveFlags = 0x1ae;
        inline constexpr uintptr_t PrimitiveOwner = 0x1f0;
        inline constexpr uintptr_t Rotation = 0xc0;
        inline constexpr uintptr_t Shape = 0x1b1;
        inline constexpr uintptr_t Size = 0x1b0;
        inline constexpr uintptr_t Transparency = 0xf0;
        inline constexpr uintptr_t ValidatePrimitive = 0x6;
    }

    namespace ByteCode {
        inline constexpr uintptr_t Pointer = 0x10;
        inline constexpr uintptr_t Size = 0x20;
    }

    namespace Camera {
        inline constexpr uintptr_t CameraSubject = 0xe8;
        inline constexpr uintptr_t CameraType = 0x158;
        inline constexpr uintptr_t FieldOfView = 0x160;
        inline constexpr uintptr_t Position = 0x11c;
        inline constexpr uintptr_t Rotation = 0xf8;
    }

    namespace ClickDetector {
        inline constexpr uintptr_t MaxActivationDistance = 0x100;
        inline constexpr uintptr_t MouseIcon = 0xe0;
    }

    namespace DataModel {
        inline constexpr uintptr_t CreatorId = 0x188;
        inline constexpr uintptr_t GameId = 0x190;
        inline constexpr uintptr_t GameLoaded = 0x600;
        inline constexpr uintptr_t JobId = 0x138;
        inline constexpr uintptr_t PlaceId = 0x198;
        inline constexpr uintptr_t PlaceVersion = 0x1b4;
        inline constexpr uintptr_t PrimitiveCount = 0x440;
        inline constexpr uintptr_t ScriptContext = 0x3f0;
        inline constexpr uintptr_t ServerIP = 0x5e8;
        inline constexpr uintptr_t Workspace = 0x178;
    }

    namespace FFlags {
        inline constexpr uintptr_t DebugDisableTimeoutDisconnect = 0x67ae7a0;
        inline constexpr uintptr_t EnableLoadModule = 0x679d8f8;
        inline constexpr uintptr_t PartyPlayerInactivityTimeoutInSeconds = 0x676ca70;
        inline constexpr uintptr_t TaskSchedulerTargetFps = 0x74c0e6c;
        inline constexpr uintptr_t WebSocketServiceEnableClientCreation = 0x67bb778;
    }

    namespace FakeDataModel {
        inline constexpr uintptr_t Pointer = 0x7d03628;
        inline constexpr uintptr_t RealDataModel = 0x1c0;
    }

    namespace GuiObject {
        inline constexpr uintptr_t BackgroundColor3 = 0x550;
        inline constexpr uintptr_t BorderColor3 = 0x55c;
        inline constexpr uintptr_t Image = 0xa48;
        inline constexpr uintptr_t LayoutOrder = 0x58c;
        inline constexpr uintptr_t Position = 0x520;
        inline constexpr uintptr_t RichText = 0xae0;
        inline constexpr uintptr_t Rotation = 0x188;
        inline constexpr uintptr_t ScreenGui_Enabled = 0x4d4;
        inline constexpr uintptr_t Size = 0x540;
        inline constexpr uintptr_t Text = 0xe40;
        inline constexpr uintptr_t TextColor3 = 0xef0;
        inline constexpr uintptr_t Visible = 0x5b9;
    }

    namespace Humanoid {
        inline constexpr uintptr_t AutoRotate = 0x1d9;
        inline constexpr uintptr_t FloorMaterial = 0x190;
        inline constexpr uintptr_t Health = 0x194;
        inline constexpr uintptr_t HipHeight = 0x1a0;
        inline constexpr uintptr_t HumanoidState = 0x8d8;
        inline constexpr uintptr_t HumanoidStateID = 0x20;
        inline constexpr uintptr_t Jump = 0x1dd;
        inline constexpr uintptr_t JumpHeight = 0x1ac;
        inline constexpr uintptr_t JumpPower = 0x1b0;
        inline constexpr uintptr_t MaxHealth = 0x1b4;
        inline constexpr uintptr_t MaxSlopeAngle = 0x1b8;
        inline constexpr uintptr_t MoveDirection = 0x158;
        inline constexpr uintptr_t RigType = 0x1c8;
        inline constexpr uintptr_t Walkspeed = 0x1d4;
        inline constexpr uintptr_t WalkspeedCheck = 0x3c0;
    }

    namespace Instance {
        inline constexpr uintptr_t AttributeContainer = 0x48;
        inline constexpr uintptr_t AttributeList = 0x18;
        inline constexpr uintptr_t AttributeToNext = 0x58;
        inline constexpr uintptr_t AttributeToValue = 0x18;
        inline constexpr uintptr_t ChildrenEnd = 0x8;
        inline constexpr uintptr_t ChildrenStart = 0x70;
        inline constexpr uintptr_t ClassBase = 0xc58;
        inline constexpr uintptr_t ClassDescriptor = 0x18;
        inline constexpr uintptr_t ClassName = 0x8;
        inline constexpr uintptr_t Name = 0xb0;
        inline constexpr uintptr_t Parent = 0x68;
    }

    namespace Lighting {
        inline constexpr uintptr_t Ambient = 0xd8;
        inline constexpr uintptr_t Brightness = 0x120;
        inline constexpr uintptr_t ClockTime = 0x1b8;
        inline constexpr uintptr_t ColorShift_Bottom = 0xf0;
        inline constexpr uintptr_t ColorShift_Top = 0xe4;
        inline constexpr uintptr_t ExposureCompensation = 0x12c;
        inline constexpr uintptr_t FogColor = 0xfc;
        inline constexpr uintptr_t FogEnd = 0x134;
        inline constexpr uintptr_t FogStart = 0x138;
        inline constexpr uintptr_t GeographicLatitude = 0x190;
        inline constexpr uintptr_t OutdoorAmbient = 0x108;
    }

    namespace LocalScript {
        inline constexpr uintptr_t ByteCode = 0x1a8;
    }

    namespace MeshPart {
        inline constexpr uintptr_t MeshId = 0x2e0;
        inline constexpr uintptr_t Texture = 0x310;
    }

    namespace Misc {
        inline constexpr uintptr_t Adornee = 0x108;
        inline constexpr uintptr_t AnimationId = 0xd0;
        inline constexpr uintptr_t RequireLock = 0x0;
        inline constexpr uintptr_t StringLength = 0x10;
        inline constexpr uintptr_t Value = 0xd0;
    }

    namespace Model {
        inline constexpr uintptr_t PrimaryPart = 0x268;
        inline constexpr uintptr_t Scale = 0x164;
    }

    namespace ModuleScript {
        inline constexpr uintptr_t ByteCode = 0x150;
    }

    namespace MouseService {
        inline constexpr uintptr_t InputObject = 0x0;
        inline constexpr uintptr_t MousePosition = 0x0;
        inline constexpr uintptr_t SensitivityPointer = 0x7daf110;
    }

    namespace Player {
        inline constexpr uintptr_t CameraMode = 0x2f8;
        inline constexpr uintptr_t Country = 0x110;
        inline constexpr uintptr_t DisplayName = 0x130;
        inline constexpr uintptr_t Gender = 0xe68;
        inline constexpr uintptr_t LocalPlayer = 0x130;
        inline constexpr uintptr_t MaxZoomDistance = 0x2f0;
        inline constexpr uintptr_t MinZoomDistance = 0x2f4;
        inline constexpr uintptr_t ModelInstance = 0x360;
        inline constexpr uintptr_t Mouse = 0xcd8;
        inline constexpr uintptr_t Team = 0x270;
        inline constexpr uintptr_t UserId = 0x298;
    }

    namespace PlayerConfigurer {
        inline constexpr uintptr_t OverrideDuration = 0x5894805;
        inline constexpr uintptr_t Pointer = 0x7ce11e8;
    }

    namespace PlayerMouse {
        inline constexpr uintptr_t Icon = 0xe0;
        inline constexpr uintptr_t Workspace = 0x168;
    }

    namespace PrimitiveFlags {
        inline constexpr uintptr_t Anchored = 0x2;
        inline constexpr uintptr_t CanCollide = 0x8;
        inline constexpr uintptr_t CanTouch = 0x10;
    }

    namespace ProximityPrompt {
        inline constexpr uintptr_t ActionText = 0xd0;
        inline constexpr uintptr_t Enabled = 0x156;
        inline constexpr uintptr_t GamepadKeyCode = 0x13c;
        inline constexpr uintptr_t HoldDuration = 0x140;
        inline constexpr uintptr_t KeyCode = 0x9f;
        inline constexpr uintptr_t MaxActivationDistance = 0x148;
        inline constexpr uintptr_t ObjectText = 0xf0;
        inline constexpr uintptr_t RequiresLineOfSight = 0x157;
    }

    namespace RenderView {
        inline constexpr uintptr_t DeviceD3D11 = 0x8;
        inline constexpr uintptr_t VisualEngine = 0x10;
    }

    namespace RunService {
        inline constexpr uintptr_t HeartbeatFPS = 0xb8;
        inline constexpr uintptr_t HeartbeatTask = 0xe8;
    }

    namespace Sky {
        inline constexpr uintptr_t MoonAngularSize = 0x25c;
        inline constexpr uintptr_t MoonTextureId = 0xe0;
        inline constexpr uintptr_t SkyboxBk = 0x110;
        inline constexpr uintptr_t SkyboxDn = 0x140;
        inline constexpr uintptr_t SkyboxFt = 0x170;
        inline constexpr uintptr_t SkyboxLf = 0x1a0;
        inline constexpr uintptr_t SkyboxOrientation = 0x250;
        inline constexpr uintptr_t SkyboxRt = 0x1d0;
        inline constexpr uintptr_t SkyboxUp = 0x200;
        inline constexpr uintptr_t StarCount = 0x260;
        inline constexpr uintptr_t SunAngularSize = 0x254;
        inline constexpr uintptr_t SunTextureId = 0x230;
    }

    namespace SpecialMesh {
        inline constexpr uintptr_t MeshId = 0x108;
        inline constexpr uintptr_t Scale = 0x164;
    }

    namespace StatsItem {
        inline constexpr uintptr_t Value = 0x1c8;
    }

    namespace TaskScheduler {
        inline constexpr uintptr_t FakeDataModelToDataModel = 0x1b0;
        inline constexpr uintptr_t JobEnd = 0x1d8;
        inline constexpr uintptr_t JobName = 0x18;
        inline constexpr uintptr_t JobStart = 0x1d0;
        inline constexpr uintptr_t MaxFPS = 0x1b0;
        inline constexpr uintptr_t Pointer = 0x7e1cb88;
        inline constexpr uintptr_t RenderJobToFakeDataModel = 0x38;
        inline constexpr uintptr_t RenderJobToRenderView = 0x218;
    }

    namespace Team {
        inline constexpr uintptr_t BrickColor = 0xd0;
    }

    namespace Textures {
        inline constexpr uintptr_t Decal_Texture = 0x198;
        inline constexpr uintptr_t Texture_Texture = 0x198;
    }

    namespace VisualEngine {
        inline constexpr uintptr_t Dimensions = 0x720;
        inline constexpr uintptr_t Pointer = 0x7a69470;
        inline constexpr uintptr_t ToDataModel1 = 0x700;
        inline constexpr uintptr_t ToDataModel2 = 0x1c0;
        inline constexpr uintptr_t ViewMatrix = 0x4b0;
    }

    namespace Workspace {
        inline constexpr uintptr_t CurrentCamera = 0x450;
        inline constexpr uintptr_t DistributedGameTime = 0x470;
        inline constexpr uintptr_t Gravity = 0x1d0;
        inline constexpr uintptr_t GravityContainer = 0x3c8;
        inline constexpr uintptr_t PrimitivesPointer1 = 0x3c8;
        inline constexpr uintptr_t PrimitivesPointer2 = 0x240;
        inline constexpr uintptr_t ReadOnlyGravity = 0x9b0;
    }


    // ========================================================================
    // LEGACY FLAT ALIASES - Complete backward compatibility layer
    // ========================================================================

    // AnimationTrack
    inline constexpr uintptr_t Animation = AnimationTrack::Animation;
    inline constexpr uintptr_t Animator = AnimationTrack::Animator;
    inline constexpr uintptr_t IsPlaying = AnimationTrack::IsPlaying;
    inline constexpr uintptr_t Looped = AnimationTrack::Looped;
    inline constexpr uintptr_t Speed = AnimationTrack::Speed;

    // BasePart
    inline constexpr uintptr_t AssemblyAngularVelocity = BasePart::AssemblyAngularVelocity;
    inline constexpr uintptr_t AssemblyLinearVelocity = BasePart::AssemblyLinearVelocity;
    inline constexpr uintptr_t Color3 = BasePart::Color3;
    inline constexpr uintptr_t Material = BasePart::Material;
    inline constexpr uintptr_t MaterialType = BasePart::Material;
    inline constexpr uintptr_t Position = BasePart::Position;
    inline constexpr uintptr_t Primitive = BasePart::Primitive;
    inline constexpr uintptr_t PrimitiveOwner = BasePart::PrimitiveOwner;
    inline constexpr uintptr_t Rotation = BasePart::Rotation;
    inline constexpr uintptr_t CFrame = BasePart::Rotation;
    inline constexpr uintptr_t Shape = BasePart::Shape;
    inline constexpr uintptr_t Size = BasePart::Size;
    inline constexpr uintptr_t PartSize = BasePart::Size;
    inline constexpr uintptr_t Transparency = BasePart::Transparency;
    inline constexpr uintptr_t ValidatePrimitive = BasePart::ValidatePrimitive;
    inline constexpr uintptr_t PrimitiveValidateValue = BasePart::ValidatePrimitive;
    inline constexpr uintptr_t Velocity = BasePart::AssemblyLinearVelocity;

    // ByteCode
    inline constexpr uintptr_t ByteCodePointer = ByteCode::Pointer;
    inline constexpr uintptr_t LocalScriptBytecodePointer = ByteCode::Pointer;
    inline constexpr uintptr_t ModuleScriptBytecodePointer = ByteCode::Pointer;
    inline constexpr uintptr_t ByteCodeSize = ByteCode::Size;

    // Camera
    inline constexpr uintptr_t CameraSubject = Camera::CameraSubject;
    inline constexpr uintptr_t CameraType = Camera::CameraType;
    inline constexpr uintptr_t FieldOfView = Camera::FieldOfView;
    inline constexpr uintptr_t FOV = Camera::FieldOfView;
    inline constexpr uintptr_t CameraPos = Camera::Position;
    inline constexpr uintptr_t CameraRotation = Camera::Rotation;

    // ClickDetector
    inline constexpr uintptr_t ClickDetectorMaxActivationDistance = ClickDetector::MaxActivationDistance;
    inline constexpr uintptr_t MouseIcon = ClickDetector::MouseIcon;

    // DataModel
    inline constexpr uintptr_t CreatorId = DataModel::CreatorId;
    inline constexpr uintptr_t GameId = DataModel::GameId;
    inline constexpr uintptr_t GameLoaded = DataModel::GameLoaded;
    inline constexpr uintptr_t JobId = DataModel::JobId;
    inline constexpr uintptr_t PlaceId = DataModel::PlaceId;
    inline constexpr uintptr_t PlaceVersion = DataModel::PlaceVersion;
    inline constexpr uintptr_t PrimitiveCount = DataModel::PrimitiveCount;
    inline constexpr uintptr_t DataModelPrimitiveCount = DataModel::PrimitiveCount;
    inline constexpr uintptr_t ScriptContext = DataModel::ScriptContext;
    inline constexpr uintptr_t ServerIP = DataModel::ServerIP;

    // FFlags
    inline constexpr uintptr_t DebugDisableTimeoutDisconnect = FFlags::DebugDisableTimeoutDisconnect;
    inline constexpr uintptr_t EnableLoadModule = FFlags::EnableLoadModule;
    inline constexpr uintptr_t PartyPlayerInactivityTimeoutInSeconds = FFlags::PartyPlayerInactivityTimeoutInSeconds;
    inline constexpr uintptr_t TaskSchedulerTargetFps = FFlags::TaskSchedulerTargetFps;
    inline constexpr uintptr_t WebSocketServiceEnableClientCreation = FFlags::WebSocketServiceEnableClientCreation;
    inline constexpr uintptr_t FFlagList = FFlags::DebugDisableTimeoutDisconnect;

    // FakeDataModel
    inline constexpr uintptr_t FakeDataModelPointer = FakeDataModel::Pointer;
    inline constexpr uintptr_t FakeDataModelToDataModel = FakeDataModel::RealDataModel;

    // GuiObject
    inline constexpr uintptr_t BackgroundColor3 = GuiObject::BackgroundColor3;
    inline constexpr uintptr_t BorderColor3 = GuiObject::BorderColor3;
    inline constexpr uintptr_t Image = GuiObject::Image;
    inline constexpr uintptr_t LayoutOrder = GuiObject::LayoutOrder;
    inline constexpr uintptr_t FramePositionX = GuiObject::Position;
    inline constexpr uintptr_t FramePositionY = GuiObject::Position;
    inline constexpr uintptr_t RichText = GuiObject::RichText;
    inline constexpr uintptr_t FrameRotation = GuiObject::Rotation;
    inline constexpr uintptr_t ScreenGuiEnabled = GuiObject::ScreenGui_Enabled;
    inline constexpr uintptr_t FrameSizeX = GuiObject::Size;
    inline constexpr uintptr_t FrameSizeY = GuiObject::Size;
    inline constexpr uintptr_t Text = GuiObject::Text;
    inline constexpr uintptr_t TextLabelText = GuiObject::Text;
    inline constexpr uintptr_t TextColor3 = GuiObject::TextColor3;
    inline constexpr uintptr_t Visible = GuiObject::Visible;
    inline constexpr uintptr_t TextLabelVisible = GuiObject::Visible;
    inline constexpr uintptr_t FrameVisible = GuiObject::Visible;

    // Humanoid
    inline constexpr uintptr_t AutoRotate = Humanoid::AutoRotate;
    inline constexpr uintptr_t FloorMaterial = Humanoid::FloorMaterial;
    inline constexpr uintptr_t Health = Humanoid::Health;
    inline constexpr uintptr_t HipHeight = Humanoid::HipHeight;
    inline constexpr uintptr_t HumanoidState = Humanoid::HumanoidState;
    inline constexpr uintptr_t HumanoidStateID = Humanoid::HumanoidStateID;
    inline constexpr uintptr_t HumanoidStateId = Humanoid::HumanoidStateID;
    inline constexpr uintptr_t Jump = Humanoid::Jump;
    inline constexpr uintptr_t EvaluateStateMachine = Humanoid::Jump;
    inline constexpr uintptr_t Sit = Humanoid::Jump;
    inline constexpr uintptr_t JumpHeight = Humanoid::JumpHeight;
    inline constexpr uintptr_t JumpPower = Humanoid::JumpPower;
    inline constexpr uintptr_t MaxHealth = Humanoid::MaxHealth;
    inline constexpr uintptr_t MaxSlopeAngle = Humanoid::MaxSlopeAngle;
    inline constexpr uintptr_t MoveDirection = Humanoid::MoveDirection;
    inline constexpr uintptr_t RigType = Humanoid::RigType;
    inline constexpr uintptr_t Walkspeed = Humanoid::Walkspeed;
    inline constexpr uintptr_t WalkSpeed = Humanoid::Walkspeed;
    inline constexpr uintptr_t WalkspeedCheck = Humanoid::WalkspeedCheck;
    inline constexpr uintptr_t WalkSpeedCheck = Humanoid::WalkspeedCheck;

    // Instance
    inline constexpr uintptr_t AttributeContainer = Instance::AttributeContainer;
    inline constexpr uintptr_t InstanceAttributePointer1 = Instance::AttributeContainer;
    inline constexpr uintptr_t AttributeList = Instance::AttributeList;
    inline constexpr uintptr_t InstanceAttributePointer2 = Instance::AttributeList;
    inline constexpr uintptr_t AttributeToNext = Instance::AttributeToNext;
    inline constexpr uintptr_t AttributeToValue = Instance::AttributeToValue;
    inline constexpr uintptr_t ChildrenEnd = Instance::ChildrenEnd;
    inline constexpr uintptr_t ChildrenStart = Instance::ChildrenStart;
    inline constexpr uintptr_t Children = Instance::ChildrenStart;
    inline constexpr uintptr_t ClassBase = Instance::ClassBase;
    inline constexpr uintptr_t ClassDescriptor = Instance::ClassDescriptor;
    inline constexpr uintptr_t ClassName = Instance::ClassName;
    inline constexpr uintptr_t ClassDescriptorToClassName = Instance::ClassName;
    inline constexpr uintptr_t Name = Instance::Name;
    inline constexpr uintptr_t NameSize = Instance::Name;
    inline constexpr uintptr_t Parent = Instance::Parent;

    // Lighting
    inline constexpr uintptr_t Ambient = Lighting::Ambient;
    inline constexpr uintptr_t Brightness = Lighting::Brightness;
    inline constexpr uintptr_t ClockTime = Lighting::ClockTime;
    inline constexpr uintptr_t ColorShift_Bottom = Lighting::ColorShift_Bottom;
    inline constexpr uintptr_t ColorShift_Top = Lighting::ColorShift_Top;
    inline constexpr uintptr_t ExposureCompensation = Lighting::ExposureCompensation;
    inline constexpr uintptr_t FogColor = Lighting::FogColor;
    inline constexpr uintptr_t FogEnd = Lighting::FogEnd;
    inline constexpr uintptr_t FogStart = Lighting::FogStart;
    inline constexpr uintptr_t GeographicLatitude = Lighting::GeographicLatitude;
    inline constexpr uintptr_t OutdoorAmbient = Lighting::OutdoorAmbient;

    // LocalScript
    inline constexpr uintptr_t LocalScriptByteCode = LocalScript::ByteCode;

    // MeshPart
    inline constexpr uintptr_t MeshId = MeshPart::MeshId;
    inline constexpr uintptr_t MeshPartTexture = MeshPart::Texture;
    inline constexpr uintptr_t Texture = MeshPart::Texture;

    // Misc
    inline constexpr uintptr_t Adornee = Misc::Adornee;
    inline constexpr uintptr_t AnimationId = Misc::AnimationId;
    inline constexpr uintptr_t RequireLock = Misc::RequireLock;
    inline constexpr uintptr_t RequireBypass = Misc::RequireLock;
    inline constexpr uintptr_t StringLength = Misc::StringLength;
    inline constexpr uintptr_t Value = Misc::Value;

    // Model
    inline constexpr uintptr_t PrimaryPart = Model::PrimaryPart;
    inline constexpr uintptr_t Scale = Model::Scale;

    // ModuleScript
    inline constexpr uintptr_t ModuleScriptByteCode = ModuleScript::ByteCode;

    // MouseService
    inline constexpr uintptr_t InputObject = MouseService::InputObject;
    inline constexpr uintptr_t MousePosition = MouseService::MousePosition;
    inline constexpr uintptr_t SensitivityPointer = MouseService::SensitivityPointer;
    inline constexpr uintptr_t MouseSensitivity = MouseService::SensitivityPointer;

    // Player
    inline constexpr uintptr_t CameraMode = Player::CameraMode;
    inline constexpr uintptr_t Country = Player::Country;
    inline constexpr uintptr_t DisplayName = Player::DisplayName;
    inline constexpr uintptr_t HumanoidDisplayName = Player::DisplayName;
    inline constexpr uintptr_t Gender = Player::Gender;
    inline constexpr uintptr_t LocalPlayer = Player::LocalPlayer;
    inline constexpr uintptr_t MaxZoomDistance = Player::MaxZoomDistance;
    inline constexpr uintptr_t CameraMaxZoomDistance = Player::MaxZoomDistance;
    inline constexpr uintptr_t MinZoomDistance = Player::MinZoomDistance;
    inline constexpr uintptr_t CameraMinZoomDistance = Player::MinZoomDistance;
    inline constexpr uintptr_t ModelInstance = Player::ModelInstance;
    inline constexpr uintptr_t Mouse = Player::Mouse;
    inline constexpr uintptr_t UserId = Player::UserId;
    inline constexpr uintptr_t CharacterAppearanceId = Player::UserId;

    // PlayerConfigurer
    inline constexpr uintptr_t OverrideDuration = PlayerConfigurer::OverrideDuration;
    inline constexpr uintptr_t PlayerConfigurerPointer = PlayerConfigurer::Pointer;

    // PlayerMouse
    inline constexpr uintptr_t Icon = PlayerMouse::Icon;
    inline constexpr uintptr_t WorkspacePtr = PlayerMouse::Workspace;

    // PrimitiveFlags
    inline constexpr uintptr_t Anchored = PrimitiveFlags::Anchored;
    inline constexpr uintptr_t AnchoredMask = PrimitiveFlags::Anchored;
    inline constexpr uintptr_t CanCollide = PrimitiveFlags::CanCollide;
    inline constexpr uintptr_t CanCollideMask = PrimitiveFlags::CanCollide;
    inline constexpr uintptr_t CanTouch = PrimitiveFlags::CanTouch;
    inline constexpr uintptr_t CanTouchMask = PrimitiveFlags::CanTouch;

    // ProximityPrompt
    inline constexpr uintptr_t ProximityPromptActionText = ProximityPrompt::ActionText;
    inline constexpr uintptr_t ProximityPromptEnabled = ProximityPrompt::Enabled;
    inline constexpr uintptr_t ProximityPromptGamepadKeyCode = ProximityPrompt::GamepadKeyCode;
    inline constexpr uintptr_t ProximityPromptHoldDuration = ProximityPrompt::HoldDuration;
    inline constexpr uintptr_t ProximityPromptHoldDuraction = ProximityPrompt::HoldDuration;
    inline constexpr uintptr_t KeyCode = ProximityPrompt::KeyCode;
    inline constexpr uintptr_t ProximityPromptMaxActivationDistance = ProximityPrompt::MaxActivationDistance;
    inline constexpr uintptr_t ProximityPromptMaxObjectText = ProximityPrompt::ObjectText;
    inline constexpr uintptr_t RequiresLineOfSight = ProximityPrompt::RequiresLineOfSight;

    // RenderView
    inline constexpr uintptr_t DeviceD3D11 = RenderView::DeviceD3D11;

    // RunService
    inline constexpr uintptr_t HeartbeatFPS = RunService::HeartbeatFPS;
    inline constexpr uintptr_t HeartbeatTask = RunService::HeartbeatTask;

    // Sky
    inline constexpr uintptr_t MoonAngularSize = Sky::MoonAngularSize;
    inline constexpr uintptr_t MoonTextureId = Sky::MoonTextureId;
    inline constexpr uintptr_t SkyboxBk = Sky::SkyboxBk;
    inline constexpr uintptr_t SkyboxDn = Sky::SkyboxDn;
    inline constexpr uintptr_t SkyboxFt = Sky::SkyboxFt;
    inline constexpr uintptr_t SkyboxLf = Sky::SkyboxLf;
    inline constexpr uintptr_t SkyboxOrientation = Sky::SkyboxOrientation;
    inline constexpr uintptr_t SkyboxRt = Sky::SkyboxRt;
    inline constexpr uintptr_t SkyboxUp = Sky::SkyboxUp;
    inline constexpr uintptr_t StarCount = Sky::StarCount;
    inline constexpr uintptr_t SunAngularSize = Sky::SunAngularSize;
    inline constexpr uintptr_t SunTextureId = Sky::SunTextureId;

    // SpecialMesh
    inline constexpr uintptr_t SpecialMeshId = SpecialMesh::MeshId;
    inline constexpr uintptr_t SpecialMeshScale = SpecialMesh::Scale;

    // StatsItem
    inline constexpr uintptr_t StatsValue = StatsItem::Value;

    // TaskScheduler
    inline constexpr uintptr_t FakeDataModelToDataModelTask = TaskScheduler::FakeDataModelToDataModel;
    inline constexpr uintptr_t JobEnd = TaskScheduler::JobEnd;
    inline constexpr uintptr_t JobName = TaskScheduler::JobName;
    inline constexpr uintptr_t Job_Name = TaskScheduler::JobName;
    inline constexpr uintptr_t JobStart = TaskScheduler::JobStart;
    inline constexpr uintptr_t MaxFPS = TaskScheduler::MaxFPS;
    inline constexpr uintptr_t TaskSchedulerMaxFPS = TaskScheduler::MaxFPS;
    inline constexpr uintptr_t TaskSchedulerPointer = TaskScheduler::Pointer;
    inline constexpr uintptr_t JobsPointer = TaskScheduler::Pointer;
    inline constexpr uintptr_t RenderJobToFakeDataModel = TaskScheduler::RenderJobToFakeDataModel;
    inline constexpr uintptr_t RenderJobToRenderView = TaskScheduler::RenderJobToRenderView;

    // Team
    inline constexpr uintptr_t BrickColor = Team::BrickColor;
    inline constexpr uintptr_t TeamColor = Team::BrickColor;

    // Textures
    inline constexpr uintptr_t DecalTexture = Textures::Decal_Texture;
    inline constexpr uintptr_t TextureTexture = Textures::Texture_Texture;
    inline constexpr uintptr_t SoundId = Textures::Decal_Texture;

    // VisualEngine
    inline constexpr uintptr_t Dimensions = VisualEngine::Dimensions;
    inline constexpr uintptr_t VisualEnginePointer = VisualEngine::Pointer;
    inline constexpr uintptr_t ToDataModel1 = VisualEngine::ToDataModel1;
    inline constexpr uintptr_t VisualEngineToDataModel1 = VisualEngine::ToDataModel1;
    inline constexpr uintptr_t ToDataModel2 = VisualEngine::ToDataModel2;
    inline constexpr uintptr_t VisualEngineToDataModel2 = VisualEngine::ToDataModel2;
    inline constexpr uintptr_t ViewMatrix = VisualEngine::ViewMatrix;
    inline constexpr uintptr_t viewmatrix = VisualEngine::ViewMatrix;

    // Workspace
    inline constexpr uintptr_t CurrentCamera = Workspace::CurrentCamera;
    inline constexpr uintptr_t DistributedGameTime = Workspace::DistributedGameTime;
    inline constexpr uintptr_t Gravity = Workspace::Gravity;
    inline constexpr uintptr_t GravityContainer = Workspace::GravityContainer;
    inline constexpr uintptr_t WorkspaceToWorld = Workspace::GravityContainer;
    inline constexpr uintptr_t PrimitivesPointer1 = Workspace::PrimitivesPointer1;
    inline constexpr uintptr_t PrimitivesPointer2 = Workspace::PrimitivesPointer2;
    inline constexpr uintptr_t ReadOnlyGravity = Workspace::ReadOnlyGravity;

    // Special pointer aliases
    inline constexpr uintptr_t DataModelDeleterPointer = FakeDataModel::Pointer;
    inline constexpr uintptr_t Deleter = Instance::Parent;
    inline constexpr uintptr_t DeleterBack = Instance::ChildrenEnd;
    inline constexpr uintptr_t OnDemandInstance = Instance::AttributeContainer;
    inline constexpr uintptr_t RootPartR15 = Player::ModelInstance;
    inline constexpr uintptr_t RootPartR6 = Player::ModelInstance;
    inline constexpr uintptr_t Sandboxed = LocalScript::ByteCode;
    inline constexpr uintptr_t RunContext = LocalScript::ByteCode;
    inline constexpr uintptr_t ViewportSize = Camera::Position;
    inline constexpr uintptr_t Ping = Player::Country;
    inline constexpr uintptr_t HealthDisplayDistance = Player::MaxZoomDistance;
    inline constexpr uintptr_t NameDisplayDistance = Player::MaxZoomDistance;
    inline constexpr uintptr_t Tool_Grip_Position = GuiObject::LayoutOrder;
    inline constexpr uintptr_t MeshPartColor3 = BasePart::Color3;
    inline constexpr uintptr_t AutoJumpEnabled = Humanoid::AutoRotate;
    inline constexpr uintptr_t BanningEnabled = DataModel::PlaceVersion;
    inline constexpr uintptr_t ForceNewAFKDuration = DataModel::PlaceVersion;
    inline constexpr uintptr_t InstanceCapabilities = GuiObject::BackgroundColor3;
}