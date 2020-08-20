#include "PhysicsDemoLayer.h"
#include "Aphelion/Renderer/Primitive.h"
#include "Aphelion/Renderer/Renderer.h"
#include "Aphelion/Renderer/Texture.h"

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

PxRigidDynamic* createDynamic(const PxTransform & t, const PxGeometry & geometry, const PxVec3 & velocity = PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

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
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

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

	//if (!interactive)
		createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
}

void PhysicsDemoLayer::OnDetach() 
{
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
}

void PhysicsDemoLayer::OnEvent(ap::Event& event)
{
	m_camera.OnEvent(event);
}

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

			ap::Renderer::Submit(shader, cube, glm::scale(glm::make_mat4(shapePose.front()),glm::vec3(2)));
			//if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			
			// render object
			//glPushMatrix();
			//glMultMatrixf(&shapePose.column0.x);
			//if (sleeping)
			//{
				//const PxVec3 darkColor = color * 0.25f;
				//glColor4f(darkColor.x, darkColor.y, darkColor.z, 1.0f);
			//}
			//else
				//glColor4f(color.x, color.y, color.z, 1.0f);
			//renderGeometryHolder(h);
			//glPopMatrix();

			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			//if (shadows)
			//{
			//	glPushMatrix();
			//	glMultMatrixf(shadowMat);
			//	glMultMatrixf(&shapePose.column0.x);
			//	glDisable(GL_LIGHTING);
			//	glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
			//	renderGeometryHolder(h);
			//	glEnable(GL_LIGHTING);
			//	glPopMatrix();
			//}
		}
	}
}

void PhysicsDemoLayer::OnUpdate(ap::Timestep ts) 
{
    gScene->simulate(1.0f / 60.0f);
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
}