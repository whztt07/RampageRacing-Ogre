#include "Physics.h"

PhysicsWorld::PhysicsWorld(){
	 
}
PhysicsWorld::~PhysicsWorld()
{
	delete solver;
	delete groundShape;
	delete groundMotionState;
	delete groundRigidBody;
	delete broadphase;
	delete dispatcher;
	delete collisionConfiguration;
}

void PhysicsWorld::initWorld(){
	

	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	solver = new btSequentialImpulseConstraintSolver;

	sweepBP = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
	broadphase = sweepBP;

	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//gravity set here
	world->setGravity(btVector3(0,-100,0));

	//set floor as horizontal plane centered at 0,0,0
	groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -50, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
    world->addRigidBody(groundRigidBody);

	sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

void PhysicsWorld::addBodyToWorld(btRigidBody* newBody){
	world->addRigidBody(newBody);
}

void PhysicsWorld::updateWorld(float timeStep){
	world->stepSimulation(timeStep, 5, 1.f/60.f);
}

void PhysicsWorld::deleteWorld(){
	//TODO clean up all those pointers you have lying around
}

void PhysicsBody::initCar(){}//TODO
void PhysicsBody::initTrackSegment(){}//TODO
btRigidBody* PhysicsBody::initBox(btScalar mass, btVector3& pos, btQuaternion& rot){
	//if mass is zero it counts as infinite
	boxShape = new btBoxShape(btVector3(1, 1, 1));
	boxMotionState = new btDefaultMotionState(btTransform(rot, pos));
	btVector3 boxInertia(0,0,0);
	//boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass,boxMotionState,boxShape,boxInertia);
	boxRigidBody = new btRigidBody(boxRigidBodyCI);

	boxRigidBody->setActivationState(DISABLE_DEACTIVATION);

	return boxRigidBody;
}
void PhysicsBody::initSphere(){}

