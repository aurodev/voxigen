
#include "urho3DApp.h"

#include <Urho3D/Urho3D.h>

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>


Urho3DApp::Urho3DApp(Urho3D::Context *context):
Urho3D::Application(context)
{

}


void Urho3DApp::Setup()
{
    // Modify engine startup parameters
    engineParameters_["WindowTitle"]=GetTypeName();
    engineParameters_["LogName"]=GetSubsystem<Urho3D::FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+GetTypeName()+".log";
    engineParameters_["FullScreen"]=false;
    engineParameters_["WindowWidth"]=1280;
    engineParameters_["WindowHeight"]=720;
    engineParameters_["Headless"]=false;

    engineParameters_["ResourcePrefixPaths"]="D:/projects/voxel/Urho3D/bin";

    //Engine::ParseParameters(GetArguments());
}

void Urho3DApp::Start()
{
    m_frameCount=0;
    m_time=0.0;

//    if(Urho3D::GetPlatform()=="Android"||Urho3D::GetPlatform()=="iOS")
//        // On mobile platform, enable touch by adding a screen joystick
//        InitTouchInput();
//    else if(GetSubsystem<Input>()->GetNumJoysticks()==0)
//        // On desktop platform, do not detect touch when we already got a joystick
//        SubscribeToEvent(E_TOUCHBEGIN, HANDLER(Sample, HandleTouchBegin));

//    // Create logo
//    CreateLogo();
//
//    // Set custom window Title & Icon
//    SetWindowTitleAndIcon();

#ifdef NDEBUG
    // Create console and debug HUD
    CreateConsoleAndDebugHud();
#endif

    createScene();

    // Subscribe key down event
    SubscribeToEvent(Urho3D::E_KEYDOWN, URHO3D_HANDLER(Urho3DApp, onKeyDown));
    // Subscribe scene update event
    SubscribeToEvent(Urho3D::E_UPDATE, URHO3D_HANDLER(Urho3DApp, onUpdate));
}

void Urho3DApp::CreateConsoleAndDebugHud()
{
    // Get default style
    Urho3D::ResourceCache *cache=GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::XMLFile *xmlFile=cache->GetResource<Urho3D::XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Urho3D::Console *console=engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    Urho3D::DebugHud *debugHud=engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);
}


void Urho3DApp::Stop()
{
    engine_->DumpResources(true);
}


void Urho3DApp::createScene()
{
    Urho3D::ResourceCache *cache=GetSubsystem<Urho3D::ResourceCache>();
    
    cache->SetAutoReloadResources(true);
    
    Urho3D::Renderer *renderer=GetSubsystem<Urho3D::Renderer>();
    
    m_scene=new Urho3D::Scene(context_);

    // Create the Octree component to the scene. This is required before adding any drawable components, or else nothing will
    // show up. The default octree volume will be from (-1000, -1000, -1000) to (1000, 1000, 1000) in world coordinates; it
    // is also legal to place objects outside the volume but their visibility can then not be checked in a hierarchically
    // optimizing manner
    m_scene->CreateComponent<Urho3D::Octree>();
    
    Urho3D::DebugRenderer* debug=m_scene->CreateComponent<Urho3D::DebugRenderer>();
    
    m_scene->CreateComponent<Urho3D::PhysicsWorld>();


    // Create skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
    // illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
    // generate the necessary 3D texture coordinates for cube mapping
    Urho3D::Node *skyNode=m_scene->CreateChild("Sky");
    Urho3D::Skybox *skybox=skyNode->CreateComponent<Urho3D::Skybox>();
    
    skybox->SetModel(cache->GetResource<Urho3D::Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/Skybox.xml"));

    // Create a scene node for the camera, which we will move around
    // The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    m_cameraNode=m_scene->CreateChild("Camera");
    
    Urho3D::Camera *camera=m_cameraNode->CreateComponent<Urho3D::Camera>();
    
    camera->SetFarClip(2000.0);

    // Set an initial position for the camera scene node above the plane
//    m_cameraNode->SetPosition(Urho3D::Vector3(0.0, 50.0, 0.0));

    // Create a red directional light (sun)
    Urho3D::Node *sunNode=m_scene->CreateChild();

    sunNode->SetDirection(Urho3D::Vector3::FORWARD);
    sunNode->Yaw(50);     // horizontal
    sunNode->Pitch(10);   // vertical
        
    Urho3D::Light *sun=sunNode->CreateComponent<Urho3D::Light>();

    sun->SetLightType(Urho3D::LIGHT_DIRECTIONAL);
    sun->SetBrightness(1.6);
    sun->SetColor(Urho3D::Color(1.0, .6, 0.3, 1));
    sun->SetCastShadows(true);

    //Create a directional light to the world so that we can see something. The light scene node's orientation controls the
    //light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
    //The light will use default settings (white light, no shadows)
    Urho3D::Node *lightNode=skyNode->CreateChild("DirectionalLight");
    
    lightNode->SetDirection(Urho3D::Vector3(0.8f, -0.2f, 0.8f)); // The direction vector does not need to be normalized


    Urho3D::Zone *zone=skyNode->CreateComponent<Urho3D::Zone>();
    
    zone->SetBoundingBox(Urho3D::BoundingBox(Urho3D::Vector3(-100000.0f, 0.0f, -100000.0f), Urho3D::Vector3(100000.0f, 128.0f, 100000.0f)));
    zone->SetAmbientColor(Urho3D::Color(0.7f, 0.7f, 0.7f));
    //zone->SetHeightFog(true);
    //zone->SetAmbientGradient(true);
    zone->SetFogColor(Urho3D::Color(0.3f, 0.3f, 0.3f));
    zone->SetFogStart(400.0f);
    zone->SetFogEnd(900.0f);

//    Urho3D::Renderer *renderer=GetSubsystem<Urho3D::Renderer>();
    Urho3D::SharedPtr<Urho3D::Viewport> viewport(new Urho3D::Viewport(context_, m_scene, m_cameraNode->GetComponent<Urho3D::Camera>()));

    renderer->SetViewport(0, viewport);
}

void Urho3DApp::onBeginFrame(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{

}

void Urho3DApp::onKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
}

void Urho3DApp::onUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    float timeStep=eventData[Urho3D::Update::P_TIMESTEP].GetFloat();

    m_frameCount++;
    m_time+=timeStep;
    // Movement speed as world units per second
    float MOVE_SPEED=10.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY=0.1f;

    Urho3D::Input *input=GetSubsystem<Urho3D::Input>();

    if(input->GetKeyDown('W'))
        m_cameraNode->Translate(Urho3D::Vector3(0, 0, 1)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown('S'))
        m_cameraNode->Translate(Urho3D::Vector3(0, 0, -1)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown('A'))
        m_cameraNode->Translate(Urho3D::Vector3(-1, 0, 0)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown('D'))
        m_cameraNode->Translate(Urho3D::Vector3(1, 0, 0)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown(Urho3D::KEY_ESCAPE))
        engine_->Exit();

    if(!input->IsMouseVisible())
    {
        // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
        Urho3D::IntVector2 mouseMove=input->GetMouseMove();
        static float yaw_=0;
        static float pitch_=0;

        yaw_+=MOUSE_SENSITIVITY*mouseMove.x_;
        pitch_+=MOUSE_SENSITIVITY*mouseMove.y_;
        pitch_=Urho3D::Clamp(pitch_, -90.0f, 90.0f);
        // Reset rotation and set yaw and pitch again
        m_cameraNode->SetDirection(Urho3D::Vector3::FORWARD);
        m_cameraNode->Yaw(yaw_);
        m_cameraNode->Pitch(pitch_);
    }

}

void Urho3DApp::onClosePressed(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    engine_->Exit();
}

URHO3D_DEFINE_APPLICATION_MAIN(Urho3DApp)