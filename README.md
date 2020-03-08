 TinyECS
=================
TinyECS is a lightweight C++ ECS (Entity-Component-System) Library, which can be used in game programming, game engine design or any other C++ software compiled with the standard of C++17.

## Download & Install
TinyECS is a header-only library. The easiest way to start is to download the project onto your local environment, and include the single header file, "TinyECS.hpp", in any project. Or you can use cmake to build the UnitTest demo given in the project. A C++ compiler supporting C++17 standard is a necessity, and the following compilers have already been tested:
* MSVC 15 & 16  ( Visual Studio 2017 & 2019 )
* GCC 8.2.0
* Clang 8.0.0

## Tutorial
Although TinyECS is tiny, it still supports the most essential and fundamental operations with good performance and high efficiency.

### World & EntityContext
These two concepts lay the foundations of the whole system. Any software system should only have one **World** object, but is permitted to create multiple **EntityContexts** at the same time. This gives developers a tool to differentiate entities by seperating them into different contexts. For example, your entire software may have several subsystems, in this situation, you can create one EntityContext for each subsystem; thus, entities in different subsystems are managed by their own contexts.

Remember that *World* must be a singleton, so you can create it through static method *World::GetInstance()*. Creating through *new operator* is also allowed, but it is the developer's duty to guarantee it's a singleton:

```C++
World* pWorld = World::GetInstance();
```
Or:
```C++
World* pWorld = new World(); // make sure you only call it once
```
To Create an EntityContext:
```C++
EntityContext* pContext = pWorld->CreateContext();
```

### Define Components
A componet in TinyECS is just a plain C++ struct, without any extra size being added. There are two ways to create a component, one is use *DefineComponent* macro:
```C++
DefineComponent(Profile)
{
	char	name[128] = { 0 };
	int		age = 0;
};

DefineComponent(Transform)
{
	Vector3		position;
	Vector3		scale;
	float		yaw = 0;
};

DefineComponent(Velocity)
{
	Vector3		Direction;
	float		Magnitude = 0.0f;
};
```
Another approach is to give each component  an explicit identifier. When using this, you have to set the macro `TINYECS_USE_CUSTOM_COMPONENT_TYPE_ID` to 1, which means you give a custumized id for each component:
``` C++
#define TINYECS_USE_CUSTOM_COMPONENT_TYPE_ID 1

DefineComponentWithID(Profile, 1) // give it an integer as component id
{
	char	name[128] = { 0 };
	int		age = 0;
};
```
### EntityArchetype
An **EntityArchetype** refers to an *entity type* that  contains several specific component types. Archetype describles the type of entity, but it has nothing to do with the creation or management of entities or components.
One approach to create (or get) an archetype is by giving a list of componet types as template parameters, the order of components given doesn't matter:
```C++
EntityArchetype* pArchetype1 = pWorld->CreateArchetype<Transform, Profile>();
EntityArchetype* pArchetype2 = pWorld->CreateArchetype<Profile, Transform>();
assert(pArchetype1 == pArchetype2); // the order doesn't matter

EntityArchetype* pArchetype3 = pWorld->CreateArchetype<Profile, Transform, Velocity>();
// they are different, because they have different component types.
assert(pArchetype1 != pArchetype3); 
```
Another way to create (or get) an archetype is by defining an **EntityClass** first.  To define an  EntityClass, just define a struct inherited from *EntityClass* with component types as its class template parameters:
```C++
// define an entity class
// an actor contains three components: Profile, Transform, Velocity.
class ActorClass 
	: public EntityClass<Profile, Transform, Velocity> {};
```
And then, call CreateArchetype with this *ActorClass*:
```C++
EntityArchetype* pArchetype1 = pWorld->CreateArchetype<ActorClass>();
EntityArchetype* pArchetype2 = pWorld->CreateArchetype<Transform, Profile, Velocity>();
assert(pArchetype1 == pArchetype2); // they are equivalent
```

### Create Entity

The only method to create an entity is **EntityContext::CreateEntity**; however, the parameters you can give it might vary in different situations.

The most straightfoward approach is just giving a list of component types as its template parameters:
```C++
Entity* pEntity1 = pContext->CreateEntity<Transform, Profile, Velocity>();
Entity* pEntity2 = pContext->CreateEntity<Transform>();
Entity* pEntity3 = pContext->CreateEntity<Transform, Profile>();
```
The component types in the template parameters indicate what components this entity contains, for example:
```C++
// pEntity1 contains 'Profile' component
assert(pEntity1->ContainComponent<Profile>() == true); 
// pEntity2 doesn't contain 'Profile' component
assert(pEntity2->ContainComponent<Profile>() == false);
```
You can also specify an **EntityArchetype** instead of several component types. Remember that an archetype is tantamount to a list of component types. This is where *EntityArchetype* comes into the picture:
```C++
EntityArchetype* pArchetype = pWorld->CreateArchetype<Transform, Profile>();
// create entity with an archetype
Entity* pEntity1 = pContext->CreateEntity(pArchetype);
// equivalent way:
Entity* pEntity2 = pContext->CreateEntity<Transform, Profile>();

// pEntity1 and pEntity2 belong to the same archetype
assert(pArchetype == pEntity1->GetArchetype());
assert(pArchetype == pEntity2->GetArchetype());
```
You can also give an *EntityClass* as the template parameter:
```C++
Entity* pEntity1 = pContext->CreateEntity<ActorClass>();
```
If no arguments are given when calling *CreateEntity*, the components will be created through default constructors; however, you can specify the component objects in the arguments:
```C++
Transform transform1;
Profile profile1;
// change the fields of transform1 and profile1 here
//....

// pass components as arguments, be careful about the order
Entity* pEntity = pContext->CreateEntity<Transform, Profile>(transform1, profile1);
```
You can also omit the template parameters above, and specify component objects directly. This may be **the easiest way** of all:
```C++
Entity* p = pContext->CreateEntity(transform1, profile1);
```

### Get Component
You can visit a component by calling GetComponent method on an entity:
```C++
Profile* pProfile = pEntity->GetComponent<Profile>();
printf("%s, %d\n", pProfile->name, pProfile->age);
```
If the entity doesn't contain the component you specify, it will return *null*.

### EntityID
Sometimes, you may want to store some entity references in a container or in a cache, or as class members. In these situations, I highly recommend you to store **EntityID** instead of *Entity**, since an entity pointer might become invalid if you delete the entity object somewhere else.
To get the EntityID of an entity object, just call GetEntityID method:
```C++
EntityID eid = pEntity->GetEntityID();
```
To get the actual entity pointer correlating to an EntityID, call **EntityContext::GetEntity** or **World::GetEntity**, make sure to check the validity of the pointer after getting it:
```C++
Entity* pEntity = pContext->GetEntity(eid);
// remember to check the pointer 
if (pEntity && pEntity->IsValid())
{
	// can use pEntity now ...
}
```
### Delete Entity
To delete an entity, just call its **Release** method:
```C++
pEntity->Release();
```

### Extend & Remove Operations

If you want to add components to an existing entity, then **Extend** method is an option. *Extend* method creates a new entity and retain the old one at the same. If you think the old entity is useless, just delete it:
```C++
// assume pEntity only contains a 'Transform' component
Entity* pEntity1 = pEntity->Extend<Profile>();
assert(pEntity1->ContainComponent<Profile>());

// you can add multiple components at the same time 
Entity* pEntity2 = pEntity->Extend<Profile, Velocity>();

// if you don't need the old entity any more, delete it.
pEntity->Release();
```

The same principle applies to component removal. When you intend to remove one or multiple component(s) from an entity, call *Remove* method and give a list of component types to be removed. 
```C++
// assume pEntity contains 'Transform', 'Profile', 'Velocity' three components
pEntity1 = pEntity->Remove<Transform>();
assert(pEntity1->ContainComponent<Transform>() == false);
// can remove multiple components at the same time
pEntity2 = pEntity->Remove<Transform, Profile>();
assert(pEntity2->ContainAnyComponents<Transform, Profile>() == false);
```

### ForEach
ForEach is an important method that iterates through all the entities that contain a specific list of component types.

One approach is calling ForEach with a list of component types as its template parameters, which indicate the components you care about, and a callback function which will be called for any valid and qualified entity.

The following code is calculating the average age of all the entities with a profile:
```C++
int sum = 0; // the sum of all entites' age
int count = 0; // entity count
pContext->ForEach<Profile>([&sum, &count](Entity* pEntity, Profile* pProfile) {
	sum += pProfile->age;
	count += 1;
});
float average_age = (float)sum / count;
```
Here, I give a *Profile* as its template parameter, meaning we only care about those entities at least with a 'Profile' component. The argument is a function given as a lambda expression, whose prototype must be paid attention to: the first parameter must be an Entity pointer, and the rest ones must correspond to the template parameters, here is Profile, of course.

Multiple comonent types are also allowed to appear in the template parameters:
```C++
pContext->ForEach<Profile, Transform>([](Entity* e, Profile* p1, Transform* t1) {
	// your code...
});
```
*World* class also has ForEach method, which iterates the qualified entities in the entire system rather than in a particular EntityContext.

### ForEachBatch
ForEachBatch means processing a bunch of entities at the same time in one callback function. For example:
```C++
pContext->ForEachBatch<Profile>([](Entity* pEntity, int count, Profile* p){
	for (int i = 0; i < count; i++) {
		if (pEntity->IsValid()) {
			// process here...
		}
		// advance entity and all components' pointers
		AdvancePointers(pEntity, p);
	}
});
```
There is an extra parameter in the callback function, count, which indicates how many entities are included in the current batch. While iterating the entities, make sure to check validity for each entity, and move forward current entity and comonents' pointers by calling AdvancePointers method during each iteration.

### ForEach on Multiple Threads
When the ForEach has to iterate a huge amount of entites, it might cost much time. One effective way to solve this is to take advantage of parallelism on a multicore computer and run the ForEach parallelly, which means dividing the *ForEach* into several threads.

To run a ForEach on multiple threads, you first need to declare a **ParallelJob**:
```C++
// this struct records some intermediate results during the execution
// each thread owns its own local context
struct ThreadLocalContext
{
	int		sum = 0;
	int		count = 0;
};

// declare a ParallelJob here
ParallelJob<true, ThreadLocalContext, Profile> job1([]
	(ThreadLocalContext* pLocalContext, Entity* pEntity, Profile* pProfile) 
	{
		pLocalContext->sum += pProfile->age;
		pLocalContext->count += 1;
	});
// must call Prepare before execution, specify EntityContext and thread count
// here I want to divide the job and dispatch it to 4 threads.
static const int threadCount = 4;
job1.Prepare(pEntityContext, threadCount );

// prepare thread-local memory for all the threads
ThreadLocalContext localContexts[threadCount];

// create threads and call `Execute` on each thread
std::thread threads[threadCount];
for (int i = 0; i < threadCount; i++) {
	threads[i] = std::thread([&localContexts, &job1, i]() {
		job1.Execute(&localContexts[i]);
	});
}

// Join all the threads, and calculate the final result
int sum = 0;
int count = 0;
for (int i = 0; i < threadCount; i++) {
	threads[i].join();
	sum += localContexts[i].sum;
	count += localContexts[i].count;
}
float average_age = (float)sum / count;
```
The code above looks a little complex. First, you create a job of *ParallelJob* type. The first class template parameter is a *bool* value, indicating whether you want to specify a local context for each thread. If this is false, the rest template parameters are the component types. If this is true, than the second parameter is the type of the thread-local context (in this example, it's *ThreadLocalContext*), and the rest are component types.

 Thread-local context is a user-defined structure. Each thread has its own thread-local context to store thread-local data without worrying about the thread-safe issues.

After creating a *ParallelJob*, now we can call **Prepare** before real execution. It tells the job how many threads will be allocated for this task.

Now, you can start to create threads, and call **Execute** on each thread. Remember to pass in the thread-local context object as its parameter.

In TinyECS, there is another type of job called **ParallelBatchJob** which allows to run a ForEachBatch task on multiple threads. Its usage is very similar to ParallelJob.

### Event
TinyECS supports event system that allows us to subscribe any event you are interested in and write code in a observer pattern.

To use event, you first need an event manager, represented by **EventManager** class and created by **World::CreateEventManager** method.
```C++
// create event manager
EventManager* pEventManager = pWorld->CreateEventManager();
// set the event manager to an EntityContext
pContext->SetEventManager(pEventManager);
```
To define an event type, use DefineEvent macro:
```C++
DefineEvent(MyDefinedEvent)
{
	int counter = 0;
};
```
To subscribe an event, call **Subscribe** method and specify the event type as its template parameter, and also pass a callback function in the argument. It returns an event handle, which can be used to unsubscribe the current callback function later.
```C++
auto h = pEventManager->Subscrible<MyDefinedEvent>([](const MyDefinedEvent* evt){
	evt->counter += 1;
});
```
To unsubscribe,  call **Unsubscribe**, and pass an event handle to unsubscribe a certain observer, or specify an event type to remove all observers related to that event type. 
```C++
// pass event handle to it
pEventManager->Unsubscribe(h);
// or pass an event type, to remove all observers listening to that type
pEventManager->Unsubscribe<MyDefinedEvent>();
```
To trigger an event, just call **Trigger** method, and pass an event object as the parameter:
```C++
MyDefinedEvent evt;
pEventManager->TriggerEvent<MyDefinedEvent>(evt);
```
There are two built-in events, which are triggered by the system automatically, you don't need to trigger manually:
* **CreateEntityEvent**:  triggered when an entity is created.
* **DeleteEntityEvent**:  triggered when an entity is destroyed.

### Customize Memory Allocator
By default, TinyECS employs C standard functions, *malloc* and *free* , to allocate and release memory for entities and components. If you want to design your own memory management strategy and rewrite allocation algorithms, please consider defining a new memory-allocate class that implements **IChunkMemoryAllocator** interface. To use your customized one, call *SetChunkMemoryAllocator* and pass your own allocator pointer:

```C++
class MyChunkMemoryAllocator : public IChunkMemoryAllocator
{
public:
	virtual void* Malloc(size_t sizeBytes) override {
		// allocate new memory space
	}
	virtual void* Realloc(void* ptr, std::size_t new_size) override {
		// re-allocate memory space
	}
	virtual void Free(void* p) override {
		// free memory space
	}
};
MyChunkMemoryAllocator *pAllocator = new MyChunkMemoryAllocator();
pWorld->SetChunkMemoryAllocator(pAllocator);
```
