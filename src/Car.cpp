#include "Car.h"

Car::Car(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, btDiscreteDynamicsWorld* mWorld, Ogre::String carEntName, int ID)
{
	mName = name;
	mSceneManager = manager;
	
	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(name);
	Ogre::Entity* someEnt = mSceneManager->createEntity(carEntName);
	mSceneNode->attachObject(someEnt);

	Ogre::Matrix4 t = mSceneManager->getSceneNode("triggerFinishLine")->_getFullTransform();
	Ogre::Vector3 forward = Ogre::Vector3(t[2][0], t[2][1], t[2][2]);

	Ogre::Vector3 pos = Ogre::Vector3(t[0][3], t[1][3], t[2][3]);
	pos += forward.normalisedCopy() * (ID * 20.0f);
	pos.y += 5;
	mSceneNode->translate(pos);

	world = mWorld;

	lapCounter = 0;
	checkPointsHit = 0;

	steerValue = 0.0f;

	InitRigidBody();
}

Car::~Car()
{
}

void Car::MoveForward(float distance)
{
	btScalar mat[16];
	GetRigidBody()->getWorldTransform().getOpenGLMatrix(mat);

	btVector3 forward = btVector3(mat[8], mat[9], mat[10]);

	forward *= -distance;

	GetRigidBody()->activate();
	GetRigidBody()->setLinearVelocity(forward);
}

void Car::MoveBackward(float distance)
{
	MoveForward(-distance/2.0f);
}

void Car::TurnRight(float value)
{
	GetRigidBody()->activate();
	GetRigidBody()->setAngularVelocity(btVector3(0.0f, -value, 0.0f));
}
void Car::TurnLeft(float value)
{
	TurnRight(-value);
}

void Car::Update(void)
{
	world->debugDrawWorld();
	for (int i = 0; i < m_vehicle->getNumWheels(); ++i){
		m_vehicle->updateWheelTransform(i, true);
	}

	float maxSpeed = 100;
	float currentSpeed = mRigidBody->getLinearVelocity().norm();
	float speedRatio = 0.01f;
	if (currentSpeed != 0){
		speedRatio = currentSpeed / maxSpeed;
	}

	//Move the car if the button is down
	if (mCanMoveForward != 0.0f)
	{
		if (mCanMoveForward > 1.0f)
			mCanMoveForward = 1.0f;
		engineForce = (-1000.0f / speedRatio) * mCanMoveForward;
	}
	else if (mCanMoveBackward != 0.0f)
	{
		if (mCanMoveBackward > 1.0f)
			mCanMoveBackward = 1.0f;
		engineForce = (600) * mCanMoveBackward;
	}
	else 
	{
		engineForce = 0;
	}

	if (mTurning != 0.0f)
	{
		steerValue += (0.002f * mTurning);

		if (steerValue > 0.1f)
			steerValue = 0.1f;
		else if (steerValue < -0.1f)
			steerValue = -0.1f;
	}
	else
	{
		steerValue *= 0.97;
	}

	int wheelIndex = 2;
	m_vehicle->applyEngineForce(engineForce, wheelIndex);
	m_vehicle->setBrake(100, wheelIndex);
	wheelIndex = 3;
	m_vehicle->applyEngineForce(engineForce, wheelIndex);
	m_vehicle->setBrake(100, wheelIndex);


	wheelIndex = 0;
	m_vehicle->setSteeringValue(steerValue, wheelIndex);
	wheelIndex = 1;
	m_vehicle->setSteeringValue(steerValue, wheelIndex);

	ghost->setWorldTransform(mRigidBody->getWorldTransform());

	GameObject::Update();

	mSceneNode->roll(Ogre::Radian(-steerValue*2));
}

void Car::InitRigidBody()
{
	//if mass is zero it counts as infinite
	float mass = 50.0f;

	btQuaternion q = OgreToBtQuaternion(mSceneNode->getOrientation());
	btVector3 v = OgreToBtVector3(mSceneNode->getPosition());
	//v.setY(v.getY() - 100);

	btTransform transform = btTransform(q, v);

	ghost = new btPairCachingGhostObject();
	ghost->setWorldTransform(transform);
	

	btCollisionShape* boxShape = new btBoxShape(btVector3(20, 5, 20));
	btCompoundShape* compound = new btCompoundShape();
	mCollisionShapes.push_back(boxShape);
	mCollisionShapes.push_back(compound);

	ghost->setCollisionShape(boxShape);
	ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	compound->addChildShape(transform, boxShape);

	btDefaultMotionState* boxMotionState = new btDefaultMotionState(transform);
	btVector3 boxInertia(0, 0, 0);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, compound, boxInertia);
	
	mRigidBody = new btRigidBody(boxRigidBodyCI);
	mRigidBody->setActivationState(DISABLE_DEACTIVATION);
	world->addRigidBody(mRigidBody);
	world->addCollisionObject(ghost);
	

	
	float connectionHeight = 2.f;
	float wheelWidth = 5.f;
	float wheelRadius = 2.f;
	int rightIndex = 0;
	int upIndex = 1;
	int forwardIndex = 2;
	btVector3 wheelDirection(0, -1, 0);
	btVector3 wheelAxle(-1, 0, 0);
	float suspensionRestLength = 4.f;
	bool isFrontWheel = true;

	btCollisionShape* wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));

	m_vehicleRayCaster = new btDefaultVehicleRaycaster(world);
	m_vehicle = new btRaycastVehicle(m_tuning, mRigidBody, m_vehicleRayCaster);

	world->addVehicle(m_vehicle);

	m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);
	btVector3 connectionPoint((10.f - 0.3f*wheelWidth), connectionHeight, (10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	connectionPoint = btVector3((-10.f - 0.3f*wheelWidth), connectionHeight, (10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	connectionPoint = btVector3((-10.f - 0.3f*wheelWidth), connectionHeight, (-10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, false);
	connectionPoint = btVector3((10.f - 0.3f*wheelWidth), connectionHeight, (-10.f - wheelRadius));
	m_vehicle->addWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, false);

	float   wheelFriction = 2000;
	float   suspensionStiffness = 20.f;
	float   suspensionDamping = 4.3f;
	float   suspensionCompression = 10.4f;
	float   rollInfluence = 0.1f;


	for (int i = 0; i < m_vehicle->getNumWheels(); ++i){
		btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
	}
	
}