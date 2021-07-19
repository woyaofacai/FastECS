/*
---------------------------------------------------------------------------
FastECS (Fast Entity-Component-System Library)
---------------------------------------------------------------------------

Original code by edisongao
contact: ed8266@gmail.com / 937907032@qq.com

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the FastECS team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the FastECS team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

#pragma once
#include "MathLib.hpp"

#if !defined(_MSC_VER) && !defined(__STDC_LIB_EXT1__)  
#define sprintf_s sprintf
#define strcpy_s strcpy
#endif

// Component Index Table Type, the values you can specify:
// 0: use sequential array, the search speed is log(n), n is the number of components that the current entity has
// 1: use hashmap as a container, with the same search speed as std::unordered_map
// 2: use direct array, the search speed is log(1), but remember to give the component and unique id using DefineComponentWithID
//	  instead of DefineComponent when you define your own component, and make sure this id doesn't excceed
//    FASTECS_MAX_COMPONENT_COUNT whose default value is 128.
#define FASTECS_COMPONENT_INDEX_TABLE_TYPE 2

// if FASTECS_COMPONENT_INDEX_TABLE_TYPE is set to 2, then enable FASTECS_USE_CUSTOM_COMPONENT_TYPE_ID
#if FASTECS_COMPONENT_INDEX_TABLE_TYPE == 2
#define FASTECS_USE_CUSTOM_COMPONENT_TYPE_ID 1
#endif
// 

// Maximum count of entities in each chunk is (1 << FASTECS_MAX_BLOCK_COUNT_BITS)
#define FASTECS_MAX_BLOCK_COUNT_BITS 10

// Enable dynamic assert in Debug mode 
#define FASTECS_ASSERT_ENABLE 1

#include "../FastECS.hpp"

using namespace FastECS;

// If you want to use direct array as component table for better performance
// you ought to use DefineComponentWithID  instead of DefineComponent and give it an unique id.

#if FASTECS_COMPONENT_INDEX_TABLE_TYPE != 2
DefineComponent(Profile)
{
	char	name[128] = { 0 };
	int		age = 0;

	Profile(){}
	inline Profile(const char* name, int age);
	inline bool operator==(const Profile& other) const;
	inline bool operator!=(const Profile& other) const;
};

DefineComponent(Transform)
{
	Vector3		position;
	Vector3		scale;
	float		yaw = 0;

	Transform(){}
	inline Transform(const Vector3& p, const Vector3& scale, float yaw);
	inline bool operator==(const Transform& other) const;
	inline bool operator!=(const Transform& other) const;
};

DefineComponent(Velocity)
{
	Vector3		Direction;
	float		Magnitude = 0.0f;

	Velocity(){}
	inline Velocity(const Vector3& dir, float mag);
	inline bool operator==(const Velocity& other) const;
	inline bool operator!=(const Velocity& other) const;
};

#else

DefineComponentWithID(Profile, 1)
{
	char	name[128] = { 0 };
	int		age = 0;

	Profile() {}
	inline Profile(const char* name, int age);
	inline bool operator==(const Profile& other) const;
	inline bool operator!=(const Profile& other) const;
};

DefineComponentWithID(Transform, 2)
{
	Vector3		position;
	Vector3		scale;
	float		yaw = 0;

	Transform() {}
	inline Transform(const Vector3& p, const Vector3& scale, float yaw);
	inline bool operator==(const Transform& other) const;
	inline bool operator!=(const Transform& other) const;
};

DefineComponentWithID(Velocity, 3)
{
	Vector3		Direction;
	float		Magnitude = 0.0f;

	Velocity() {}
	inline Velocity(const Vector3& dir, float mag);
	inline bool operator==(const Velocity& other) const;
	inline bool operator!=(const Velocity& other) const;
};

// comment out the following code to replace the above definination
// for memory alignment testing
//struct alignas(64) Velocity : public FastECS::component_name_class<3, FASTECS_STR("Velocity")>
//{
//	Vector3		Direction;
//	float		Magnitude = 0.0f;
//
//	Velocity() {}
//	inline Velocity(const Vector3& dir, float mag);
//	inline bool operator==(const Velocity& other) const;
//	inline bool operator!=(const Velocity& other) const;
//};

#endif


Profile::Profile(const char* name, int age)
{
	strcpy_s(this->name, name);
	this->age = age;
}

bool Profile::operator==(const Profile& other) const
{
	return strcmp(name, other.name) == 0 && age == other.age;
}

bool Profile::operator!=(const Profile& other) const
{
	return !(*this == other);
}

Transform::Transform(const Vector3& p, const Vector3& scale, float yaw)
	:position(p), scale(scale), yaw(yaw)
{
}

bool Transform::operator==(const Transform& other) const
{
	return position == other.position
		&& scale == other.scale
		&& yaw == other.yaw;
}

bool Transform::operator!=(const Transform& other) const
{
	return !(*this == other);
}

Velocity::Velocity(const Vector3& dir, float mag)
	:Direction(dir), Magnitude(mag)
{
}

bool Velocity::operator==(const Velocity& other) const
{
	return Direction == other.Direction && Magnitude == other.Magnitude;
}

bool Velocity::operator!=(const Velocity& other) const
{
	return !(*this == other);
}

// define an entity class
// an actor contains three components: Profile, Transform, Velocity.
class ActorClass 
	: public EntityClass<Profile, Transform, Velocity> {};



