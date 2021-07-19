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

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

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

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "Common.hpp"
#include <atomic>
#include <thread>
using namespace FastECS;

float rand_float();
float rand_float(float a, float b);
int rand_int(int a, int b);
Vector3 rand_vector3(float a = 0.0f, float b = 1.0f);
Color rand_color();

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> gFloatDis(0.0f, 1.0f);

float rand_float()
{
	return gFloatDis(gen);
}

float rand_float(float a, float b)
{
	return a + (b - a) * gFloatDis(gen);
}

int rand_int(int a, int b)
{
	std::uniform_int_distribution<int> dis(a, b);
	return dis(gen);
}

Vector3 rand_vector3(float a, float b)
{
	return Vector3(rand_float(a, b), rand_float(a, b), rand_float(a, b));
}

Color rand_color()
{
	return Color(rand_float(), rand_float(), rand_float(), rand_float());
}

TEST_CASE("Test Archetypes", "[EntityArchetype]") {

	World* pWorld = World::GetInstance();

	SECTION("Archetypes with same components are identical")
	{
		EntityArchetype* pArchetype1 = pWorld->CreateArchetype<Transform>();
		EntityArchetype* pArchetype2 = pWorld->CreateArchetype<Transform>();

		EntityArchetype* pArchetype3 = pWorld->CreateArchetype<Transform, Profile>();
		// the order of components given doesn't matter
		EntityArchetype* pArchetype4 = pWorld->CreateArchetype<Profile, Transform>();

		REQUIRE(pArchetype1 == pArchetype2);
		REQUIRE(pArchetype3 == pArchetype4);
		REQUIRE(pArchetype1 != pArchetype3);
	}

	SECTION("An archetype must contain all components it specifies")
	{
		EntityArchetype* pArchetype = pWorld->CreateArchetype<Transform, Profile>();

		REQUIRE(pArchetype->ContainComponent<Transform>());
		REQUIRE(pArchetype->ContainComponent<Profile>());
		REQUIRE(!pArchetype->ContainComponent<Velocity>());
	}

	SECTION("Create Archetype through an entity class")
	{
		EntityArchetype* pArchetype1 = pWorld->CreateArchetype<ActorClass>();
		EntityArchetype* pArchetype2 = pWorld->CreateArchetype<Transform, Profile, Velocity>();
		REQUIRE(pArchetype1 == pArchetype2);
	}
}

TEST_CASE("Create Entity in different ways", "[CreateEntity]")
{
	World* pWorld = World::GetInstance();
	EntityContext* pContext = pWorld->CreateContext();
	EntityArchetype* pActorArchetype = pWorld->CreateArchetype<ActorClass>();

	Transform transform1(Vector3(1.0f, 2.0f, 3.0f), Vector3(10.0f, 10.0f, 10.0f), 90.0f);
	Profile profile1("Hello", 15);

	SECTION("Create entity by only giving an archetype")
	{
		Entity* pEntity1 = pContext->CreateEntity(pActorArchetype);
		REQUIRE(pActorArchetype == pEntity1->GetArchetype());

		REQUIRE(*pEntity1->GetComponent<Transform>() == Transform());
		REQUIRE(*pEntity1->GetComponent<Profile>() == Profile());
		REQUIRE(*pEntity1->GetComponent<Velocity>() == Velocity());
	}

	SECTION("Create entity by giving an archetype and component values")
	{
		Entity* pEntity1 = pContext->CreateEntity(pActorArchetype, transform1, profile1);
		REQUIRE(pActorArchetype == pEntity1->GetArchetype());

		REQUIRE(*pEntity1->GetComponent<Transform>() == transform1);
		REQUIRE(*pEntity1->GetComponent<Profile>() == profile1);
		REQUIRE(*pEntity1->GetComponent<Velocity>() == Velocity());
	}

	SECTION("Create entity by giving several component types")
	{
		Entity* pEntity1 = pContext->CreateEntity<Transform, Profile, Velocity>();
		REQUIRE(pActorArchetype == pEntity1->GetArchetype());

		REQUIRE(*pEntity1->GetComponent<Transform>() == Transform());
		REQUIRE(*pEntity1->GetComponent<Profile>() == Profile());
		REQUIRE(*pEntity1->GetComponent<Velocity>() == Velocity());
	}

	SECTION("Create entity by giving an entity class")
	{
		Entity* pEntity1 = pContext->CreateEntity<ActorClass>();
		REQUIRE(pActorArchetype == pEntity1->GetArchetype());

		REQUIRE(*pEntity1->GetComponent<Transform>() == Transform());
		REQUIRE(*pEntity1->GetComponent<Profile>() == Profile());
		REQUIRE(*pEntity1->GetComponent<Velocity>() == Velocity());
	}

	SECTION("Create entity by giving component types and passing component objects")
	{
		Entity* pEntity1 = pContext->CreateEntity<Transform, Profile, Velocity>(transform1, profile1);
		Entity* pEntity2 = pContext->CreateEntity<ActorClass>(transform1, profile1);

		REQUIRE(*pEntity1->GetComponent<Transform>() == transform1);
		REQUIRE(*pEntity1->GetComponent<Profile>() == profile1);
		REQUIRE(*pEntity1->GetComponent<Velocity>() == Velocity());

		REQUIRE(*pEntity2->GetComponent<Transform>() == transform1);
		REQUIRE(*pEntity2->GetComponent<Profile>() == profile1);
		REQUIRE(*pEntity2->GetComponent<Velocity>() == Velocity());

		REQUIRE(pEntity1->GetComponent<Transform>() != pEntity2->GetComponent<Transform>());
		REQUIRE(pEntity1->GetComponent<Profile>() != pEntity2->GetComponent<Profile>());
		REQUIRE(pEntity1->GetComponent<Velocity>() != pEntity2->GetComponent<Velocity>());
	}

	SECTION("Create entity by only passing component objects. Maybe the most convenient way to create an entity.")
	{
		Entity* pEntity1 = pContext->CreateEntity(transform1, profile1);
		EntityArchetype* pArchetype1 = pWorld->CreateArchetype<Transform, Profile>();
		REQUIRE(pEntity1->GetArchetype() == pArchetype1);
		REQUIRE(*pEntity1->GetComponent<Transform>() == transform1);
		REQUIRE(*pEntity1->GetComponent<Profile>() == profile1);
		REQUIRE(pEntity1->GetComponent<Velocity>() == nullptr);
	}

	SECTION("Create a large number of entities")
	{
		const int n = 1000 * 1000;
		std::vector<Profile> profiles;
		std::vector<Velocity> velocities;
		std::vector<EntityID> entityIds;
		for (int i = 0; i < n; i++)
		{
			char name[128];
			sprintf_s(name, "%s_%d", "entityName", i);
			Profile profile(name, rand_int(1, 100));
			Velocity velocity(rand_vector3(), rand_float());

			profiles.push_back(profile);
			velocities.push_back(velocity);

			// you can also pass an r-value
			Entity* pEntity = pContext->CreateEntity<ActorClass>(profile, velocity);
			entityIds.push_back(pEntity->GetEntityID());
		}

		int correctness = 1;
		for (int i = 0; i < n; i++)
		{
			Entity* pEntity = pContext->GetEntity(entityIds[i]);
			Entity* pEntity1 = pWorld->GetEntity(entityIds[i]);
			correctness &= (pEntity == pEntity1);
			correctness &= (int)(*pEntity->GetComponent<Transform>() == Transform());
			correctness &= (int)(*pEntity->GetComponent<Profile>() == profiles[i]);
			correctness &= (int)(*pEntity->GetComponent<Velocity>() == velocities[i]);
		}
		REQUIRE(correctness);
	}

	pContext->Release();
}

TEST_CASE("Operate components of an entity", "Component")
{
	World* pWorld = World::GetInstance();
	EntityContext* pContext = pWorld->CreateContext();

	Profile profile1("Hello", 25);
	Transform transform1(Vector3(1.0f, 2.0f, 3.0f), Vector3(10.0f, 10.0f, 10.0f), 90.0f);
	Velocity velocity1(Vector3(0.8f, 0.8f, 1.2f), 10);

	SECTION("Add new components to an entity, namely extend an existed entity")
	{
		Entity* pEntity1 = pContext->CreateEntity(profile1);
		REQUIRE(pEntity1->GetArchetype() == pWorld->CreateArchetype<Profile>());

		Entity* pEntity2 = pEntity1->Extend<Transform>();
		Entity* pEntity3 = pEntity1->Extend(transform1);

		REQUIRE(pEntity1 != pEntity2);
		REQUIRE(pEntity2 != pEntity3);
		REQUIRE(pEntity2->GetArchetype() == pWorld->CreateArchetype<Profile, Transform>());
		REQUIRE(pEntity3->GetArchetype() == pWorld->CreateArchetype<Profile, Transform>());
		REQUIRE(pEntity1->GetArchetype() == pWorld->CreateArchetype<Profile>());

		REQUIRE(pEntity1->GetComponent<Transform>() == nullptr);
		REQUIRE(*pEntity2->GetComponent<Profile>() == *pEntity1->GetComponent<Profile>());
		REQUIRE(*pEntity3->GetComponent<Profile>() == *pEntity1->GetComponent<Profile>());
		REQUIRE(*pEntity2->GetComponent<Transform>() == Transform());
		REQUIRE(*pEntity3->GetComponent<Transform>() == transform1);

		Entity* pEntity4 = pEntity1->Extend<Transform, Velocity>();
		Entity* pEntity5 = pEntity1->Extend(transform1, velocity1);
		REQUIRE(pEntity1 != pEntity4);
		REQUIRE(pEntity4 != pEntity5);
		REQUIRE(pEntity4->GetArchetype() == pWorld->CreateArchetype<ActorClass>());
		REQUIRE(pEntity5->GetArchetype() == pWorld->CreateArchetype<ActorClass>());
		REQUIRE(*pEntity4->GetComponent<Transform>() == Transform());
		REQUIRE(*pEntity5->GetComponent<Transform>() == transform1);
		REQUIRE(*pEntity4->GetComponent<Velocity>() == Velocity());
		REQUIRE(*pEntity5->GetComponent<Velocity>() == velocity1);
	}

	SECTION("Remove components from an entity")
	{
		Entity* pEntity1 = pContext->CreateEntity<ActorClass>(profile1, velocity1);
		Entity* pEntity2 = pEntity1->Remove<Transform>();
		REQUIRE(pEntity2->GetArchetype() == pWorld->CreateArchetype<Profile, Velocity>());
		REQUIRE(*pEntity2->GetComponent<Profile>() == profile1);
		REQUIRE(*pEntity2->GetComponent<Velocity>() == velocity1);
		REQUIRE(pEntity2->GetComponent<Transform>() == nullptr);
		
		Entity* pEntity3 = pEntity1->Remove<Transform, Profile>();
		REQUIRE(pEntity3->GetArchetype() == pWorld->CreateArchetype<Velocity>());
		
		REQUIRE(*pEntity3->GetComponent<Velocity>() == velocity1);
		REQUIRE(pEntity3->GetComponent<Profile>() == nullptr);
		REQUIRE(pEntity3->GetComponent<Transform>() == nullptr);
	}
	
	SECTION("Modify components of an entity")
	{
		Entity* pEntity1 = pContext->CreateEntity<ActorClass>(profile1, velocity1, transform1);
		REQUIRE(*pEntity1->GetComponent<Profile>() == profile1);
		REQUIRE(*pEntity1->GetComponent<Velocity>() == velocity1);
		REQUIRE(*pEntity1->GetComponent<Transform>() == transform1);

		Profile profile2("name2", 30);
		Velocity velocity2(Vector3(-1.0f, -1.0f, 0), 2.0f);
		Transform transform2(Vector3(-1.0f, 2.0f, -3.0f), Vector3(-4.0f, 4.5f, -3.5f), 25.0f);

		pEntity1->SetComponent(profile2);
		pEntity1->SetComponent(velocity2);
		pEntity1->SetComponent(std::move(transform2)); // try to use r-value

		REQUIRE(*pEntity1->GetComponent<Profile>() == profile2);
		REQUIRE(*pEntity1->GetComponent<Velocity>() == velocity2);
		REQUIRE(*pEntity1->GetComponent<Transform>() == transform2);
	}

	pContext->Release();
}

TEST_CASE("Test ForEach method", "ForEach")
{
	World* pWorld = World::GetInstance();
	EntityContext* pContext = pWorld->CreateContext();
	EntityArchetype* pActorArchetype = pWorld->CreateArchetype<ActorClass>();

	const int n = 10000;
	// total speed
	int totalAge[3] = { 0 };
	for (int i = 0; i < n; i++)
	{
		Profile profile("Test", rand_int(1, 100));
		Entity* pEntity = pContext->CreateEntity<ActorClass>(profile);
		totalAge[0] += profile.age;
	}

	for (int i = 0; i < n; i++)
	{
		Profile profile("Test", rand_int(1, 100));
		Entity* pEntity = pContext->CreateEntity<Profile, Transform>(profile);
		totalAge[1] += profile.age;
	}

	for (int i = 0; i < n; i++)
	{
		Profile profile("Test", rand_int(1, 100));
		Entity* pEntity = pContext->CreateEntity<Profile>(profile);
		totalAge[2] += profile.age;
	}

	// define a thread context struct, each thread has its own context
	struct ThreadLocalContext
	{
		int		sum = 0;
		int		count = 0;
	};

	SECTION("Test ForEach (by Calcuating the average speed of many entities")
	{
		// calculate average in two different ways
		// average1 represents the direct result
		// average2 stores the result calculated with a ForEach
		float average1 = 0;
		float average2 = 0;

		// use ForEach to count the total age
		int sum = 0;
		int count = 0;
		pContext->ForEach<Profile>([&sum, &count](Entity* pEntity, Profile* pProfile) {
			sum += pProfile->age;
			count += 1;
		});

		average1 = (float)(totalAge[0] + totalAge[1] + totalAge[2]) / (3 * n);
		average2 = (float)sum / count;
		REQUIRE(average1 == average2);

		sum = 0;
		count = 0;
		pContext->ForEach<Profile, Transform>([&sum, &count](Entity* pEntity, Profile* pProfile, Transform* pTransform) {
			sum += pProfile->age;
			count += 1;
		});
		average1 = (float)(totalAge[0] + totalAge[1]) / (2 * n);
		average2 = (float)sum / count;
		REQUIRE(average1 == average2);

		sum = 0;
		count = 0;
		pContext->ForEach<Profile, Transform, Velocity>([&sum, &count](Entity* pEntity, Profile* pProfile, Transform* p1, Velocity* p2) {
			sum += pProfile->age;
			count += 1;
		});
		average1 = (float)totalAge[0] / n;
		average2 = (float)sum / count;
		REQUIRE(average1 == average2);

		
	}

	SECTION("Test ForEachBatch (by Calcuating the average speed of many entities")
	{
		// calculate average in two different ways
		// average1 represents the direct result
		// average2 stores the result calculated with a ForEach
		float average1 = 0;
		float average2 = 0;

		// use ForEach to count the total age
		int sum = 0;
		int count = 0;

		sum = 0;
		count = 0;
		pContext->ForEachBatch<Profile>([&sum, &count](Entity* pEntity, int entityCount, Profile* pProfile) {
			for (int i = 0; i < entityCount; i++) {
				if (pEntity->IsValid()) {
					sum += pProfile->age;
					count += 1;
				}
				AdvancePointers(pEntity, pProfile);
			}
		});
		average1 = (float)(totalAge[0] + totalAge[1] + totalAge[2]) / (3 * n);
		average2 = (float)sum / count;
		REQUIRE(average1 == average2);

		sum = 0;
		count = 0;
		pContext->ForEachBatch<Profile, Transform>([&sum, &count](Entity* pEntity, int entityCount, Profile* pProfile, Transform* pTransform) {
			for (int i = 0; i < entityCount; i++) {
				if (pEntity->IsValid()) {
					sum += pProfile->age;
					count += 1;
				}
				AdvancePointers(pEntity, pProfile, pTransform);
			}
		});
		average1 = (float)(totalAge[0] + totalAge[1]) / (2 * n);
		average2 = (float)sum / count;
		REQUIRE(average1 == average2);
	}

	SECTION("Run a parallel ForEach encapsulated in a ParallelJob (group 1)")
	{
		const int threadCount = 4;
		float average1 = 0, average2 = 0;

		// The first template parameter is whether thread local context is used.
		// The second template is the type of thread local context
		// The rests are component types
		ParallelJob<true, ThreadLocalContext, Profile> job1([](ThreadLocalContext* pLocalContext, Entity* pEntity, Profile* pProfile) {
			pLocalContext->sum += pProfile->age;
			pLocalContext->count += 1;
		});

		// call Prepare before executing the job
		job1.Prepare(pContext, threadCount);

		// divide the job and dispatch it to 4 threads.
		ThreadLocalContext localContexts[threadCount];
		std::thread threads[threadCount];
		auto p = &localContexts[0];
		for (int i = 0; i < threadCount; i++) {
			threads[i] = std::thread([&localContexts, &job1, i]() {
				auto p1 = &localContexts[i];
				job1.Execute(&localContexts[i]);
			});
		}

		int sum = 0;
		int count = 0;
		for (int i = 0; i < threadCount; i++) {
			threads[i].join();
			sum += localContexts[i].sum;
			count += localContexts[i].count;
		}
		average1 = (float)(totalAge[0] + totalAge[1] + totalAge[2]) / (3 * n);
		average2 = (float)sum / count;
		REQUIRE(average1 == average2);


		{
			const int threadCount2 = 3;
			job1.Prepare(pContext, threadCount2);
			ThreadLocalContext localContexts2[threadCount2];
			std::thread threads2[threadCount2];
			auto p = &localContexts2[0];
			for (int i = 0; i < threadCount2; i++) {
				threads2[i] = std::thread([&localContexts2, &job1, i]() {
					auto p1 = &localContexts2[i];
					job1.Execute(&localContexts2[i]);
				});
			}

			int sum2 = 0;
			int count2 = 0;
			for (int i = 0; i < threadCount2; i++) {
				threads2[i].join();
				sum2 += localContexts2[i].sum;
				count2 += localContexts2[i].count;
			}
			float average3 = (float)sum / count;
			REQUIRE(average1 == average3);
		}
	}

	SECTION("Run a parallel ForEach encapsulated in a ParallelJob (group 2)")
	{
		const int threadCount = 4;
		float average1 = 0, average2 = 0;

		// use an atomic value to store the result, preventing thread conflicts
		std::atomic<int> sum(0);
		std::atomic<int> count(0);
		ParallelJob<false, Profile, Transform> job1([&sum, &count](Entity* pEntity, Profile* pProfile, Transform* p1) {
			sum.fetch_add(pProfile->age);
			count++;
		});

		// call Prepare before executing the job
		job1.Prepare(pContext, threadCount);

		std::thread threads[threadCount];
		for (int i = 0; i < threadCount; i++) {
			threads[i] = std::thread([&]() {
				job1.Execute();
			});
		}
		for (int i = 0; i < threadCount; i++) {
			threads[i].join();
		}
		average1 = (float)(totalAge[0] + totalAge[1]) / (2 * n);
		average2 = (float)sum.load() / count.load();
		REQUIRE(average1 == average2);
	}

	SECTION("Run a parallel ForEachBatch encapsulated in a ParallelBatchJob (group 1)")
	{
		const int threadCount = 4;
		float average1 = 0, average2 = 0;

		// The first template parameter is whether thread local context is used.
		// The second template is the type of thread local context
		// The rests are component types
		ParallelBatchJob<true, ThreadLocalContext, Profile> job1([](ThreadLocalContext* pLocalContext, Entity* pEntity, int entityCount, Profile* pProfile) {
			
			for (int i = 0; i < entityCount; i++) {
				if (pEntity->IsValid()) {
					pLocalContext->sum += pProfile->age;
					pLocalContext->count += 1;
				}
				AdvancePointers(pEntity, pProfile);
			}
		});

		// call Prepare before executing the job
		job1.Prepare(pContext, threadCount);

		// divide the job and dispatch it to 4 threads.
		ThreadLocalContext localContexts[threadCount];
		std::thread threads[threadCount];
		auto p = &localContexts[0];
		for (int i = 0; i < threadCount; i++) {
			threads[i] = std::thread([&localContexts, &job1, i]() {
				auto p1 = &localContexts[i];
				job1.Execute(&localContexts[i]);
			});
		}

		int sum = 0;
		int count = 0;
		for (int i = 0; i < threadCount; i++) {
			threads[i].join();
			sum += localContexts[i].sum;
			count += localContexts[i].count;
		}
		average1 = (float)(totalAge[0] + totalAge[1] + totalAge[2]) / (3 * n);
		average2 = (float)sum / count;
		REQUIRE(average1 == average2);
	}

	SECTION("Run a parallel ForEachBatch encapsulated in a ParallelBatchJob (group 2)")
	{
		const int threadCount = 4;
		float average1 = 0, average2 = 0;

		// use an atomic value to store the result, preventing thread conflicts
		std::atomic<int> sum(0);
		std::atomic<int> count(0);
		ParallelBatchJob<false, Profile, Transform> job1([&sum, &count](Entity* pEntity, int entityCount, Profile* pProfile, Transform* pTransform) {
			for (int i = 0; i < entityCount; i++) {
				if (pEntity->IsValid()) {
					sum.fetch_add(pProfile->age);
					count++;
				}
				AdvancePointers(pEntity, pProfile, pTransform);
			}
		});

		// call Prepare before executing the job
		job1.Prepare(pContext, threadCount);

		std::thread threads[threadCount];
		for (int i = 0; i < threadCount; i++) {
			threads[i] = std::thread([&]() {
				job1.Execute();
			});
		}
		for (int i = 0; i < threadCount; i++) {
			threads[i].join();
		}
		average1 = (float)(totalAge[0] + totalAge[1]) / (2 * n);
		average2 = (float)sum.load() / count.load();
		REQUIRE(average1 == average2);
	}

	SECTION("Run a single thread ForEach encapsulated in a DeferredJob")
	{
		float average1 = 0, average2 = 0;

		// The first template parameter is whether thread local context is used.
		// The second template is the type of thread local context
		// The rests are component types
		DeferredJob<true, ThreadLocalContext, Profile> job1([](ThreadLocalContext* pLocalContext, Entity* pEntity, Profile* pProfile) {
			pLocalContext->sum += pProfile->age;
			pLocalContext->count += 1;
		});

		ThreadLocalContext arg;
		job1.Execute(pContext, &arg);

		average1 = (float)(totalAge[0] + totalAge[1] + totalAge[2]) / (3 * n);
		average2 = (float)arg.sum / (float)arg.count;
		REQUIRE(average1 == average2);


		// DeferredJob without an external argument
		int sum = 0;
		int count = 0;
		DeferredJob<false, Profile, Transform> job2([&sum, &count](Entity* pEntity, Profile* pProfile, Transform* pTransform) {
			sum += pProfile->age;
			count += 1;
		});

		job2.Execute(pContext);
		average1 = (float)(totalAge[0] + totalAge[1]) / (2 * n);
		average2 = (float)sum / (float)count;
		REQUIRE(average1 == average2);
	}

	SECTION("Run a single thread ForEachBatch encapsulated in a DeferredBatchJob")
	{
		float average1 = 0, average2 = 0;

		// The first template parameter is whether thread local context is used.
		// The second template is the type of thread local context
		// The rests are component types
		DeferredBatchJob<true, ThreadLocalContext, Profile> job1([](ThreadLocalContext* pLocalContext, Entity* pEntity, int entityCount, Profile* pProfile) {
			for (int i = 0; i < entityCount; i++) {
				if (pEntity->IsValid()) {
					pLocalContext->sum += pProfile->age;
					pLocalContext->count += 1;
				}
				AdvancePointers(pEntity, pProfile);
			}
			});
		ThreadLocalContext arg;
		job1.Execute(pContext, &arg);

		average1 = (float)(totalAge[0] + totalAge[1] + totalAge[2]) / (3 * n);
		average2 = (float)arg.sum / (float)arg.count;
		REQUIRE(average1 == average2);


		// DeferredJob without an external argument
		int sum = 0;
		int count = 0;
		DeferredBatchJob<false, Profile, Transform> job2([&sum, &count](Entity* pEntity, int entityCount, Profile* pProfile, Transform* pTransform) {
			for (int i = 0; i < entityCount; i++) {
				if (pEntity->IsValid()) {
					sum += pProfile->age;
					count += 1;
				}
				AdvancePointers(pEntity, pProfile);
			}
			});
		job2.Execute(pContext);
		average1 = (float)(totalAge[0] + totalAge[1]) / (2 * n);
		average2 = (float)sum / (float)count;
		REQUIRE(average1 == average2);
	}

	pContext->Release();
}

TEST_CASE("Delete an entity", "Release")
{
	World* pWorld = World::GetInstance();
	EntityContext* pContext = pWorld->CreateContext();
	Entity* pEntity = pContext->CreateEntity<ActorClass>();
	REQUIRE(pEntity->IsValid());
	EntityID id = pEntity->GetEntityID();
	Entity* pEntity2 = pContext->GetEntity(id);
	REQUIRE(pEntity2 == pEntity);
	// release entity
	pEntity->Release();
	REQUIRE(!pEntity->IsValid());
	Entity* pEntity3 = pContext->GetEntity(id);
	REQUIRE(pEntity3 == nullptr);
	pContext->Release();
}

TEST_CASE("Test Event Manager", "EventManager")
{
	World* pWorld = World::GetInstance();
	EntityContext* pContext = pWorld->CreateContext();
	REQUIRE(pContext->GetEventManager() == nullptr);
	Profile profile1("Hello", 25);

	SECTION("Test Create and Delete Entity event")
	{
		EventManager* pEventManager = pWorld->CreateEventManager();
		pContext->SetEventManager(pEventManager);
		int entityCounter = 0;
		auto hCreateEntityEventHandle = pEventManager->Subscrible<CreateEntityEvent>([&entityCounter](const CreateEntityEvent* evt) {
			Entity* pEntity = evt->pEntity;
			// initialize profile with invalid name and age, which reminds the user must give them explicit values
			if (pEntity->ContainComponent<Profile>())
			{
				Profile* pProfile = pEntity->GetComponent<Profile>();
				strcpy_s(pProfile->name, "invalid name");
				pProfile->age = -1;
			}
			entityCounter += 1;
		});

		auto hDeleteEntityEventHandle = pEventManager->Subscrible<DeleteEntityEvent>([&entityCounter](const DeleteEntityEvent* evt) {
			entityCounter -= 1;
		});

		Entity* pEntity1 = pContext->CreateEntity<ActorClass>();
		Entity* pEntity2 = pContext->CreateEntity(profile1);
		REQUIRE(entityCounter == 2);

		// create a new entity implicitly when adding a component to an entity.
		Entity* pEntity3 = pEntity2->Extend<Velocity>();
		REQUIRE(entityCounter == 3);

		// create a new entity implicitly when removing a component from an entity.
	 	Entity* pEntity4 = pEntity1->Remove<Velocity>();
		REQUIRE(entityCounter == 4);

		// check invalid names and ages
		pContext->ForEach<Profile>([](Entity* pEntity, Profile* pProfile) {
			REQUIRE(strcmp(pProfile->name, "invalid name") == 0);
			REQUIRE(pProfile->age == -1);
		});

		// release an entity
		pEntity1->Release();
		REQUIRE(entityCounter == 3);

		// unsubscribe the DeleteEntity event, from now on, the delete event will not be listened
		pEventManager->Unsubscribe(hDeleteEntityEventHandle);
		pEntity2->Release();
		REQUIRE(entityCounter == 3);

		// but create entity event is still being listened
		Entity* pEntity5 = pContext->CreateEntity<ActorClass>();
		REQUIRE(entityCounter == 4);

		// unsubscrible all the 'CreateEntityEvent' listeners
		pEventManager->Unsubscribe<CreateEntityEvent>();
		// create event is no longer listened
		Entity* pEntity6 = pContext->CreateEntity<Transform>();
		REQUIRE(entityCounter == 4);

		pContext->SetEventManager(nullptr);
		pEventManager->Release();

	}

	SECTION("Use-defined Event")
	{
		// define an event
		DefineEvent(MyDefinedEvent)
		{
			int value = 10;
		};

		int sum = 0;
		EventManager* pEventManager = pWorld->CreateEventManager();
		pContext->SetEventManager(pEventManager);
		pEventManager->Subscrible<MyDefinedEvent>([&sum](const MyDefinedEvent* evt) {
			sum += evt->value;
		});

		MyDefinedEvent evt;
		pEventManager->TriggerEvent<MyDefinedEvent>(evt);
		REQUIRE(sum == 10);
		pEventManager->TriggerEvent<MyDefinedEvent>(evt);
		REQUIRE(sum == 20);
		pEventManager->Unsubscribe<MyDefinedEvent>();
		pEventManager->TriggerEvent<MyDefinedEvent>(evt);
		REQUIRE(sum == 20);
		pContext->SetEventManager(nullptr);
		pEventManager->Release();
	}

	pContext->Release();
}

TEST_CASE("Migration of entities", "EntityContext")
{
	World* pWorld = World::GetInstance();
	EntityContext* pContext1 = pWorld->CreateContext();
	EntityContext* pContext2 = pWorld->CreateContext();
	SECTION("Migrate an entity from one context to another")
	{
		Profile profile("Name1", 30);
		Velocity velocity(Vector3f(1.2f, 1.0f, 3.0f), 12.0f);

		Entity* pEntity1 = pContext1->CreateEntity(profile, velocity);
		EntityID entityId1 = pEntity1->GetEntityID();
		// step1: create an entity with the same archtype in context2
		Entity* pEntity2 = pContext2->CreateEntity(pEntity1->GetArchetype());
		EntityID entityId2 = pEntity2->GetEntityID();
		// step2 : copy entity data
		pContext2->CopyEntityData(pEntity2, pEntity1);
		
		REQUIRE(pEntity1->GetContext() == pContext1);
		REQUIRE(pEntity2->GetContext() == pContext2);

		// step3 : delete entity1
		pEntity1->Release();


		REQUIRE(pContext1->GetEntity(entityId1) == nullptr);
		REQUIRE(pContext2->GetEntity(entityId2) == pEntity2);
		REQUIRE(pWorld->GetEntity(entityId1) == nullptr);
		REQUIRE(pWorld->GetEntity(entityId2) == pEntity2);
		REQUIRE(pEntity1 != pEntity2);
		REQUIRE(*pEntity2->GetComponent<Profile>() == profile);
		REQUIRE(*pEntity2->GetComponent<Velocity>() == velocity);
	}

	pContext1->Release();
	pContext2->Release();
}



int main(int argc, char* argv[]) {
	int result = Catch::Session().run(argc, argv);
	//system("pause");
	return result;
}