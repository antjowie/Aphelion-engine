#include "PhysicsDemoLayer.h"

#include "Aphelion/Core/Application.h"
#include "Aphelion/Core/Input/KeyCodes.h"
#include "Aphelion/Core/Event/KeyEvent.h"

#include "Aphelion/Renderer/Primitive.h"
#include "Aphelion/Renderer/Renderer.h"
#include "Aphelion/Renderer/Texture.h"

#include "Aphelion/Physics/PhysicsFoundation.h"
#include "Aphelion/Physics/PhysicsShape.h"
#include "Aphelion/Physics/PhysicsScene.h"

//#define USE_PX 

#ifdef USE_PX
#include <PxPhysicsAPI.h>
#include <ctype.h>
// Taken from PhysX snippet: SnippetHelloWorld
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}
#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define MAX_NUM_ACTOR_SHAPES 128
using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxReal stackZ = 10.0f;
#else
ap::PhysicsScene* scene;
ap::PhysicsMaterial* material;
float stackZ = 10.0f;
#endif // 0

#ifdef USE_PX
PxRigidDynamic* createDynamic(const PxTransform & t, const PxGeometry & geometry, const PxVec3 & velocity = PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}
#else
void createDynamic(const ap::Transform& t)
{
	//ap::RigidBody dynamic = 
	//	ap::RigidBody::CreateDynamic(
	//		ap::PhysicsShape{ ap::PhysicsGeometry::CreateSphere(10.f), *material }, 
	//		10.f, 
	//		glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 40, 100)));

	//
	//dynamic.SetAngularDamping(0.5f);
	//dynamic.SetLinearVelocity(glm::vec3(0, -50, -100));
	//scene->AddActor(dynamic);
	ap::RigidBody dynamic =
		ap::RigidBody::CreateDynamic(
			ap::PhysicsShape{ ap::PhysicsGeometry::CreateSphere(10.f), *material },
			10.f,t.GetWorldMatrix());

	dynamic.SetAngularDamping(0.5f);
	dynamic.SetLinearVelocity(t.GetForward() * -100.f);// glm::vec3(0, -50, -100));
	scene->AddActor(dynamic);
}
#endif // USE_PX

#ifdef USE_PX
void createStack(const PxTransform & t, PxU32 size, PxReal halfExtent)
{

	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}
#else
void createStack(const glm::mat4& t, unsigned size, float halfExtent)
{
	//PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	auto shape = ap::PhysicsShape(ap::PhysicsGeometry::CreateBox(glm::vec3(halfExtent)), *material);

	for (unsigned i = 0; i < size; i++)
	{
		for (unsigned j = 0; j < size - i; j++)
		{

			glm::mat4 tm = glm::translate(
				glm::identity<glm::mat4>(), 
				glm::vec3(
					float(j * 2) - float(size - i), 
					float(i * 2 + 1),
					0)
				* halfExtent);

			auto body = ap::RigidBody::CreateDynamic(shape,1.f,tm * t);

			//physx::PxRigidBodyExt::updateMassAndInertia(body.GetHandle(), 10.0f);
			scene->AddActor(body);
		}
	}
	//shape->release();
}
#endif

PhysicsDemoLayer::PhysicsDemoLayer() :
	m_camera(ap::Radians(45.f),16.f/9.f)
{
	m_camera.GetCamera().transform.Move(ap::Transform::GetWorldForward() * 80.f);
	m_camera.GetCamera().transform.Move(ap::Transform::GetWorldRight() * 30.f);
	m_camera.GetCamera().transform.Move(ap::Transform::GetWorldUp() * 60.f);
	m_camera.GetCamera().transform.LookAt(glm::vec3(-20,0,-100));
}

void PhysicsDemoLayer::OnAttach()
{
	AP_WARN("Attaching");
	ap::Timer timer;
#ifdef USE_PX
	//gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gFoundation = &PxGetFoundation();

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);

	for (PxU32 i = 0; i < 5; i++)
		createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

	createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
#else
	// Set up the foundation
	//ap::PhysicsFoundationDesc desc;
	//desc.logCb = [](ap::PhysicsErrorCode code, const char* message, const char* file, int line)
	//{
	//	AP_WARN("[Physics {} File: {} Line: {}] {})", code,file,line, message);
	//};
	//desc.cores = 2;
	//ap::PhysicsFoundation::Init(desc);

	ap::PhysicsSceneDesc sceneDesc;
	sceneDesc.gravity = glm::vec3(0.f, -9.81f, 0.f);

	scene = new ap::PhysicsScene(sceneDesc);
	material = new ap::PhysicsMaterial(0.5f, 0.5f, 0.6f);
	
	// Rigid bodies, volumes and collision triggers are actors. They are built from shapes which are built from geometry.
	// (The shape can be compared to collider component in Unity (I think))
	// We skip the shape class and just construct them in the actor
	//ap::RigidStatic groundPlane = ap::RigidStatic(ap::PhysicsGeometry::CreatePlane(), *material, glm::identity<glm::mat4>());
	ap::RigidBody groundPlane = PxCreatePlane(PxGetPhysics(), physx::PxPlane(0, 1, 0, 0), *material->GetHandle());

	scene->AddActor(groundPlane);

	for (unsigned i = 0; i < 5; i++)
		// Note: the normal half extent for cube is 0.5 but physx doesn't take scale for transform, only pos and rot
		createStack(glm::translate(glm::identity<glm::mat4>(),glm::vec3(0, 0, stackZ -= 10.0f)), 10, 2.f);


	//createDynamic

#endif // 0
	AP_INFO("Attaching DONE {}", timer.Elapsed());
}

void PhysicsDemoLayer::OnDetach() 
{
#ifdef USE_PX


	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
#else
	delete material;
	delete scene; // This releases the scene and sets it to an invalid scene
	//ap::PhysicsFoundation::Deinit();
#endif // USE_PX
}

void PhysicsDemoLayer::OnEvent(ap::Event& event)
{
	m_camera.OnEvent(event);
	ap::EventDispatcher d(event);
	d.Dispatch<ap::KeyPressedEvent>([&](ap::KeyPressedEvent& e)
		{
			if (e.GetKeyCode() == ap::KeyCode::Escape) ap::Application::Get().Exit();

#ifndef USE_PX


			if (e.GetKeyCode() == ap::KeyCode::Space) createDynamic(m_camera.GetCamera().transform);
			if (e.GetKeyCode() == ap::KeyCode::Z) 
			createStack(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, stackZ -= 10.0f)), 10, 2.f);
#endif // !USE_PX

			return false;
		});
}

#ifdef USE_PX
void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows/*, const PxVec3& color*/)
{
	const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
	const PxReal shadowMat[] = { 1,0,0,0, -shadowDir.x / shadowDir.y,0,-shadowDir.z / shadowDir.y,0, 0,0,1,0, 0,0,0,1 };

	static auto cube = ap::CreateCube();
	static auto shader = ap::Shader::Create("res/shader/Texture3D.glsl");
	static auto texture = ap::Texture2D::Create(1,1);
	static auto textureSleep = ap::Texture2D::Create(1, 1);

	unsigned white = 0xccccccff;
	unsigned grey = 0x505050ff;
	texture->SetData(reinterpret_cast<void*>(&white), sizeof(white));
	textureSleep->SetData(reinterpret_cast<void*>(&grey), sizeof(grey));
	//texture->Bind();

	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	for (PxU32 i = 0; i < numActors; i++)
	{
		const PxU32 nbShapes = actors[i]->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actors[i]->getShapes(shapes, nbShapes);
		const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

		for (PxU32 j = 0; j < nbShapes; j++)
		{
			const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
			const PxGeometryHolder h = shapes[j]->getGeometry();

			if (sleeping)
				textureSleep->Bind();
			else
				texture->Bind();

			ap::Renderer::Submit(shader, cube, glm::scale(glm::make_mat4(shapePose.front()),glm::vec3(4)));
		}
	}
}
#endif

void PhysicsDemoLayer::OnUpdate(ap::Timestep ts) 
{
	//AP_WARN("Update...");
	ap::Timer timer;


#ifdef USE_PX

    gScene->simulate(ts);
    gScene->fetchResults(true);

	m_camera.OnUpdate(ts);

	ap::Renderer::BeginScene(m_camera.GetCamera());
	//ap::Renderer::Submit(shader, cube);

	PxScene* scene;
	PxGetPhysics().getScenes(&scene, 1);
	PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
		renderActors(&actors[0], static_cast<PxU32>(actors.size()), true);
	}

	ap::Renderer::EndScene();
#else
	static auto cube = ap::CreateCube();
	static auto shader = ap::Shader::Create("res/shader/Texture3D.glsl");
	static auto texture = ap::Texture2D::Create(1,1);
	static auto textureSleep = ap::Texture2D::Create(1, 1);

	unsigned white = 0xccccccff;
	unsigned grey = 0x505050ff;
	texture->SetData(reinterpret_cast<void*>(&white), sizeof(white));
	textureSleep->SetData(reinterpret_cast<void*>(&grey), sizeof(grey));
	//AP_TRACE("Created vars {}", timer.Reset());

	scene->Simulate(ts);
	m_camera.OnUpdate(ts);
	//AP_TRACE("Simulated scene {}", timer.Reset());

	ap::Renderer::BeginScene(m_camera.GetCamera());
	//ap::Renderer::Submit(shader, cube);
	//texture->Bind();

	for(const auto& actor : scene->GetActors(ap::RigidBodyType::AllMask))
	{
		if (actor.IsSleeping())
			textureSleep->Bind();
		else
		texture->Bind();

		// TEMP: The scale is half ext / 0.5 (see create stack)
		//float scale = 4.f;
		auto bounds = actor.GetWorldBounds();
		
		ap::Renderer::Submit(shader, cube, actor.GetWorldTransform() * glm::scale(glm::mat4(1),bounds.GetDimensions()));	
	}

	ap::Renderer::EndScene();
	//AP_TRACE("Rendererd scene {}", timer.Reset());

#endif // USE_PX

	//AP_INFO("Update DONE {}", timer.Total());
}