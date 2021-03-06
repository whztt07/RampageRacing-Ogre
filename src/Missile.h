#pragma once

#include "GameObject.h"

class Missile:public GameObject
{
public:
	Missile(Ogre::String name, std::shared_ptr<Ogre::SceneManager> manager, Ogre::SceneNode* parent, btRigidBody* carID);
	~Missile();

	virtual void Update();

	inline void setVelocity(float x, float y, float z){ mVelocity = btVector3(x,y,z); }

private:
	virtual void InitRigidBody();

public:
	btVector3 mVelocity;
};