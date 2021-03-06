#include "GameObject.h"

GameObject::GameObject()
{
	mName = "";
	mSceneManager = NULL;
	mSceneNode = NULL;
	isActive = true;
}

GameObject::GameObject(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager)
{
	mName = name;
	mSceneManager = manager;
	mSceneNode = mSceneManager->getSceneNode(name);

	InitRigidBody();
}

GameObject::~GameObject()
{

}

void GameObject::InitRigidBody()
{
	//if mass is zero it counts as infinite
	float mass = 1.0f;

	btQuaternion q = OgreToBtQuaternion(mSceneNode->getOrientation());
	btVector3 v = OgreToBtVector3(mSceneNode->getPosition());
	
	btTransform transform = btTransform(q, v);
	
	btCollisionShape* boxShape = new btBoxShape(btVector3(1,1,1));
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(transform);
	btVector3 boxInertia(0,0,0);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass,boxMotionState,boxShape,boxInertia);
	mRigidBody = new btRigidBody(boxRigidBodyCI);
}

void GameObject::Update()
{
	//Mode the scene node to match the rigidbody position
	btTransform trans = mRigidBody->getWorldTransform();
	mSceneNode->setOrientation(BtToOgreQuaternion(&trans.getRotation()));
	mSceneNode->setPosition(BtToOgreVector3(&trans.getOrigin()));
}