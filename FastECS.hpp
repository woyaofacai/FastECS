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

#ifndef __DEFINED_FASTECS_HEADER_HPP__
#define __DEFINED_FASTECS_HEADER_HPP__

#include <typeinfo>
#include <map>
#include <unordered_map>
#include <assert.h>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <tuple>
#include <cstring>
#include <atomic>
#include <thread>
#include <cstdlib>

namespace FastECS
{

typedef unsigned char byte;


/**	Convert a char* type to char array
	e.g. "abc" => 'a', 'b', 'c', '\0'
	The length must be 32, extra bytes are filled with '\0'
*/
#define FASTECS_STR_1(S,I) (I < sizeof(S) ? S[I] : '\0')
#define FASTECS_STR_2(S,I) FASTECS_STR_1(S,I), FASTECS_STR_1(S,I+1)
#define FASTECS_STR_4(S,I) FASTECS_STR_2(S,I), FASTECS_STR_2(S,I+2)
#define FASTECS_STR_8(S,I) FASTECS_STR_4(S,I), FASTECS_STR_4(S,I+4)
#define FASTECS_STR_16(S,I) FASTECS_STR_8(S,I), FASTECS_STR_8(S,I+8)
#define FASTECS_STR_32(S,I) FASTECS_STR_16(S,I), FASTECS_STR_16(S,I+16)
#define FASTECS_STR(S) FASTECS_STR_32(S,0)

#ifndef FASTECS_SAFE_DELETE
#define FASTECS_SAFE_DELETE(x) { if(x){ delete x; x = 0; } }
#endif

#if FASTECS_ASSERT_ENABLE
#define FASTECS_ASSERT(expression) assert(expression)
#else
#define FASTECS_ASSERT(expression)
#endif

/// The maximum thread can be designated when using ParallelJob
#ifdef FASTECS_MAX_THREAD_COUNT
enum { MAX_THREAD_COUNT = FASTECS_MAX_THREAD_COUNT };
#else
enum { MAX_THREAD_COUNT = 32 };
#endif

/// The maximum component count for each entity
#ifdef FASTECS_MAX_COMPONENT_COUNT_PER_ENTITY
#define MAX_COMPONENT_COUNT_PER_ENTITY FASTECS_MAX_COMPONENT_COUNT_PER_ENTITY
enum { MAX_COMPONENT_COUNT_PER_ENTITY = FASTECS_MAX_COMPONENT_COUNT_PER_ENTITY };
#else
enum { MAX_COMPONENT_COUNT_PER_ENTITY = 32 };
#endif

enum { INVALID_COMPONENT_INDEX = -1 };
enum { INVALID_EVENT_INDEX = -1 };

/// The maximum component count user can define in the whole system
#ifdef FASTECS_MAX_COMPONENT_COUNT
enum { MAX_COMPONENT_COUNT = FASTECS_MAX_COMPONENT_COUNT };
#else
enum { MAX_COMPONENT_COUNT = 128 };
#endif

/// The maximum event count user can define in the whole system
#ifdef FASTECS_MAX_EVENT_COUNT
enum { MAX_EVENT_COUNT = FASTECS_MAX_EVENT_COUNT + 2 };
#else 
enum {MAX_EVENT_COUNT = 16 };
#endif

/// maximum chunk size
/// actual chunk size = min(MAX_ENTITY_COUNT_PER_CHUNK * entitySize, maxChunkSize)
#ifdef FASTECS_MAX_STORAGE_CHUNK_SIZE
enum { MAX_STORAGE_CHUNK_SIZE = FASTECS_MAX_STORAGE_CHUNK_SIZE };
#else
enum { MAX_STORAGE_CHUNK_SIZE = 64 * 1024 * 1024 }; // 64k
#endif

/// maximum EntityContext count in one ECS world
#ifdef FASTECS_MAX_CONTEXT_COUNT
enum { MAX_CONTEXT_COUNT = FASTECS_MAX_CONTEXT_COUNT };
#else
enum { MAX_CONTEXT_COUNT = 256 };
#endif

/// use this macro to control MAX_ENTITY_COUNT_PER_CHUNK
/// MAX_ENTITY_COUNT_PER_CHUNK == (1 << MAX_BLOCK_COUNT_BITS)
#ifdef FASTECS_MAX_BLOCK_COUNT_BITS
enum { MAX_BLOCK_COUNT_BITS = FASTECS_MAX_BLOCK_COUNT_BITS };
#else
enum { MAX_BLOCK_COUNT_BITS = 10 };
#endif

/// use this to set MAX_CHUNK_COUNT_PER_STORAGE
enum { MAX_CHUNK_COUNT_BITS = 15 };

/// use this to control MAX_STORAGE_COUNT_PER_CONTEXT
/// which means the maximum archetype you can define in each context
enum { MAX_STORAGE_COUNT_BITS = 10 };

enum { MAX_ENTITY_COUNT_PER_CHUNK = (1 << MAX_BLOCK_COUNT_BITS) };
enum { MAX_CHUNK_COUNT_PER_STORAGE = (1 << MAX_CHUNK_COUNT_BITS) };
enum { MAX_STORAGE_COUNT_PER_CONTEXT = (1 << MAX_STORAGE_COUNT_BITS) };

enum { BLOCK_INDEX_MASK = MAX_ENTITY_COUNT_PER_CHUNK - 1 };
enum { CHUNK_INDEX_MASK = MAX_CHUNK_COUNT_PER_STORAGE - 1};
enum { STORAGE_INDEX_MASK = MAX_STORAGE_COUNT_PER_CONTEXT - 1};

/// How to generate id for each component
/// 0: generate id automatically, use DefineComponent to define component
/// 1: generate id by specifying a value manually, use DefineComponentWithID to define component
#ifdef FASTECS_USE_CUSTOM_COMPONENT_TYPE_ID
#define USE_CUSTOM_COMPONENT_TYPE_ID FASTECS_USE_CUSTOM_COMPONENT_TYPE_ID
#else
#define USE_CUSTOM_COMPONENT_TYPE_ID 0
#endif

/// the meaning is like above
#ifdef FASTECS_USE_CUSTOM_EVENT_TYPE_ID
#define USE_CUSTOM_EVENT_TYPE_ID FASTECS_USE_CUSTOM_EVENT_TYPE_ID
#else
#define USE_CUSTOM_EVENT_TYPE_ID 0
#endif

// Component Index Table Type, the values you can specify:
// 0: use sequential array, the search speed is log(n), n is the number of components that the current entity has
// 1: use hashmap as a container, with the same search speed as std::unordered_map
// 2: use direct array, the search speed is log(1), but remember to give the component and unique id using DefineComponentWithID
//	  instead of DefineComponent when you define your own component, and make sure this id doesn't excceed
//    FASTECS_MAX_COMPONENT_COUNT whose default value is 128.
#ifdef FASTECS_COMPONENT_INDEX_TABLE_TYPE
#define COMPONENT_INDEX_TABLE_TYPE FASTECS_COMPONENT_INDEX_TABLE_TYPE
#else
#define COMPONENT_INDEX_TABLE_TYPE 0
#endif

/// Event Index Table Type, the values are defines the same as above
#ifdef FASTECS_EVENT_INDEX_TABLE_TYPE
#define EVENT_INDEX_TABLE_TYPE FASTECS_EVENT_INDEX_TABLE_TYPE
#else
#define EVENT_INDEX_TABLE_TYPE 0
#endif

/// How to split a paralell job before you execute it, it's used in Prepare method
/// 0: divide job by chunks, without dividing blocks inside each chunk
/// 1: divide job by blocks inside each chunk
#ifdef FASTECS_DIVIDE_PARALLEL_JOB_METHOD
#define DIVIDE_PARALLEL_JOB_METHOD FASTECS_DIVIDE_PARALLEL_JOB_METHOD
#else
#define DIVIDE_PARALLEL_JOB_METHOD 1
#endif

/// crc tables for CRC algorithms
constexpr uint32_t crc_table[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x76dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0xedb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x9b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x1db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x6b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0xf00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x86d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x3b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x4db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0xd6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0xa00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x26d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x5005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0xcb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0xbdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/// return the length of a given string
template<size_t _n> 
constexpr size_t string_length(const char(&s)[_n]) noexcept
{
	size_t n = _n;
	while (s[n - 1] == 0 && n > 0)
	{
		--n;
	}
	return n;
}

/// calculate CRC value for a given string
template<size_t _len, size_t _n>
constexpr uint32_t string_crc(const char(&s)[_n])
{
	uint32_t x = 0xffffffff;
	for (size_t i = 0; i < _len; i++) {
		x = (x >> 8) ^ crc_table[(x ^ s[i]) & 0x000000ff];
	}
	return x ^ 0xffffffff;
}

/// sum up several values of type T
/// mainly used to calculate the sum of multiple CRC values
template<typename T, T...>
struct sum_helper_class;

template<typename T, T x, T...Args>
struct sum_helper_class<T, x, Args...> {
	constexpr static const T value = x + sum_helper_class<T, Args...>::value;
};
template<typename T>
struct sum_helper_class<T> {
	constexpr static const T value = 0;
};

template<typename T, T... Args>
constexpr T sum_v = sum_helper_class<T, Args...>::value;

using ComponentHash = uint32_t;
using ComponentTypeID = ComponentHash;
using EventHash = uint32_t;
using EventTypeID = EventHash;

#define INVALID_COMPONENT_TYPE_ID 0xffffffff

/// All component class must inheret from this class
struct ComponentBase { };

/// Any component class or event class must inhere from this
/// this class gives each component class an id which is unique in the entire system
/// if USE_CUSTOM_COMPONENT_TYPE_ID is set to 0: generate type_id automatically
/// if USE_CUSTOM_COMPONENT_TYPE_ID is set to 1: use CustomId as the type_id
template<ComponentTypeID CustomId, char..._Char>
struct constant_name_class
{
public:
	static constexpr char const __className_string[sizeof...(_Char) + 1] = { _Char..., '\0' };
	static constexpr size_t __className_len = string_length(__className_string);
	static constexpr uint32_t __crc = string_crc<__className_len>(__className_string);
	//static constexpr uint32_t xxx = __className_len - 1;

	static constexpr const char* class_name() { return &__className_string[0]; }
	//static constexpr size_t classNameLength() { return __className_len; }
	static constexpr ComponentHash hash_code() { return __crc; }
};

/// NOTE: the difference between hashcode and id
/// they are both unique identifiers in the system, but hashcode is computed through CRC algorithm
/// id's calculation depends on USE_CUSTOM_COMPONENT_TYPE_ID
/// they are the same if USE_CUSTOM_COMPONENT_TYPE_ID is 0
template<ComponentTypeID CustomId, char..._Char>
struct component_name_class : public constant_name_class<CustomId, _Char...> , public ComponentBase
{
#if USE_CUSTOM_COMPONENT_TYPE_ID
	static constexpr ComponentTypeID type_id()
	{
		static_assert(CustomId != INVALID_COMPONENT_TYPE_ID, "Must assign a value when using custom component type id.");
		static_assert(CustomId < MAX_COMPONENT_COUNT, "CustomId must smaller than the maximum of components");
		return CustomId;
	}
#else
	static constexpr ComponentTypeID type_id() { return constant_name_class<CustomId, _Char...>::__crc; }
#endif
};

template<EventTypeID CustomId, char..._Char>
struct event_name_class : public constant_name_class<CustomId, _Char...> 
{
#if USE_CUSTOM_EVENT_TYPE_ID
	static constexpr EventTypeID type_id()
	{
		static_assert(CustomId != INVALID_COMPONENT_TYPE_ID, "Must assign a value when using custom event type id.");
		static_assert(CustomId < MAX_EVENT_COUNT, "CustomId must smaller than the maximum of events");
		return CustomId;
	}
#else
	static constexpr EventTypeID type_id() { return constant_name_class<CustomId, _Char...>::__crc; }
#endif
};

#define ComponentBaseClass(s, customId) FastECS::component_name_class<customId, FASTECS_STR(#s)>
#define DefineComponent(className) \
	struct className : public FastECS::component_name_class<INVALID_COMPONENT_TYPE_ID, FASTECS_STR(#className)>
#define DefineComponentWithID(className, id) \
	struct className : public FastECS::component_name_class<id, FASTECS_STR(#className)>

#define EventBaseClass(s, customId) FastECS::event_name_class<customId, FASTECS_STR(#s)>
#define DefineEvent(className) \
	struct className : public FastECS::event_name_class<INVALID_COMPONENT_TYPE_ID, FASTECS_STR(#className)>
#define DefineEventWithID(className, id) \
	struct className : public FastECS::event_name_class<id, FASTECS_STR(#className)>

/// helper class to check if these TYPEs have any duplication
template<typename T, typename...OtherTypes>
struct is_type_duplicate;

template<typename T, typename U, typename...OtherTypes>
struct is_type_duplicate<T, U, OtherTypes...>
{
	constexpr static const bool value = std::is_same<std::decay_t<T>, std::decay<U>>::value ? true : is_type_duplicate<T, OtherTypes...>::value;
};

template<typename T>
struct is_type_duplicate<T>
{
	constexpr static const bool value = false;
};

template<typename T, typename...OtherTypes>
constexpr bool is_type_duplicate_v = is_type_duplicate<T, OtherTypes...>::value;


/// helper class to sum up multiple hash codes
template<typename...T>
struct sum_component_hashcodes;

#pragma warning( push )
#pragma warning( disable : 4307 ) // warning C4307: '+': integral constant overflow

template<typename T, typename...U>
struct sum_component_hashcodes<T, U...> {
	constexpr static const uint32_t value = (is_type_duplicate_v<T, U...> ? 0 : T::hash_code()) + (uint32_t)sum_component_hashcodes<U...>::value;
};

template<>
struct sum_component_hashcodes<> {
	constexpr static const uint32_t value = 0;
};
#pragma warning( pop )

template<typename...T>
constexpr uint32_t sum_component_hashcodes_v = sum_component_hashcodes<T...>::value;

/// move forward each pointer in the parameter list by 1
template<typename T0>
inline void AdvancePointers(T0*& p0) { p0++; }
template<typename T0, typename T1>
inline void AdvancePointers(T0*& p0, T1*& p1) { p0++; p1++; }
template<typename T0, typename T1, typename T2>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2) { p0++; p1++; p2++; }
template<typename T0, typename T1, typename T2, typename T3>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2, T3*& p3) { p0++; p1++; p2++; p3++; }
template<typename T0, typename T1, typename T2, typename T3, typename T4>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2, T3*& p3, T4*& p4) { p0++; p1++; p2++; p3++; p4++; }
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2, T3*& p3, T4*& p4, T5*& p5) { p0++; p1++; p2++; p3++; p4++; p5++; }
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2, T3*& p3, T4*& p4, T5*& p5, T6*& p6) { p0++; p1++; p2++; p3++; p4++; p5++; p6++; }
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2, T3*& p3, T4*& p4, T5*& p5, T6*& p6, T7*& p7) { p0++; p1++; p2++; p3++; p4++; p5++; p6++; p7++; }
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2, T3*& p3, T4*& p4, T5*& p5, T6*& p6, T7*& p7, T8*& p8) { p0++; p1++; p2++; p3++; p4++; p5++; p6++; p7++; p8++; }
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2, T3*& p3, T4*& p4, T5*& p5, T6*& p6, T7*& p7, T8*& p8, T9*& p9) { p0++; p1++; p2++; p3++; p4++; p5++; p6++; p7++; p8++; p9++; }
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline void AdvancePointers(T0*& p0, T1*& p1, T2*& p2, T3*& p3, T4*& p4, T5*& p5, T6*& p6, T7*& p7, T8*& p8, T9*& p9, T10*& p10) { p0++; p1++; p2++; p3++; p4++; p5++; p6++; p7++; p8++; p9++; p10++; }

#define CAST_COMPONENTS_1(pBytes, T0, p0) \
	 T0* p0 = (T0*)((pBytes)[0]); 
#define CAST_COMPONENTS_2(pBytes, T0, T1, p0, p1) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; 
#define CAST_COMPONENTS_3(pBytes, T0, T1, T2, p0, p1, p2) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; T2* p2 = (T2*)pBytes[2]; 
#define CAST_COMPONENTS_4(pBytes, T0, T1, T2, T3, p0, p1, p2, p3) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; T2* p2 = (T2*)pBytes[2]; T3* p3 = (T3*)pBytes[3];
#define CAST_COMPONENTS_5(pBytes, T0, T1, T2, T3, T4, p0, p1, p2, p3, p4) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; T2* p2 = (T2*)pBytes[2]; T3* p3 = (T3*)pBytes[3]; T4* p4 = (T4*)pBytes[4];
#define CAST_COMPONENTS_6(pBytes, T0, T1, T2, T3, T4, T5, p0, p1, p2, p3, p4, p5) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; T2* p2 = (T2*)pBytes[2]; T3* p3 = (T3*)pBytes[3]; T4* p4 = (T4*)pBytes[4]; \
	T5* p5 = (T5*)pBytes[5];
#define CAST_COMPONENTS_7(pBytes, T0, T1, T2, T3, T4, T5, T6, p0, p1, p2, p3, p4, p5, p6) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; T2* p2 = (T2*)pBytes[2]; T3* p3 = (T3*)pBytes[3]; T4* p4 = (T4*)pBytes[4]; \
	T5* p5 = (T5*)pBytes[5]; T6* p6 = (T6*)pBytes[6];
#define CAST_COMPONENTS_8(pBytes, T0, T1, T2, T3, T4, T5, T6, T7, p0, p1, p2, p3, p4, p5, p6, p7) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; T2* p2 = (T2*)pBytes[2]; T3* p3 = (T3*)pBytes[3]; T4* p4 = (T4*)pBytes[4]; \
	T5* p5 = (T5*)pBytes[5]; T6* p6 = (T6*)pBytes[6]; T7* p7 = (T7*)pBytes[7];
#define CAST_COMPONENTS_9(pBytes, T0, T1, T2, T3, T4, T5, T6, T7, T8, p0, p1, p2, p3, p4, p5, p6, p7, p8) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; T2* p2 = (T2*)pBytes[2]; T3* p3 = (T3*)pBytes[3]; T4* p4 = (T4*)pBytes[4]; \
	T5* p5 = (T5*)pBytes[5]; T6* p6 = (T6*)pBytes[6]; T7* p7 = (T7*)pBytes[7]; T8* p8 = (T8*)pBytes[8]; 
#define CAST_COMPONENTS_10(pBytes, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) \
	T0* p0 = (T0*)pBytes[0]; T1* p1 = (T1*)pBytes[1]; T2* p2 = (T2*)pBytes[2]; T3* p3 = (T3*)pBytes[3]; T4* p4 = (T4*)pBytes[4]; \
	T5* p5 = (T5*)pBytes[5]; T6* p6 = (T6*)pBytes[6]; T7* p7 = (T7*)pBytes[7]; T8* p8 = (T8*)pBytes[8]; T9* p9 = (T9*)pBytes[9]; 


#define FOR_EACH_ENTITY(f, count, pEntity, ...)			\
	for (int __i = 0; __i < count; __i++) {				\
		if (pEntity->mValid)							\
			f(pEntity, __VA_ARGS__);					\
		FastECS::AdvancePointers(pEntity, __VA_ARGS__);	\
	}

#define FOR_EACH_ENTITY1(pArg, f, count, pEntity, ...)	\
	for (int __i = 0; __i < count; __i++) {				\
		if (pEntity->mValid)							\
			f(pArg, pEntity, __VA_ARGS__);				\
		FastECS::AdvancePointers(pEntity, __VA_ARGS__);	\
	}

/// if you want to design your own memory-allocate algorithm,
/// consider defining a class that implements this interface
class IChunkMemoryAllocator
{
public:
	virtual void* Malloc(size_t sizeBytes) = 0;
	virtual void* Realloc(void* ptr, std::size_t new_size) = 0;
	virtual void Free(void* p) = 0;
};

/// default memory allocator, if you don't define your own
class StandardChunkMemoryAllocator : public IChunkMemoryAllocator
{
public:
	virtual void* Malloc(size_t sizeBytes) override
	{
		void* p = std::malloc(sizeBytes);
		return p;
	}
	virtual void* Realloc(void* ptr, std::size_t new_size) override
	{
		void* new_ptr = std::realloc(ptr, new_size);
		return new_ptr;
	}
	virtual void Free(void* p) override
	{
		std::free(p);
	}
};

/// get next address that is aligned according to 'alignment' parameter
inline void* get_next_aligned_address(const void* ptr, size_t alignment)
{
	return (void*)(((uintptr_t)ptr + (uintptr_t)(alignment - 1)) / (uintptr_t)alignment * (uintptr_t)alignment);
}

/// check if pointer's address is aligned according to the 'alignment' parameter
inline bool check_aligned_address(const void* ptr, size_t alignment)
{
	return (uintptr_t)ptr % (uintptr_t)alignment == 0;
}

/// check if pointer's address is aligned with respect to the type 'T'
template<typename T>
inline bool check_aligned_address(const T* ptr)
{
	return (uintptr_t)ptr % (uintptr_t)std::alignment_of_v<T> == 0;
}

struct DummyEntityClass {};

struct EntityClassBase {};

/// define an entity class by inheriting this one
/// for example: 
///		struct S : public EntityClass<A, B, C> {}
/// A, B, C are all component classes. S is an entity class that includes A, B, C
/// By this, you can create archetype in a different way
/// pWorld->CreateArchetype<S>() equals to pWorld->CreateArchetype<A,B,C>();
template<typename...ComponentTypes>
struct EntityClass;

template<typename T, typename...U>
struct EntityClass<T, U...> : public EntityClassBase
{
	constexpr static const uint32_t hashCode = sum_component_hashcodes_v<T, U...>;
	static constexpr uint32_t hash_code() { return hashCode; }
	using type = T;
	using next_type = EntityClass<U...>;
};

template<>
struct EntityClass<> : public EntityClassBase
{
	constexpr static const uint32_t hashCode = 0;
	using type = DummyEntityClass;
	//using next_type = EntityClass<U...>;
};

class World;

using ArchetypeID = uint32_t;

using ComponentConstructor = std::function<void(void*)>;
using ComponentDestructor = std::function<void(void*)>;
using ComponentAssignment = std::function<void(void*, const void*)>;

/// meta data that describles a component class
struct ComponentMeta
{
	const char*				name;
	ComponentHash			hashCode;
	ComponentTypeID			typeId;
	size_t					size;			/// the component size, which means sizeof(C)
	size_t					alignment;		/// the component's alignment, calculated by std::alignment
	ComponentConstructor	constructor = nullptr; /// constructor of component class, which means C();
	ComponentDestructor		destructor = nullptr; /// destructor of component class, which means ~C();
	ComponentAssignment		assignment = nullptr; /// assignment operator, which means operator==(); 
};

using ComponentMetaMap = std::map<ComponentTypeID, ComponentMeta*>;


/// Following is three component index tables, 
/// whose function is to return an index of this table after you give a componentID

/// LinearComponentIndexTable: use sequential array, the search speed is log(n), n is the number of components that the current entity has
template<int MaxCount> 
class LinearComponentIndexTable
{
public:
	int Add(ComponentTypeID id) 
	{
		auto count = mComponentCount;
		mComponentTypeIds[mComponentCount++] = id; 
		return count;
	}
	template<typename T> int Add() { return Add(std::decay_t<T>::type_id()); }
	int Get(ComponentTypeID id) const
	{
		for (int i = 0; i < mComponentCount; i++) {
			if (mComponentTypeIds[i] == id)
				return i;
		}
		return INVALID_COMPONENT_INDEX;
	}
	template<typename T>
	int Get() const { return Get(std::decay_t<T>::type_id()); }
private:
	int					mComponentCount = 0;
	ComponentTypeID		mComponentTypeIds[MaxCount] = { 0 };
};

//// HashMapComponentIndexTable: use hashmap as a container, with the same search speed as std::unordered_map
class HashMapComponentIndexTable
{
public:
	int Add(ComponentTypeID id)
	{
		FASTECS_ASSERT(mIndexMap.size() == mComponentCount);
		FASTECS_ASSERT(mIndexMap.find(id) == mIndexMap.end());
		auto count = mComponentCount;
		mIndexMap.insert({ id, mComponentCount });
		mComponentCount += 1;
		return count;
	}
	template<typename T> int Add() { return Add(std::decay_t<T>::type_id()); }
	int Get(ComponentTypeID id) const
	{
		auto it = mIndexMap.find(id);
		if (it == mIndexMap.end())
			return INVALID_COMPONENT_INDEX;
		return it->second;
	}
	template<typename T> int Get() const { return Get(std::decay_t<T>::type_id()); }
private:
	int					mComponentCount = 0;
	std::unordered_map<ComponentTypeID, int>	mIndexMap;
};

// DirectComponentIndexTable: use direct array, the search speed is log(1), but remember to give the component and unique id using DefineComponentWithID
//	  instead of DefineComponent when you define your own component, and make sure this id doesn't excceed
//    FASTECS_MAX_COMPONENT_COUNT whose default value is 128.
template<int MaxCount>
class DirectComponentIndexTable
{
public:
	DirectComponentIndexTable() 
	{
		for (int i = 0; i < MaxCount; i++) {
			mIndexTable[i] = INVALID_COMPONENT_INDEX;
		}
	}
	int Add(ComponentTypeID id)
	{
		FASTECS_ASSERT(mIndexTable[id] == INVALID_COMPONENT_INDEX);
		mIndexTable[id] = mComponentCount;
		auto count = mComponentCount;
		mComponentCount += 1;
		return count;
	}
	template<typename T> int Add() { return Add(std::decay_t<T>::type_id()); }
	int Get(ComponentTypeID id) const { return mIndexTable[id]; }
	template<typename T> int Get() const { return Get(std::decay_t<T>::type_id()); }
private:
	int			mComponentCount = 0;
	int			mIndexTable[MaxCount];
};

#if COMPONENT_INDEX_TABLE_TYPE == 0
	using ComponentIndexTable = LinearComponentIndexTable<MAX_COMPONENT_COUNT_PER_ENTITY>;
#elif COMPONENT_INDEX_TABLE_TYPE == 1
	using ComponentIndexTable = HashMapComponentIndexTable;
#elif COMPONENT_INDEX_TABLE_TYPE == 2
#if USE_CUSTOM_COMPONENT_TYPE_ID
	using ComponentIndexTable = DirectComponentIndexTable<MAX_COMPONENT_COUNT>;
#else
	using ComponentIndexTable = LinearComponentIndexTable<MAX_COMPONENT_COUNT_PER_ENTITY>;
#endif
#endif

/// helper class: determines if any of these ComponentTypes... is in a component index table (componentIndexTable)
template<typename...ComponentTypes>
struct ContainComponentsHelperClass;

template<typename ComponentType, typename...Rest>
struct ContainComponentsHelperClass<ComponentType, Rest...>
{
	static bool All(const ComponentIndexTable& componentIndexTable)
	{
		return componentIndexTable.Get<ComponentType>() != INVALID_COMPONENT_INDEX &&
			ContainComponentsHelperClass<Rest...>::All(componentIndexTable);
	}
	static bool Any(const ComponentIndexTable& componentIndexTable)
	{
		return componentIndexTable.Get<ComponentType>() != INVALID_COMPONENT_INDEX ||
			ContainComponentsHelperClass<Rest...>::Any(componentIndexTable);
	}
};

template<>
struct ContainComponentsHelperClass<>
{
	static bool All(const ComponentIndexTable& componentIndexTable) { return true; }
	static bool Any(const ComponentIndexTable& componentIndexTable) { return false; }
};

class EntityArchetypeManager;
class EntityComponentStorage;
class EntityContext;
class EntityComponentChunk;

/// EntityArchetype:
/// defines an entity class that contains a specific list of components
/// Entities that contain the same component classes belong to the same EntityArchetype
class EntityArchetype
{
	friend class EntityArchetypeManager;
	friend class EntityComponentStorage;
	friend class EntityComponentChunk;
	friend class EntityContext;
private:

public:
	EntityArchetype(EntityArchetypeManager* pArchetypeManager, ArchetypeID id, const ComponentMetaMap& metaMap)
		:mArchetypeManager(pArchetypeManager), mArchetypeId(id), mComponentMetaMap(metaMap)
	{
		mComponentCount = (int)metaMap.size();
		int i = 0;
		size_t currentOffset = 0;
		for (auto it : metaMap) 
		{
			ComponentMeta* meta = it.second;
			mComponentNames[i] = meta->name;
			mComponentHashes[i] = meta->hashCode;
			mComponentTypeIds[i] = meta->typeId;
			mComponentSizes[i] = meta->size;
			mComponentAlignments[i] = meta->alignment;
			mComponentOffsets[i] = currentOffset;

			mComponentConstructors[i] = &meta->constructor;
			mComponentDestructors[i] = &meta->destructor;
			mComponentAssignments[i] = &meta->assignment;

			mComponentIndexTable.Add(meta->typeId);
			currentOffset += meta->size;
			i += 1;
		}
	}

	EntityArchetype(const EntityArchetype&) = delete;
	EntityArchetype& operator=(const EntityArchetype&) = delete;

	template<typename ComponentType>
	bool ContainComponent() const
	{
		return  mComponentIndexTable.Get<ComponentType>() != INVALID_COMPONENT_INDEX;
	}

	template<typename... ComponentTypes>
	bool ContainAllComponents() const
	{
		return ContainComponentsHelperClass<ComponentTypes...>::All(mComponentIndexTable);
	}

	template<typename... ComponentTypes>
	bool ContainAnyComponents() const
	{
		return ContainComponentsHelperClass<ComponentTypes...>::Any(mComponentIndexTable);
	}

	/// Get an index that indicates the component's position in this archetype
	template<typename ComponentType>
	int GetComponentIndex() const
	{
		return mComponentIndexTable.Get<ComponentType>();
	}

	/// Get an index that indicates the component's position in this archetype
	int GetComponentIndex(ComponentTypeID componentTypeID) const
	{
		return mComponentIndexTable.Get(componentTypeID);
	}

	ArchetypeID GetID() const {
		return mArchetypeId;
	}

	/// Extend an existing archetype with a list of component types
	/// to create a new archtype
	template<typename...ComponentTypes>
	inline EntityArchetype* Extend();

private:
	EntityArchetypeManager*		mArchetypeManager = nullptr;
	ArchetypeID			mArchetypeId = 0;
	ComponentMetaMap	mComponentMetaMap;
	
	int					mComponentCount = 0;
	const char*			mComponentNames[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	ComponentTypeID		mComponentHashes[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	ComponentHash		mComponentTypeIds[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	size_t				mComponentSizes[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	size_t				mComponentAlignments[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	size_t				mComponentOffsets[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	ComponentConstructor*	mComponentConstructors[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	ComponentDestructor*	mComponentDestructors[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	ComponentAssignment*	mComponentAssignments[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };

	ComponentIndexTable	mComponentIndexTable;

	// map from context to storage, just for fast indexing.
	EntityComponentStorage*		mStoragesInContext[MAX_CONTEXT_COUNT] = { 0 };
};

/// EntityArchetypeManager:
/// Is a container that manages all the archetype types
/// Include Create and Get Archetype
class EntityArchetypeManager
{
public:
	EntityArchetypeManager()
	{

	}

	/// create an archetype, ...T can be:
	/// a list of component types.
	/// an entity class
	template<typename...T>
	EntityArchetype* CreateArchetype();

	/// create an archetype with an entity class (defined by inheriting EntityClass)
	template<typename EntityClassType>
	EntityArchetype* CreateArchetypeByEntityClass()
	{
		const ArchetypeID id = EntityClassType::hashCode;
		auto it = mArchetypesMap.find(id);
		if (it != mArchetypesMap.end()) {
			return it->second;
		}
		ComponentMetaMap metaMap;
		GetComponentsMetaFromEntityClass<EntityClassType>(metaMap);
		EntityArchetype* pEntityArchetype = new EntityArchetype(this, id, metaMap);
		mArchetypesMap.insert({ id, pEntityArchetype });
		return pEntityArchetype;
	}
	/// create an archetype by a list of component types
	template<typename...ComponentTypes>
	EntityArchetype* CreateArchetypeByComponentTypes();

	/// Add component types to an existing archetype to create a new archetype
	template<typename...ComponentTypes>
	EntityArchetype* AddComponents(const EntityArchetype* pArchetype);

	/// Remove component types from an existing archetype.
	/// return a new archetype
	template<typename... ComponentTypes>
	EntityArchetype* RemoveComponents(const EntityArchetype* pArchetype);

	ArchetypeID GetArchetypeIDFromComponentMetaMap(const ComponentMetaMap& metaMap)
	{
		ArchetypeID id = 0;
		for (const auto& it : metaMap) {
			id += it.second->hashCode;
		}
		return id;
	}
	
	/// Get a map of component meta data from entity class
	template<typename EntityClassType>
	void GetComponentsMetaFromEntityClass(ComponentMetaMap& metaMap)
	{
		if constexpr (std::is_same_v<typename EntityClassType::type, DummyEntityClass>)
		{
			return;
		}
		else
		{
			using ComponentType = typename EntityClassType::type;
			ComponentTypeID componentTypeId = ComponentType::type_id();
			if (metaMap.find(componentTypeId) == metaMap.end()) 
			{
				ComponentMeta* meta = GetComponentMeta<ComponentType>();
				metaMap.insert({ componentTypeId, meta });
			}
			GetComponentsMetaFromEntityClass<typename EntityClassType::next_type>(metaMap);
		}
	}
	
	/// Get meta data of type T
	/// if T's meta has already been calculated, just get it from cache
	/// if not, create one
	template<typename T>
	ComponentMeta* GetComponentMeta()
	{
		using ComponentType = std::decay_t<T>;
		ComponentHash hashcode = ComponentType::hash_code();
		auto it = mComponentMetas.find(hashcode);
		if (it != mComponentMetas.end())
			return it->second;

		ComponentMeta* meta = new ComponentMeta();
		meta->name = ComponentType::class_name();
		meta->hashCode = ComponentType::hash_code();
		meta->typeId = ComponentType::type_id();;
		meta->size = sizeof(ComponentType);
		meta->alignment = std::alignment_of<ComponentType>::value;
		meta->constructor = [](void* pMem) {
			new (pMem) ComponentType();
		};
		meta->destructor = [](void* pMem) {
			ComponentType* pComponent = reinterpret_cast<ComponentType*>(pMem);
			pComponent->~ComponentType();
		};
		meta->assignment = [](void* pDst, const void* pSrc) {
			//ComponentType* pDstComponent = reinterpret_cast<ComponentType*>(pDst);
			const ComponentType* pSrcComponent = reinterpret_cast<const ComponentType*>(pSrc);
			new (pDst) ComponentType(*pSrcComponent);
		};

		mComponentMetas.insert({ hashcode, meta });
		return meta;
	}


private:
	//World*												mWorld;
	std::unordered_map<ComponentHash, ComponentMeta*>	mComponentMetas;
	std::unordered_map<ArchetypeID, EntityArchetype*>	mArchetypesMap;
};

/// help class to create an archetype, T... can be:
/// a list of component types.
/// an entity class
template<typename...T>
struct CreateArchetypeHelperClass;

template<typename T, typename...U>
struct CreateArchetypeHelperClass<T, U...>
{
	static EntityArchetype* Create(EntityArchetypeManager* pManager)
	{
		if constexpr (std::is_base_of<EntityClassBase, T>::value) {
			return pManager->CreateArchetypeByEntityClass<T>();
		}
		else {
			return pManager->CreateArchetypeByComponentTypes<T, U...>();
		}
	}
};

template<>
struct CreateArchetypeHelperClass<>
{
	static EntityArchetype* Create(EntityArchetypeManager* pManager)
	{
		return pManager->CreateArchetypeByComponentTypes<>();
	}
};

/// help class:
/// Get component meta map from a list of component types
template<typename...ComponentTypes>
struct GetComponentsMetaHelperClass;

template<typename ComponentType, typename...OtherTypes>
struct GetComponentsMetaHelperClass<ComponentType, OtherTypes...>
{
	static void Call(EntityArchetypeManager* pArchetypeManager, ComponentMetaMap& metaMap)
	{
		ComponentTypeID componentTypeId = std::decay_t<ComponentType>::type_id();
		if (metaMap.find(componentTypeId) == metaMap.end()) {
			ComponentMeta* meta = pArchetypeManager->GetComponentMeta<ComponentType>();
			metaMap.insert({ componentTypeId, meta });
		}
		GetComponentsMetaHelperClass<OtherTypes...>::Call(pArchetypeManager, metaMap);
	}
};

template<>
struct GetComponentsMetaHelperClass<>
{
	static void Call(EntityArchetypeManager* pArchetypeManager, ComponentMetaMap& metaMap) { }
};

/// remove components from a component meta map
/// that will return a new meta map
template<typename...ComponentTypes>
struct RemoveComponentsFromMetaMap;

template<typename ComponentType, typename...OtherTypes>
struct RemoveComponentsFromMetaMap<ComponentType, OtherTypes...>
{
	static void Call(EntityArchetypeManager* pArchetypeManager, ComponentMetaMap& metaMap)
	{
		ComponentTypeID componentTypeId = std::decay_t<ComponentType>::type_id();
		if (metaMap.find(componentTypeId) != metaMap.end()) {
			metaMap.erase(componentTypeId);
		}
		RemoveComponentsFromMetaMap<OtherTypes...>::Call(pArchetypeManager, metaMap);
	}
};

template<>
struct RemoveComponentsFromMetaMap<>
{
	static void Call(EntityArchetypeManager* pArchetypeManager, ComponentMetaMap& metaMap) {}
};


/// create an archetype, ...T can be:
/// a list of component types.
/// an entity class
template<typename...T>
EntityArchetype* EntityArchetypeManager::CreateArchetype()
{
	return CreateArchetypeHelperClass<T...>::Create(this);
}

/// create an archetype by a list of component types
template<typename...ComponentTypes>
EntityArchetype* EntityArchetypeManager::CreateArchetypeByComponentTypes()
{
	const ArchetypeID id = sum_component_hashcodes_v<ComponentTypes...>;
	auto it = mArchetypesMap.find(id);
	if (it != mArchetypesMap.end()) {
		return it->second;
	}

	ComponentMetaMap metaMap;
	GetComponentsMetaHelperClass<ComponentTypes...>::Call(this, metaMap);
	EntityArchetype* pEntityArchetype = new EntityArchetype(this, id, metaMap);
	mArchetypesMap.insert({ id, pEntityArchetype });
	return pEntityArchetype;
}

/// Add component types to an existing archetype to create a new archetype
template<typename...ComponentTypes>
EntityArchetype* EntityArchetypeManager::AddComponents(const EntityArchetype* pArchetype)
{
	ComponentMetaMap metaMap(pArchetype->mComponentMetaMap);
	GetComponentsMetaHelperClass<ComponentTypes...>::Call(this, metaMap);
	if (metaMap.size() == pArchetype->mComponentMetaMap.size()) {
		return nullptr;
	}
	ArchetypeID id = GetArchetypeIDFromComponentMetaMap(metaMap);
	auto it = mArchetypesMap.find(id);
	if (it != mArchetypesMap.end()) {
		return it->second;
	}
	EntityArchetype* pEntityArchetype = new EntityArchetype(this, id, metaMap);
	mArchetypesMap.insert({ id, pEntityArchetype });
	return pEntityArchetype;
}

/// Remove component types from an existing archetype.
/// return a new archetype
template<typename... ComponentTypes>
EntityArchetype* EntityArchetypeManager::RemoveComponents(const EntityArchetype* pArchetype)
{
	ComponentMetaMap metaMap(pArchetype->mComponentMetaMap);
	RemoveComponentsFromMetaMap<ComponentTypes...>::Call(this, metaMap);
	if (metaMap.size() == pArchetype->mComponentMetaMap.size()) {
		return nullptr;
	}
	ArchetypeID id = GetArchetypeIDFromComponentMetaMap(metaMap);
	auto it = mArchetypesMap.find(id);
	if (it != mArchetypesMap.end()) {
		return it->second;
	}
	EntityArchetype* pEntityArchetype = new EntityArchetype(this, id, metaMap);
	mArchetypesMap.insert({ id, pEntityArchetype });
	return pEntityArchetype;
}

template<typename...ComponentTypes>
inline EntityArchetype* EntityArchetype::Extend()
{
	return mArchetypeManager->AddComponents<ComponentTypes...>(this);
}

using EntityID = uint64_t;

class Entity
{
	friend class EntityComponentChunk;
	friend class EntityComponentStorage;
	friend class EntityContext;
public:
	Entity() : mValid(false), mGenID(0)
	{

	}

	// check if two entity objects are the same
	bool operator==(const Entity& ent) const
	{
		return mChunkIndex == ent.mChunkIndex
			&& mBlockIndex == ent.mBlockIndex
			&& mGenID == ent.mGenID
			&& mStorage == ent.mStorage;
	}

	// Get EntityID of this entity.
	// EntityID is an unique id representing this entity in the system.
	// You can get entity by EntityID throught calling GetEntity() method
	inline EntityID GetEntityID() const;

	inline EntityContext* GetContext();

	inline static void ParseEntityID(EntityID eid, uint16_t* genid, 
		uint8_t* contextId, uint16_t* storageIndex,
		uint16_t* chunkIndex, uint16_t* blockIndex);

	inline static uint8_t ExtractContextIdFromEntityID(EntityID eid);

	inline void Release();
	bool IsValid() const { return mValid; }

	template<typename ComponentType>
	inline ComponentType* GetComponent();

	template<typename ComponentType>
	inline const ComponentType* GetComponent() const;

	template<typename ComponentType>
	inline int GetComponentIndex() const;

	inline int GetComponentIndex(ComponentTypeID componentTypeID) const;

	template<typename T=byte>
	inline T* GetComponentByIndex(int index);

	template<typename T=byte>
	inline T* GetComponentByTypeID(ComponentTypeID typeId);

	template<typename T = byte>
	inline const T* GetComponentByIndex(int index) const;

	template<typename T = byte>
	inline const T* GetComponentByTypeID(ComponentTypeID typeId) const;

	template<typename ComponentType>
	inline bool ContainComponent() const;

	template<typename... ComponentTypes>
	inline bool ContainAllComponents() const;

	template<typename... ComponentTypes>
	inline bool ContainAnyComponents() const;

	template<typename ComponentType>
	inline bool SetComponent(ComponentType&& component);

	inline EntityArchetype* GetArchetype();
	inline const EntityArchetype* GetArchetype() const;

	inline int GetComponentCount() const;

	// clone current entity, 
	// return a new entity
	inline Entity* Clone() const;

	// extend current entity by a list of component types.
	// the components are given in the function parameters
	// return a new entity and retain the current one at the same time
	template<typename...ComponentTypes>
	inline Entity* Extend(ComponentTypes&&... Args) const;

	// extend current entity by a list of component types.
	// return a new entity and retain the current one at the same time
	template<typename...ComponentTypes>
	inline Entity* Extend() const;

	// remove component types from current entity.
	// return a new entity and retain the current one at the same time
	template<typename...ComponentTypes>
	inline Entity* Remove() const;


private:
	bool					mValid;
	uint16_t				mGenID; // an 16 bit id generated automatically, to do validation check
	uint16_t				mChunkIndex = 0; // the chunkIndex inside a storage
	uint16_t				mBlockIndex = 0; // block index inside a chunk
	EntityComponentStorage*	mStorage = nullptr;
};


template<typename...ComponentTypes>
struct GetComponentIndexesHelperClass;

template<typename ComponentType, typename...Rest>
struct GetComponentIndexesHelperClass<ComponentType, Rest...>
{
	static void Call(EntityArchetype* pArchetype, int* indexArray, int currentIndex)
	{
		indexArray[currentIndex] = pArchetype->GetComponentIndex<ComponentType>();
		GetComponentIndexesHelperClass<Rest...>::Call(pArchetype, indexArray, currentIndex + 1);
	}
};

template<>
struct GetComponentIndexesHelperClass<>
{
	static void Call(EntityArchetype* pArchetype, int* indexArray, int currentIndex) {}
};

/// EntityComponentChunk:
/// is a chunk of memory that contains N entities with (components)
/// Memory Layout:
/// FreeList
/// entity1 | entity2 | ...... | entity N |
/// component1 | component1 | ...... | component1 |
/// component2 | component2 | ...... | component2 |
class EntityComponentChunk
{
public:
	EntityComponentChunk(uint16_t chunkId, EntityComponentStorage* pStorage, 
		EntityArchetype* pArchetype,
		size_t n, size_t chunkSize)
		: mChunkId(chunkId)
		, mEntityComponentStorage(pStorage)
		, mArchetype(pArchetype)
		, mBlockCount((uint16_t)n)
		, mChunkSize(chunkSize)
		, mComponentCount((int)pArchetype->mComponentCount)
		, mUsedCount(0)
	{
		//mMem = (byte*)malloc(mBlockCount * mBlockSize);
		void* pMem = GetMemoryAllocator()->Malloc(mChunkSize);

		mFreeList = reinterpret_cast<uint16_t*>(pMem);
		//mEntitiesBuffer = reinterpret_cast<Entity*>(mFreeList + mBlockCount);
		void* pEntityBufferAddress = get_next_aligned_address(mFreeList + mBlockCount, std::alignment_of_v<Entity>);
		mEntitiesBuffer = reinterpret_cast<Entity*>(pEntityBufferAddress);

		//mComponentsBuffer = reinterpret_cast<byte*>(mEntitiesBuffer + mBlockCount);
		byte* pComponentBufferAddress = (byte*)(mEntitiesBuffer + mBlockCount);
		for (int i = 0; i < mComponentCount; i++) {
			size_t componentAlignment = pArchetype->mComponentAlignments[i];
			size_t componentSize = pArchetype->mComponentSizes[i];
			mComponentBuffers[i] = (byte*)get_next_aligned_address(pComponentBufferAddress, componentAlignment);
			pComponentBufferAddress = mComponentBuffers[i] + mBlockCount * componentSize;
		}

		// init free list
		for (uint16_t i = 0; i < mBlockCount; i++) {
			mFreeList[i] = i + 1;
		}
		mFreeHead = 0;
		mFreeTail = mBlockCount;

		for (uint16_t i = 0; i < mBlockCount; i++) {
			Entity* pEntity = &mEntitiesBuffer[i];
			pEntity->mValid = false;
			pEntity->mGenID = 0;
			pEntity->mBlockIndex = i;
			pEntity->mChunkIndex = mChunkId;
			pEntity->mStorage = mEntityComponentStorage;
		}
	}

	inline IChunkMemoryAllocator* GetMemoryAllocator();

	Entity* Allocate(bool bCallConstruct)
	{
		FASTECS_ASSERT(mFreeHead != mFreeTail);
		uint16_t head = mFreeHead;
		mFreeHead = mFreeList[head];
		Entity* pEntity = &mEntitiesBuffer[head];
		pEntity->mValid = true;
		pEntity->mGenID = (pEntity->mGenID + 1) & 0x0000FFFF; // mGenID just has 16 bits
		FASTECS_ASSERT(pEntity->mBlockIndex == head);
		// construct components
		if (bCallConstruct)
			ConstructComponents(pEntity);
		mUsedCount++;
		return pEntity;
	}

	// Construct an entity by calling its components' constructors
	void ConstructComponents(Entity* pEntity)
	{
		for (int i = 0; i < mComponentCount; i++) {
			byte* pMem = GetComponentByIndex(pEntity, i);
			ComponentConstructor* constructor = mArchetype->mComponentConstructors[i];
			(*constructor)(pMem);
		}
	}
	
	// Destroy an entity
	// bCallDestructor: if need to call its components' destructors
	void Deallocate(Entity* pEntity, bool bCallDestructor)
	{
		FASTECS_ASSERT(pEntity->mChunkIndex == mChunkId);
		if (bCallDestructor)
			DestructComponents(pEntity);
		mFreeList[pEntity->mBlockIndex] = mFreeHead;
		mFreeHead = pEntity->mBlockIndex;
		pEntity->mValid = false;
		mUsedCount--;
	}

	// Destroy an entity by calling its components' destructors.
	void DestructComponents(Entity* pEntity)
	{
		for (int i = 0; i < mComponentCount; i++) {
			byte* pMem = GetComponentByIndex(pEntity, i);
			ComponentDestructor* destructor = mArchetype->mComponentDestructors[i];
			(*destructor)(pMem);
		}
	}

	// if there is no empty space
	bool IsFull() const 
	{
		return mFreeHead == mFreeTail;
	}

	// get entity by its index
	Entity* GetEntity(uint16_t blockIndex)
	{
		return &mEntitiesBuffer[blockIndex];
	}

	static size_t CalculateBlockSize(EntityArchetype* pArchetype)
	{
		int n = (int)pArchetype->mComponentCount;
		size_t blockSize = 0;
		blockSize += sizeof(uint16_t); // freeList
		blockSize += sizeof(Entity); // entity
		// all components
		for (int i = 0; i < n; i++) {
			blockSize += pArchetype->mComponentSizes[i];
		}
		return blockSize;
	}

	template<typename ComponentType>
	ComponentType* GetComponent(Entity* pEntity)
	{
		int index = mArchetype->GetComponentIndex<ComponentType>();
		if (index == INVALID_COMPONENT_INDEX)
			return nullptr;
		size_t size = mArchetype->mComponentSizes[index];
		auto pComponent = reinterpret_cast<ComponentType*>(mComponentBuffers[index] + (size * pEntity->mBlockIndex));
		FASTECS_ASSERT(check_aligned_address(pComponent));
		return pComponent;
	}

	template<typename ComponentType>
	const ComponentType* GetComponent(const Entity* pEntity) const
	{
		int index = mArchetype->GetComponentIndex<ComponentType>();
		if (index == INVALID_COMPONENT_INDEX)
			return nullptr;
		size_t size = mArchetype->mComponentSizes[index];
		auto pComponent = reinterpret_cast<const ComponentType*>(mComponentBuffers[index] + (size * pEntity->mBlockIndex));
		FASTECS_ASSERT(check_aligned_address(pComponent));
		return pComponent;
	}

	template<typename T = byte>
	T* GetComponentByIndex(Entity* pEntity, int index)
	{
		FASTECS_ASSERT(index < mComponentCount);
		size_t size = mArchetype->mComponentSizes[index];
		T* pComponent = reinterpret_cast<T*>(mComponentBuffers[index] + (size * pEntity->mBlockIndex));
		FASTECS_ASSERT(check_aligned_address(pComponent, mArchetype->mComponentAlignments[index]));
		return pComponent;
	}

	template<typename T = byte>
	const T* GetComponentByIndex(const Entity* pEntity, int index) const
	{
		FASTECS_ASSERT(index < mComponentCount);
		size_t size = mArchetype->mComponentSizes[index];
		const T* pComponent = reinterpret_cast<const T*>(mComponentBuffers[index] + (size * pEntity->mBlockIndex));
		FASTECS_ASSERT(check_aligned_address(pComponent, mArchetype->mComponentAlignments[index]));
		return pComponent;
	}

	template<typename T = byte>
	T* GetComponentByTypeID(Entity* pEntity, ComponentTypeID componentTypeID)
	{
		int index = mArchetype->GetComponentIndex(componentTypeID);
		if (index == INVALID_COMPONENT_INDEX) {
			return nullptr;
		}
		return GetComponentByIndex<T>(pEntity, index);
	}

	template<typename T = byte>
	const T* GetComponentByTypeID(const Entity* pEntity, ComponentTypeID componentTypeID) const
	{
		int index = mArchetype->GetComponentIndex(componentTypeID);
		if (index == INVALID_COMPONENT_INDEX) {
			return nullptr;
		}
		return GetComponentByIndex<T>(pEntity, index);
	}

	template<typename TupleType, int startIndex, int currentIndex, typename...ComponentTypes>
	struct GetComponentsTupleHelperClass;

	template<typename TupleType, int startIndex, int currentIndex, typename ComponentType, typename...Rest>
	struct GetComponentsTupleHelperClass<TupleType, startIndex, currentIndex, ComponentType, Rest...>
	{
		static void Call(TupleType& componentsTuple, byte* componentsByteArray[]) {
			// use 'currentIndex + 1', since position 0 is reserved for Entity pointer
			std::get<currentIndex + startIndex>(componentsTuple) = reinterpret_cast<ComponentType*>(componentsByteArray[currentIndex]);
			GetComponentsTupleHelperClass<TupleType, startIndex, currentIndex + 1, Rest...>::Call(componentsTuple, componentsByteArray);
		}
	};

	template<typename TupleType, int startIndex, int currentIndex>
	struct GetComponentsTupleHelperClass<TupleType, startIndex, currentIndex>
	{
		static void Call(TupleType& componentsTuple, byte* componentsByteArray[]) {}
	};

	template<int startIndex, int currentIndex, typename TupleType>
	void AdvanceComponentsTuple(TupleType& componentTuple)
	{
		if constexpr (currentIndex >= startIndex)
		{
			std::get<currentIndex>(componentTuple) += 1;
			AdvanceComponentsTuple<startIndex, currentIndex - 1>(componentTuple);
		}
	}

	// ForEach without a particular argment
	template<typename T0, typename F>
	void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_1(componentsBytes, T0, p0);
		FOR_EACH_ENTITY(f, count, pEntity, p0);
	}

	template<typename T0, typename T1, typename F>
	void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_2(componentsBytes, T0, T1, p0, p1);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1);
	}

	template<typename T0, typename T1, typename T2, typename F>
	void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_3(componentsBytes, T0, T1, T2, p0, p1, p2);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1, p2);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename F>
	void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_4(componentsBytes, T0, T1, T2, T3, p0, p1, p2, p3);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1, p2, p3);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename F>
	void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_5(componentsBytes, T0, T1, T2, T3, T4, p0, p1, p2, p3, p4);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1, p2, p3, p4);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename F>
	void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_6(componentsBytes, T0, T1, T2, T3, T4, T5, p0, p1, p2, p3, p4, p5);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1, p2, p3, p4, p5);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename F>
	void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_7(componentsBytes, T0, T1, T2, T3, T4, T5, T6, p0, p1, p2, p3, p4, p5, p6);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1, p2, p3, p4, p5, p6);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename F>
	void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_8(componentsBytes, T0, T1, T2, T3, T4, T5, T6, T7, p0, p1, p2, p3, p4, p5, p6, p7);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1, p2, p3, p4, p5, p6, p7);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7,
		typename T8, typename F>
		void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_9(componentsBytes, T0, T1, T2, T3, T4, T5, T6, T7, T8, p0, p1, p2, p3, p4, p5, p6, p7, p8);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1, p2, p3, p4, p5, p6, p7, p8);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7,
		typename T8, typename T9, typename F>
		void _DoForEach(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_10(componentsBytes, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		FOR_EACH_ENTITY(f, count, pEntity, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
	}

	// ForEach with a particular argument
	template<typename T0, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_1(componentsBytes, T0, p0);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0);
	}

	template<typename T0, typename T1, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_2(componentsBytes, T0, T1, p0, p1);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1);
	}

	template<typename T0, typename T1, typename T2, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_3(componentsBytes, T0, T1, T2, p0, p1, p2);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1, p2);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_4(componentsBytes, T0, T1, T2, T3, p0, p1, p2, p3);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1, p2, p3);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_5(componentsBytes, T0, T1, T2, T3, T4, p0, p1, p2, p3, p4);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1, p2, p3, p4);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_6(componentsBytes, T0, T1, T2, T3, T4, T5, p0, p1, p2, p3, p4, p5);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1, p2, p3, p4, p5);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_7(componentsBytes, T0, T1, T2, T3, T4, T5, T6, p0, p1, p2, p3, p4, p5, p6);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1, p2, p3, p4, p5, p6);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_8(componentsBytes, T0, T1, T2, T3, T4, T5, T6, T7, p0, p1, p2, p3, p4, p5, p6, p7);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1, p2, p3, p4, p5, p6, p7);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, 
		typename T8, typename F, typename RuntimeArg>
	void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_9(componentsBytes, T0, T1, T2, T3, T4, T5, T6, T7, T8, p0, p1, p2, p3, p4, p5, p6, p7, p8);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1, p2, p3, p4, p5, p6, p7, p8);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7,
		typename T8, typename T9, typename F, typename RuntimeArg>
		void _DoForEach(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		CAST_COMPONENTS_10(componentsBytes, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		FOR_EACH_ENTITY1(pArg, f, count, pEntity, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
	}


	template<typename...ComponentTypes, typename F, typename RuntimeArg>
	void _DoForEach_WithTooManyComponents(F&& f, RuntimeArg* pArg, int count, Entity* pEntity, byte* componentsBytes[])
	{
		constexpr int n = sizeof...(ComponentTypes);
		using ComponentTuple = std::tuple<RuntimeArg*, Entity*, std::decay_t<ComponentTypes> *...>;
		ComponentTuple componentTuple;
		std::get<0>(componentTuple) = pArg;
		std::get<1>(componentTuple) = pEntity;
		GetComponentsTupleHelperClass<ComponentTuple, 2, 0, std::decay_t<ComponentTypes>...>::Call(componentTuple, componentsBytes);

		for (int i = 0; i < count; i++) {
			if (pEntity->IsValid()) {
				std::apply(std::forward<F>(f), componentTuple);
			}
			AdvanceComponentsTuple<2, n + 1>(componentTuple);
			pEntity++;
			std::get<1>(componentTuple) = pEntity;
		}
	}

	template<typename...ComponentTypes, typename F>
	void _DoForEach_WithTooManyComponents(F&& f, int count, Entity* pEntity, byte* componentsBytes[])
	{
		constexpr int n = sizeof...(ComponentTypes);
		using ComponentTuple = std::tuple<Entity*, std::decay_t<ComponentTypes> *...>;
		ComponentTuple componentTuple;
		std::get<0>(componentTuple) = pEntity;
		GetComponentsTupleHelperClass<ComponentTuple, 1, 0, std::decay_t<ComponentTypes>...>::Call(componentTuple, componentsBytes);

		for (int i = 0; i < count; i++) {
			if (pEntity->IsValid()) {
				std::apply(std::forward<F>(f), componentTuple);
			}
			AdvanceComponentsTuple<1, n>(componentTuple);
			pEntity++;
			std::get<0>(componentTuple) = pEntity;
		}
	}

	template<typename...ComponentTypes, typename F, typename RuntimeArg>
	void ForEach(F&& f, RuntimeArg* pArg, int startBlockIndex, int endBlockIndex)
	{
		//using ComponentTuple = std::tuple<RuntimeArg*, Entity*, std::decay_t<ComponentTypes>*...>;
		constexpr int n = sizeof...(ComponentTypes);
		int componentIndexes[n];
		GetComponentIndexesHelperClass<ComponentTypes...>::Call(mArchetype, componentIndexes, 0);
		Entity* pEntity = &mEntitiesBuffer[startBlockIndex];

		byte* componentsBytes[n] = { 0 };
		for (int i = 0; i < n; i++) {
			int index = componentIndexes[i];
			//size_t offset = mArchetype->mComponentOffsets[index];
			size_t componentSize = mArchetype->mComponentSizes[index];
			//componentsBytes[i] = mComponentsBuffer + mBlockCount * offset + startBlockIndex * componentSize;
			componentsBytes[i] = mComponentBuffers[index] + startBlockIndex * componentSize;
		}

		if constexpr (n <= 10)
		{
			_DoForEach<ComponentTypes...>(std::forward<F>(f), pArg, endBlockIndex - startBlockIndex, pEntity, componentsBytes);
		}
		else
		{
			_DoForEach_WithTooManyComponents< ComponentTypes...>(std::forward<F>(f), pArg, endBlockIndex - startBlockIndex, pEntity, componentsBytes);
		}
	}

	template<typename...ComponentTypes, typename F>
	void ForEach(F&& f, int startBlockIndex, int endBlockIndex)
	{
		//using ComponentTuple = std::tuple<Entity*, std::decay_t<ComponentTypes>*...>;
		constexpr int n = sizeof...(ComponentTypes);
		int componentIndexes[n];
		GetComponentIndexesHelperClass<ComponentTypes...>::Call(mArchetype, componentIndexes, 0);
		Entity* pEntity = &mEntitiesBuffer[startBlockIndex];

		byte* componentsBytes[n] = { 0 };
		for (int i = 0; i < n; i++) {
			int index = componentIndexes[i];
			//size_t offset = mArchetype->mComponentOffsets[index];
			size_t componentSize = mArchetype->mComponentSizes[index];
			//componentsBytes[i] = mComponentsBuffer + mBlockCount * offset + startBlockIndex * componentSize;
			componentsBytes[i] = mComponentBuffers[index] + startBlockIndex * componentSize;
		}

		if constexpr (n <= 10)
		{
			_DoForEach<ComponentTypes...>(std::forward<F>(f), endBlockIndex - startBlockIndex, pEntity, componentsBytes);
		}
		else
		{
			_DoForEach_WithTooManyComponents< ComponentTypes...>(std::forward<F>(f), endBlockIndex - startBlockIndex, pEntity, componentsBytes);
		}
	}

	template<typename F, typename...ComponentTypes>
	void ForEach(F&& f, int* componentIndexes)
	{
		//using ComponentTuple = std::tuple<Entity*, std::decay_t<ComponentTypes>*...>;
		constexpr int n = sizeof...(ComponentTypes);
		Entity* pEntity = &mEntitiesBuffer[0];

		byte* componentsBytes[n] = { 0 };
		for (int i = 0; i < n; i++) {
			int index = componentIndexes[i];
			//size_t offset = mArchetype->mComponentOffsets[index];
			//componentsBytes[i] = mComponentsBuffer + mBlockCount * offset;
			componentsBytes[i] = mComponentBuffers[index];
		}

		if constexpr (n <= 10)
		{
			_DoForEach<ComponentTypes...>(std::forward<F>(f), mBlockCount, pEntity, componentsBytes);
		}
		else
		{
			_DoForEach_WithTooManyComponents< ComponentTypes...>(std::forward<F>(f), mBlockCount, pEntity, componentsBytes);
		}
	}

	template<typename F, typename RuntimeArg, typename...ComponentTypes>
	void ForEach(F&& f, RuntimeArg* pArg, int* componentIndexes)
	{
		//using ComponentTuple = std::tuple<Entity*, std::decay_t<ComponentTypes>*...>;
		constexpr int n = sizeof...(ComponentTypes);
		Entity* pEntity = &mEntitiesBuffer[0];

		byte* componentsBytes[n] = { 0 };
		for (int i = 0; i < n; i++) {
			int index = componentIndexes[i];
			//size_t offset = mArchetype->mComponentOffsets[index];
			//componentsBytes[i] = mComponentsBuffer + mBlockCount * offset;
			componentsBytes[i] = mComponentBuffers[index];
		}

		if constexpr (n <= 10)
		{
			_DoForEach<ComponentTypes...>(std::forward<F>(f), pArg, mBlockCount, pEntity, componentsBytes);
		}
		else
		{
			_DoForEach_WithTooManyComponents< ComponentTypes...>(std::forward<F>(f), pArg, mBlockCount, pEntity, componentsBytes);
		}
	}

	template<typename F, typename...ComponentTypes>
	void ForEachBatch(F&& f, int* componentIndexes)
	{
		using ComponentTuple = std::tuple<Entity*, int, std::decay_t<ComponentTypes> *...>;
		constexpr int n = sizeof...(ComponentTypes);
		Entity* pEntity = &mEntitiesBuffer[0];

		byte* componentsBytes[n] = { 0 };
		for (int i = 0; i < n; i++) {
			int index = componentIndexes[i];
			//size_t offset = mArchetype->mComponentOffsets[index];
			//componentsBytes[i] = mComponentsBuffer + mBlockCount * offset;
			componentsBytes[i] = mComponentBuffers[index];
		}

		ComponentTuple componentTuple;
		std::get<0>(componentTuple) = pEntity;
		std::get<1>(componentTuple) = mBlockCount;
		GetComponentsTupleHelperClass<ComponentTuple, 2, 0, std::decay_t<ComponentTypes>...>::Call(componentTuple, componentsBytes);
		std::apply(std::forward<F>(f), componentTuple);
	}

	template<typename F, typename RuntimeArg, typename...ComponentTypes>
	void ForEachBatch(F&& f, RuntimeArg* pArg, int* componentIndexes)
	{
		using ComponentTuple = std::tuple<RuntimeArg*, Entity*, int, std::decay_t<ComponentTypes> *...>;
		constexpr int n = sizeof...(ComponentTypes);
		Entity* pEntity = &mEntitiesBuffer[0];

		byte* componentsBytes[n] = { 0 };
		for (int i = 0; i < n; i++) {
			int index = componentIndexes[i];
			//size_t offset = mArchetype->mComponentOffsets[index];
			//componentsBytes[i] = mComponentsBuffer + mBlockCount * offset;
			componentsBytes[i] = mComponentBuffers[index];
		}

		ComponentTuple componentTuple;
		std::get<0>(componentTuple) = pArg;
		std::get<1>(componentTuple) = pEntity;
		std::get<2>(componentTuple) = mBlockCount;
		GetComponentsTupleHelperClass<ComponentTuple, 3, 0, std::decay_t<ComponentTypes>...>::Call(componentTuple, componentsBytes);
		std::apply(std::forward<F>(f), componentTuple);
	}

	template<typename...ComponentTypes, typename F>
	void ForEachBatch(F&& f, int startBlockIndex, int endBlockIndex)
	{
		using ComponentTuple = std::tuple<Entity*, int, std::decay_t<ComponentTypes>*...>;
		constexpr int n = sizeof...(ComponentTypes);
		int componentIndexes[n];
		GetComponentIndexesHelperClass<ComponentTypes...>::Call(mArchetype, componentIndexes, 0);
		Entity* pEntity = &mEntitiesBuffer[startBlockIndex];

		byte* componentsBytes[n] = { 0 };
		for (int i = 0; i < n; i++) {
			int index = componentIndexes[i];
			//size_t offset = mArchetype->mComponentOffsets[index];
			size_t componentSize = mArchetype->mComponentSizes[index];
			//componentsBytes[i] = mComponentsBuffer + mBlockCount * offset + startBlockIndex * componentSize;
			componentsBytes[i] = mComponentBuffers[index] + startBlockIndex * componentSize;
		}

		ComponentTuple componentTuple;
		std::get<0>(componentTuple) = pEntity;
		std::get<1>(componentTuple) = endBlockIndex - startBlockIndex;
		GetComponentsTupleHelperClass<ComponentTuple, 2, 0, std::decay_t<ComponentTypes>...>::Call(componentTuple, componentsBytes);
		std::apply(std::forward<F>(f), componentTuple);
	}

	template<typename...ComponentTypes, typename F, typename RuntimeArg>
	void ForEachBatch(F&& f, RuntimeArg* pArg, int startBlockIndex, int endBlockIndex)
	{
		using ComponentTuple = std::tuple<RuntimeArg*, Entity*, int, std::decay_t<ComponentTypes> *...>;
		constexpr int n = sizeof...(ComponentTypes);
		int componentIndexes[n];
		GetComponentIndexesHelperClass<ComponentTypes...>::Call(mArchetype, componentIndexes, 0);
		Entity* pEntity = &mEntitiesBuffer[startBlockIndex];

		byte* componentsBytes[n] = { 0 };
		for (int i = 0; i < n; i++) {
			int index = componentIndexes[i];
			//size_t offset = mArchetype->mComponentOffsets[index];
			size_t componentSize = mArchetype->mComponentSizes[index];
			//componentsBytes[i] = mComponentsBuffer + mBlockCount * offset + startBlockIndex * componentSize;
			componentsBytes[i] = mComponentBuffers[index] + startBlockIndex * componentSize;
		}

		ComponentTuple componentTuple;
		std::get<0>(componentTuple) = pArg;
		std::get<1>(componentTuple) = pEntity;
		std::get<2>(componentTuple) = endBlockIndex - startBlockIndex;
		GetComponentsTupleHelperClass<ComponentTuple, 3, 0, std::decay_t<ComponentTypes>...>::Call(componentTuple, componentsBytes);
		std::apply(std::forward<F>(f), componentTuple);
	}

	bool IsEmpty() const { return mUsedCount == 0; }

	~EntityComponentChunk()
	{
		// release all entities
		for (int i = 0; i < mBlockCount; i++)
		{
			Entity* pEntity = &mEntitiesBuffer[i];
			if (pEntity->IsValid())
			{
				pEntity->Release();
			}
		}

		if (mFreeList) {
			GetMemoryAllocator()->Free(mFreeList);
			mFreeList = nullptr;
			mEntitiesBuffer = nullptr;
			//mComponentsBuffer = nullptr;
		}
	}
private:
	uint16_t			mChunkId;
	EntityComponentStorage*	mEntityComponentStorage;
	EntityArchetype*	mArchetype;
	uint16_t			mBlockCount;
	//size_t				mBlockSize;
	size_t				mChunkSize;
	int					mComponentCount;
	uint16_t			mUsedCount = 0;

	uint16_t			mFreeHead;
	uint16_t			mFreeTail;

	// FreeList is a linked list that indicates those empty slots of memory
	// Each element in freelist points to the next empty element's index
	uint16_t*			mFreeList = nullptr;
	Entity*				mEntitiesBuffer = nullptr;
	//byte*				mComponentsBuffer = nullptr;
	byte*				mComponentBuffers[MAX_COMPONENT_COUNT_PER_ENTITY] = { 0 };
	//byte*				mMem = nullptr;
};


// EntityComponentStorage:
// A container that has multiple chunks related to the same archetype
// One archetype and one context together correlates to one EntityComponentStorage
// while same archetype across different contexts have different storages
class EntityComponentStorage
{
	friend class Entity;
	template<typename...T>
	friend class ParallelJobBase;
public:
	EntityComponentStorage(EntityContext* pContext, uint16_t index, EntityArchetype* pArchetype)
		: mContext(pContext)
		, mIndex(index)
		, mArchetype(pArchetype)
		, mChunkArrayCapacity(16)
	{
		mComponentCountPerEntity = (int)pArchetype->mComponentCount;
		size_t entityBlockSize = EntityComponentChunk::CalculateBlockSize(pArchetype);

		mChunkSize = MAX_STORAGE_CHUNK_SIZE;
		mEntityCountPerChunk = mChunkSize / entityBlockSize - 1;
		if (mEntityCountPerChunk > MAX_ENTITY_COUNT_PER_CHUNK)
		{
			mEntityCountPerChunk = MAX_ENTITY_COUNT_PER_CHUNK;
			// allocate one extra block, for memory alignment
			mChunkSize = (MAX_ENTITY_COUNT_PER_CHUNK + 1) * entityBlockSize;
		}

		//mChunkFreeList = (uint16_t*)malloc(sizeof(uint16_t) * mChunkArrayCapacity);
		//mChunks = (EntityComponentChunk*)malloc(sizeof(EntityComponentChunk) * mChunkArrayCapacity);
		mChunkFreeList = (uint16_t*)GetChunkMemoryAllocator()->Malloc(sizeof(uint16_t) * mChunkArrayCapacity);
		mChunks = (EntityComponentChunk*)GetChunkMemoryAllocator()->Malloc(sizeof(EntityComponentChunk) * mChunkArrayCapacity);
		
		memset(mChunkFreeList, 0, sizeof(sizeof(uint16_t) * mChunkArrayCapacity));
		memset(mChunks, 0, sizeof(EntityComponentChunk) * mChunkArrayCapacity);
		mChunkFreeHead = mChunkCount = 0;

	}

	Entity* Allocate(bool bCallConstructor)
	{
		// find chunk that is not full
		//uint16_t freeChunkIndex = -1;
		if (mChunkFreeHead == mChunkCount) // free list is full
		{
			// don't have enough capacity
			if (mChunkCount >= mChunkArrayCapacity) {
				IncreaseCapacity();
			}
			EntityComponentChunk* pChunk = &mChunks[mChunkCount];
			//pChunk->EntityComponentChunk::EntityComponentChunk(mChunkCount, this, mArchetype, mEntityCountPerChunk, mEntityBlockSize);
			new (pChunk) EntityComponentChunk(mChunkCount, this, mArchetype, mEntityCountPerChunk, mChunkSize);
			mChunkCount += 1;
			mChunkFreeList[mChunkFreeHead] = mChunkCount;
		}
		EntityComponentChunk* pChunk = &mChunks[mChunkFreeHead];
		Entity* pEntity = pChunk->Allocate(bCallConstructor);
		if (pChunk->IsFull()) {
			mChunkFreeHead = mChunkFreeList[mChunkFreeHead];
		}
		return pEntity;
	}

	void Deallocate(Entity* pEntity, bool bCallDestructor)
	{
		EntityComponentChunk* pChunk = &mChunks[pEntity->mChunkIndex];
		bool bFull = pChunk->IsFull();
		pChunk->Deallocate(pEntity, bCallDestructor);
		if (bFull) {
			mChunkFreeList[pEntity->mChunkIndex] = mChunkFreeHead;
			mChunkFreeHead = pEntity->mChunkIndex;
		}
	}

	Entity* GetEntity(uint16_t chunkIndex, uint16_t blockIndex)
	{
		if (chunkIndex >= mChunkCount)
			return nullptr;

		Entity* pEntity = mChunks[chunkIndex].GetEntity(blockIndex);
		if (!pEntity->mValid)
			return nullptr;
		return pEntity;
	}

	Entity* CloneEntity(const Entity* pEntity)
	{
		FASTECS_ASSERT(mArchetype == pEntity->GetArchetype());
		Entity* pClonedEntity = Allocate(false);
		for (int i = 0; i < mComponentCountPerEntity; i++)
		{
			const byte* pSrcMem = GetComponentByIndex(pEntity, i);
			byte* pDstMem = GetComponentByIndex(pClonedEntity, i);
			ComponentAssignment* pAssignment = mArchetype->mComponentAssignments[i];
			(*pAssignment)(pDstMem, pSrcMem);
		}
		return pClonedEntity;
	}

	~EntityComponentStorage()
	{
		for (uint16_t i = 0; i < mChunkCount; i++)
		{
			mChunks[i].~EntityComponentChunk();
		}
		//free(mChunkFreeList);
		//free(mChunks);
		GetChunkMemoryAllocator()->Free(mChunkFreeList);
		GetChunkMemoryAllocator()->Free(mChunks);
	}

	EntityArchetype* GetArchetype() { return mArchetype; }
	const EntityArchetype* GetArchetype() const { return mArchetype; }

	template<typename ComponentType>
	ComponentType* GetComponent(Entity* pEntity)
	{
		return mChunks[pEntity->mChunkIndex].GetComponent<ComponentType>(pEntity);
	}

	template<typename ComponentType>
	const ComponentType* GetComponent(const Entity* pEntity) const
	{
		const EntityComponentChunk* pChunk = &mChunks[pEntity->mChunkIndex];
		return pChunk->GetComponent<ComponentType>(pEntity);
	}

	template<typename T = byte>
	T* GetComponentByIndex(Entity* pEntity, int index)
	{
		return mChunks[pEntity->mChunkIndex].GetComponentByIndex<T>(pEntity, index);
	}

	template<typename T = byte>
	const T* GetComponentByIndex(const Entity* pEntity, int index) const
	{
		return mChunks[pEntity->mChunkIndex].GetComponentByIndex<T>(pEntity, index);
	}

	template<typename T = byte>
	T* GetComponentByTypeID(Entity* pEntity, ComponentTypeID componentTypeID)
	{
		return mChunks[pEntity->mChunkIndex].GetComponentByTypeID<T>(pEntity, componentTypeID);
	}

	template<typename T = byte>
	const T* GetComponentByTypeID(const Entity* pEntity, ComponentTypeID componentTypeID) const
	{
		return mChunks[pEntity->mChunkIndex].GetComponentByTypeID<T>(pEntity, componentTypeID);
	}

	uint16_t GetIndex() const { return mIndex; }

	template<typename F, typename...ComponentTypes>
	void ForEach(F&& f)
	{
		constexpr int n = sizeof...(ComponentTypes);
		int componentIndexes[n];
		GetComponentIndexesHelperClass<ComponentTypes...>::Call(mArchetype, componentIndexes, 0);

		for (int i = 0; i < mChunkCount; i++) {
			if (!mChunks[i].IsEmpty()) {
				mChunks[i].ForEach<F, ComponentTypes...>(std::forward<F>(f), componentIndexes);
			}
		}
	}

	template<typename F, typename RuntimeArg, typename...ComponentTypes>
	void ForEach(F&& f, RuntimeArg* pArg)
	{
		constexpr int n = sizeof...(ComponentTypes);
		int componentIndexes[n];
		GetComponentIndexesHelperClass<ComponentTypes...>::Call(mArchetype, componentIndexes, 0);

		for (int i = 0; i < mChunkCount; i++) {
			if (!mChunks[i].IsEmpty()) {
				mChunks[i].ForEach<F, RuntimeArg, ComponentTypes...>(std::forward<F>(f), pArg, componentIndexes);
			}
		}
	}

	template<typename F, typename...ComponentTypes>
	void ForEachBatch(F&& f)
	{
		constexpr int n = sizeof...(ComponentTypes);
		int componentIndexes[n];
		GetComponentIndexesHelperClass<ComponentTypes...>::Call(mArchetype, componentIndexes, 0);

		for (int i = 0; i < mChunkCount; i++) {
			if (!mChunks[i].IsEmpty()) {
				mChunks[i].ForEachBatch<F, ComponentTypes...>(std::forward<F>(f), componentIndexes);
			}
		}
	}

	template<typename F, typename RuntimeArg, typename...ComponentTypes>
	void ForEachBatch(F&& f, RuntimeArg* pArg)
	{
		constexpr int n = sizeof...(ComponentTypes);
		int componentIndexes[n];
		GetComponentIndexesHelperClass<ComponentTypes...>::Call(mArchetype, componentIndexes, 0);

		for (int i = 0; i < mChunkCount; i++) {
			if (!mChunks[i].IsEmpty()) {
				mChunks[i].ForEachBatch<F, RuntimeArg, ComponentTypes...>(std::forward<F>(f), pArg, componentIndexes);
			}
		}
	}

	EntityComponentChunk* GetChunk(int index) 
	{
		return &mChunks[index];
	}

	inline IChunkMemoryAllocator* GetChunkMemoryAllocator();

private:

	void IncreaseCapacity()
	{
		mChunkArrayCapacity *= 2;
		//mChunkFreeList = (uint16_t*)realloc(mChunkFreeList, sizeof(uint16_t) * mChunkArrayCapacity);
		//mChunks = (EntityComponentChunk*)realloc(mChunks, sizeof(EntityComponentChunk) * mChunkArrayCapacity);
		mChunkFreeList = (uint16_t*)GetChunkMemoryAllocator()->Realloc(mChunkFreeList, sizeof(uint16_t) * mChunkArrayCapacity);
		mChunks = (EntityComponentChunk*)GetChunkMemoryAllocator()->Realloc(mChunks, sizeof(EntityComponentChunk) * mChunkArrayCapacity);
	}

private:
	EntityContext*				mContext;
	uint16_t					mIndex;
	EntityArchetype*			mArchetype;
	size_t						mEntityCountPerChunk;
	int							mComponentCountPerEntity;
	size_t						mChunkSize;
	//size_t						mEntityBlockSize;

	// freeList indicates which chunk is free
	uint16_t*					mChunkFreeList;
	
	// an array of chunks, some of whose memory might not be allocated.
	// the valid chunks are indicated by mChunkCount
	// the size of this array is indicated by mChunkArrayCapacity
	EntityComponentChunk*		mChunks;
	
	// the size of chunk array
	uint16_t					mChunkArrayCapacity;

	// the valid chunk's count
	uint16_t					mChunkCount;
	uint16_t					mChunkFreeHead;
};

#if EVENT_INDEX_TABLE_TYPE == 0
using EventIndexTable = LinearComponentIndexTable<MAX_EVENT_COUNT>;
#elif EVENT_INDEX_TABLE_TYPE == 1
using EventIndexTable = HashMapComponentIndexTable;
#elif EVENT_INDEX_TABLE_TYPE == 2
#if USE_CUSTOM_EVENT_TYPE_ID
using EventIndexTable = DirectComponentIndexTable<MAX_EVENT_COUNT>;
#else
using EventIndexTable = LinearComponentIndexTable<MAX_EVENT_COUNT>;
#endif
#endif

// CreateEntityEvent
// when a new entity is created, this event will be triggered
struct CreateEntityEvent : EventBaseClass(CreateEntityEvent, MAX_EVENT_COUNT - 1)
{
	Entity* pEntity = nullptr;
	CreateEntityEvent() {}
	CreateEntityEvent(Entity* pEntity) : pEntity(pEntity){}
};

// DeleteEntityEvent:
// when an entity is deleted, this event will be triggered
struct DeleteEntityEvent : EventBaseClass(DeleteEntityEvent, MAX_EVENT_COUNT - 2)
{
	Entity* pEntity = nullptr;
	DeleteEntityEvent() {}
	DeleteEntityEvent(Entity* pEntity) : pEntity(pEntity) {}
};

using EventCallbackFunction = std::function<void(const void*)>;
using EventCallbackHandle = uint64_t;

// each event has an id and a callback function
struct EventCallbackObject
{
	EventCallbackHandle			id;
	EventCallbackFunction		callback;
};

using EventCallbackList = std::list<EventCallbackObject>;

class EventManager
{
public:

	// Subscrible an event by giving it a callback function
	// return a handle which can be used to unsubscribe an specific event
	template<typename EventType, typename F>
	EventCallbackHandle Subscrible(F&& cb)
	{
		int index = mIndexTable.Get<EventType>();
		if (index == INVALID_EVENT_INDEX) {
			index = mIndexTable.Add<EventType>();
		}
		uint32_t genid = GenID();
		EventCallbackObject callbackObject;
		callbackObject.id = genid;
		callbackObject.callback = [&cb](const void* pMem) {
			const EventType* pEvent = reinterpret_cast<const EventType*>(pMem);
			cb(pEvent);
		};
		mEventCallbacks[index].push_back(std::move(callbackObject));
		return ((uint64_t)index << 32) | genid;
	}

	// Unsubscribe an event by giving a handle
	void Unsubscribe(EventCallbackHandle handle) 
	{
		uint32_t index = (uint32_t)(handle >> 32);
		uint32_t id = handle & 0x0ffffffff;
		EventCallbackList& callbackList = mEventCallbacks[index];
		for (auto it = callbackList.begin(); it != callbackList.end(); it++) {
			if (it->id == id) {
				callbackList.erase(it);
				break;
			}
		}
	}

	// Unsubscribe all listeners for one specific event
	template<typename EventType>
	void Unsubscribe()
	{
		int index = mIndexTable.Get<EventType>();
		if (index == INVALID_EVENT_INDEX)
			return;
		EventCallbackList& callbackList = mEventCallbacks[index];
		callbackList.clear();
	}

	// Trigger an event
	// For CreateEntityEvent and DeleteEntityEvent, this function will be called implicitly by the system
	template<typename EventType>
	void TriggerEvent(EventType& evt)
	{
		int index = mIndexTable.Get<EventType>();
		if (index == INVALID_EVENT_INDEX) {
			return;
		}
		EventCallbackList& callbackList = mEventCallbacks[index];
		for (auto it = callbackList.begin(); it != callbackList.end(); it++) {
			it->callback(&evt);
		}
	}

	void Release()
	{
		delete this;
	}

private:

	// Generate an event id
	static uint32_t GenID()
	{
		static uint32_t id = 0;
		return ++id;
	}

private:
	EventIndexTable		mIndexTable;
	EventCallbackList	mEventCallbacks[MAX_EVENT_COUNT];
};


template<typename...ComponentTypes>
struct ComponentTypesHelperClass;

template<typename ComponentType, typename...Rest>
struct ComponentTypesHelperClass<ComponentType, Rest...>
{
	/// Helper class to call all the constructors in the list of components, with the arguments given
	static void ConstructEntity(Entity* pEntity, ComponentType&& data, Rest&&... args)
	{
		using T = std::decay_t<ComponentType>;
		if constexpr (!is_type_duplicate_v<T, Rest...>)
		{
			T* pComponentMem = pEntity->GetComponent<T>();
			if (pComponentMem) {
				new (pComponentMem) T(std::forward<ComponentType>(data));
			}
		}
		ComponentTypesHelperClass<Rest...>::ConstructEntity(pEntity, std::forward<Rest>(args)...);
	}

	/// Helper class to call all the constructors in the list of components
	static void ConstructEntity(Entity* pEntity)
	{
		using T = std::decay_t<ComponentType>;
		if constexpr (!is_type_duplicate_v<T, Rest...>)
		{
			T* pComponentMem = pEntity->GetComponent<T>();
			if (pComponentMem) {
				new (pComponentMem) T();
			}
		}
		ComponentTypesHelperClass<Rest...>::ConstructEntity(pEntity);
	}

	/// check if componentTypeId is contained in the list of the given components
	static bool Contain(ComponentTypeID componentTypeId)
	{
		return componentTypeId == std::decay_t<ComponentType>::type_id() || ComponentTypesHelperClass<Rest...>::Contain(componentTypeId);
	}
};

template<>
struct ComponentTypesHelperClass<>
{
	static void ConstructEntity(Entity* pEntity) {}
	static bool Contain(ComponentTypeID) { return false; }
};

/// EntityContext:
/// A world can have multiple contexts, 
/// entities across different contexts are independent, cannot communicate with each other 
class EntityContext
{
	friend class World;
	friend class EntityArchetype;
	friend class Entity;

	template<typename...ComponentTypes>
	friend class ParallelJobBase;
public:
	EntityContext(int id, World* pWorld, EntityArchetypeManager* pArchetypeManager)
		:mContextId(id), mWorld(pWorld), mArchetypeManager(pArchetypeManager)
	{

	}

	// create an entity by an archetype
	Entity* CreateEntity(EntityArchetype* pArchetype)
	{
		EntityComponentStorage* pStorage = GetEntityComponentStorage(pArchetype);
		Entity* pEntity = pStorage->Allocate(true);
		OnEntityCreated(pEntity);
		return pEntity;
	}

	// create an entity with a list of components as its parameters
	template<typename...Args>
	Entity* CreateEntity(EntityArchetype* pArchetype, Args&&...args)
	{
		EntityComponentStorage* pStorage = GetEntityComponentStorage(pArchetype);
		Entity* pEntity = pStorage->Allocate(false);
		int componentCount = pArchetype->mComponentCount;
		
		// construct those components whose values aren't provided through parameters
		for (int i = 0; i < componentCount; i++) {
			ComponentTypeID componentTypeId = pArchetype->mComponentTypeIds[i];
			if (!ComponentTypesHelperClass<Args...>::Contain(componentTypeId))
			{
				byte* pComponentBytes = pStorage->GetComponentByIndex(pEntity, i);
				ComponentConstructor* pConstructor = pArchetype->mComponentConstructors[i];
				(*pConstructor)(pComponentBytes);
			}
		}

		// construct those components in parameters
		ComponentTypesHelperClass<Args...>::ConstructEntity(pEntity, std::forward<Args>(args)...);
		OnEntityCreated(pEntity);
		return pEntity;
	}

	/// create an entity with a list of component types.
	/// it will call the default constructors of these components while creating the entity.
	template<typename...ComponentTypes>
	Entity* CreateEntity()
	{
		EntityArchetype* pArchetype = mArchetypeManager->CreateArchetype<ComponentTypes...>();
		return CreateEntity(pArchetype);
	}

	template<typename...ComponentTypes, typename...Args>
	Entity* CreateEntity(Args&&... args)
	{
		// 1,if component types are given, then use these types to create entity.
		// then the component values to initialize entity are given as arguments
		// the types whose values are not specified in arguments will be given a default value 
		// by calling the component's default constructor.

		// 2,if component types are not given, the deduce types from the arguments

		if constexpr (sizeof...(ComponentTypes) != 0)
		{
			EntityArchetype* pArchetype = mArchetypeManager->CreateArchetype<ComponentTypes...>();
			return CreateEntity(pArchetype, std::forward<Args>(args)...);
		}
		else
		{
			EntityArchetype* pArchetype = mArchetypeManager->CreateArchetype<std::decay_t<Args>...>();
			EntityComponentStorage* pStorage = GetEntityComponentStorage(pArchetype);
			Entity* pEntity = pStorage->Allocate(false);

			ComponentTypesHelperClass<Args...>::ConstructEntity(pEntity, std::forward<Args>(args)...);
			OnEntityCreated(pEntity);
			return pEntity;
		}
	}

	inline void Release();
	
	EntityComponentStorage* GetEntityComponentStorage(uint16_t index)
	{
		FASTECS_ASSERT(index < mEntityComponentStorageList.size());
		return mEntityComponentStorageList[index];
	}

	// return an entity by giving it an id
	// this id is the same gotten by GetEntityID
	Entity* GetEntity(EntityID eid)
	{
		uint16_t genid;
		uint8_t contextId;
		uint16_t chunkIndex, blockIndex, storageIndex;
		Entity::ParseEntityID(eid, &genid, &contextId, &storageIndex, &chunkIndex, &blockIndex);
		if (storageIndex >= mEntityComponentStorageList.size()) {
			return nullptr;
		}
		EntityComponentStorage* pStorage = mEntityComponentStorageList[storageIndex];
		Entity* pEntity = pStorage->GetEntity(chunkIndex, blockIndex);
		if (pEntity == nullptr)
			return nullptr;
		if (pEntity->mGenID != genid) {
			return nullptr;
		}
		return pEntity;
	}

	EntityComponentStorage* GetEntityComponentStorage(EntityArchetype* pArchetype) 
	{
		EntityComponentStorage* pStorage = pArchetype->mStoragesInContext[mContextId];
		if (!pStorage) {
			uint16_t index = (uint16_t)mEntityComponentStorageList.size();
			FASTECS_ASSERT(index < MAX_STORAGE_COUNT_PER_CONTEXT);
			pStorage = new EntityComponentStorage(this, index, pArchetype);
			mEntityComponentStorageList.push_back(pStorage);
			pArchetype->mStoragesInContext[mContextId] = pStorage;
		}
		return pStorage;
	}

	// extend an entity by giving a list of components
	// 'extend' means adding extra components to an existing entity,
	// and retain the current entity without any change
	template<typename... ComponentTypes>
	Entity* ExtendEntity(const Entity* pSrcEntity, ComponentTypes&&... args)
	{
		const EntityArchetype* pSrcArchetype = pSrcEntity->GetArchetype();
		if (pSrcArchetype->ContainAnyComponents<ComponentTypes...>()) {
			return nullptr;
		}
		
		EntityArchetype* pDstArchetype = mArchetypeManager->AddComponents<std::decay_t<ComponentTypes>...>(pSrcArchetype);
		EntityComponentStorage* pDstStorage = GetEntityComponentStorage(pDstArchetype);
		Entity* pDstEntity = pDstStorage->Allocate(false);
		CopyEntityData(pDstEntity, pSrcEntity);
		// construct new added components
		ComponentTypesHelperClass<ComponentTypes...>::ConstructEntity(pDstEntity, std::forward<ComponentTypes>(args)...);
		OnEntityCreated(pDstEntity);
		return pDstEntity;
	}

	// extend an entity by giving a list of component types
	// 'extend' means adding extra components to an existing entity,
	// and retaining the current entity without any change
	template<typename... ComponentTypes>
	Entity* ExtendEntity(const Entity* pSrcEntity)
	{
		const EntityArchetype* pSrcArchetype = pSrcEntity->GetArchetype();
		if (pSrcArchetype->ContainAnyComponents<ComponentTypes...>()) {
			return nullptr;
		}

		EntityArchetype* pDstArchetype = mArchetypeManager->AddComponents<std::decay_t<ComponentTypes>...>(pSrcArchetype);
		EntityComponentStorage* pDstStorage = GetEntityComponentStorage(pDstArchetype);
		Entity* pDstEntity = pDstStorage->Allocate(false);
		CopyEntityData(pDstEntity, pSrcEntity);
		// construct new added components
		ComponentTypesHelperClass<ComponentTypes...>::ConstructEntity(pDstEntity);
		OnEntityCreated(pDstEntity);
		return pDstEntity;
	}

	// remove components from an entity
	// return a new entity
	template<typename...ComponentTypes>
	Entity* RemoveComponentsFromEntity(const Entity* pSrcEntity) 
	{
		const EntityArchetype* pSrcArchetype = pSrcEntity->GetArchetype();
		if (!pSrcArchetype->ContainAllComponents<ComponentTypes...>()) {
			return nullptr;
		}
		EntityArchetype* pDstArchetype = mArchetypeManager->RemoveComponents<std::decay_t<ComponentTypes>...>(pSrcArchetype);
		EntityComponentStorage* pDstStorage = GetEntityComponentStorage(pDstArchetype);
		Entity* pDstEntity = pDstStorage->Allocate(false);
		CopyEntityData(pDstEntity, pSrcEntity);
		OnEntityCreated(pDstEntity);
		return pDstEntity;
	}

	// call ForEach with a list of component types and a callback function
	template<typename...ComponentTypes, typename F>
	void ForEach(F&& f)
	{
		for (EntityComponentStorage* pStorage : mEntityComponentStorageList) {
			if (pStorage->GetArchetype()->ContainAllComponents<ComponentTypes...>()) {
				pStorage->ForEach<F, ComponentTypes...>(std::forward<F>(f));
			}
		}
	}

	// call ForEach with a list of component types, a callback function as well as a runtime argument
	template<typename...ComponentTypes, typename F, typename RuntimeArg>
	void ForEach(F&& f, RuntimeArg* pArg)
	{
		for (EntityComponentStorage* pStorage : mEntityComponentStorageList) {
			if (pStorage->GetArchetype()->ContainAllComponents<ComponentTypes...>()) {
				pStorage->ForEach<F, RuntimeArg, ComponentTypes...>(std::forward<F>(f), pArg);
			}
		}
	}

	// call ForEachBatch with a list of component types, a callback function
	// ForEachBatch means callback function 'f' will not only process one entity each time,
	// but will process a batch of entities.
	// the prototype and implementation of 'f' will be different from 'ForEach'
	template<typename...ComponentTypes, typename F>
	void ForEachBatch(F&& f)
	{
		for (EntityComponentStorage* pStorage : mEntityComponentStorageList) {
			if (pStorage->GetArchetype()->ContainAllComponents<ComponentTypes...>()) {
				pStorage->ForEachBatch<F, ComponentTypes...>(std::forward<F>(f));
			}
		}
	}

	// call ForEachBatch with a list of component types, a callback function as well as a runtime argument
	template<typename...ComponentTypes, typename F, typename RuntimeArg>
	void ForEachBatch(F&& f, RuntimeArg* pArg)
	{
		for (EntityComponentStorage* pStorage : mEntityComponentStorageList) {
			if (pStorage->GetArchetype()->ContainAllComponents<ComponentTypes...>()) {
				pStorage->ForEachBatch<F, RuntimeArg, ComponentTypes...>(std::forward<F>(f), pArg);
			}
		}
	}

	// Set event manager 
	void SetEventManager(EventManager* pEventManager) { mEventManager = pEventManager; }
	
	// Get event manager
	EventManager* GetEventManager() { return mEventManager; }
	const EventManager* GetEventManager() const { return mEventManager; }

	// trigger an event
	template<typename EventType>
	void TriggerEvent(const EventType& evt)
	{
		if (mEventManager != nullptr)
		{
			mEventManager->TriggerEvent(evt);
		}
	}

	World* GetWorld() { return mWorld; }
	int GetContextId() { return mContextId; }

private:
	
	void OnEntityCreated(Entity* pEntity)
	{
		CreateEntityEvent evt(pEntity);
		TriggerEvent(evt);
	}

	void OnEntityDeleted(Entity* pEntity)
	{
		DeleteEntityEvent evt(pEntity);
		TriggerEvent(evt);
	}

public:

	// copy all components' data from pSrcEntity to pDstEntity
	void CopyEntityData(Entity* pDstEntity, const Entity* pSrcEntity)
	{
		const EntityArchetype* pSrcArchetype = pSrcEntity->GetArchetype();
		for (int i = 0; i < pSrcArchetype->mComponentCount; i++) {
			ComponentTypeID componentTypeID = pSrcArchetype->mComponentTypeIds[i];
			byte* pDstComponentMem = pDstEntity->GetComponentByTypeID(componentTypeID);
			if (pDstComponentMem) {
				const byte* pSrcComponentMem = pSrcEntity->GetComponentByIndex(i);
				ComponentAssignment* pAssignment = pSrcArchetype->mComponentAssignments[i];
				(*pAssignment)(pDstComponentMem, pSrcComponentMem);
			}
		}
	}

private:
	int					mContextId;
	std::vector<EntityComponentStorage*>		mEntityComponentStorageList;
	World*						mWorld;
	EntityArchetypeManager*		mArchetypeManager;
	EventManager*				mEventManager = nullptr;
};

/// chunk segment that is put into an parallelJob
struct ParallelJobChunkSegement
{
	EntityComponentChunk*		pChunk = nullptr;
	int							RangeStart; // inclusive
	int							RangeEnd; // exclusive
};

using ParallelJobChunkSegementList = std::list<ParallelJobChunkSegement>;

/// job state in a parallel job
enum class ParallelJobState
{
	Unprepared,
	Prepared,
	Executing,
};

/// parallelJob must inherit from this
template<typename...ComponentTypes>
class ParallelJobBase
{
public:
	void Prepare(EntityContext* pContext, int threadCount)
	{
		FASTECS_ASSERT(threadCount <= MAX_THREAD_COUNT);
		FASTECS_ASSERT(mState != ParallelJobState::Executing);
		mContext = pContext;
		mThreadCount = threadCount;
		mStartCounter.store(0);
		mCompleteCounter.store(0);

		for (int i = 0; i < MAX_THREAD_COUNT; i++) {
			mDividedJobChunkSegmentArray[i].clear();
		}

		int threadTaskCounts[MAX_THREAD_COUNT] = { 0 };
		std::vector<EntityComponentStorage*> vecStorages;
		for (EntityComponentStorage* pStorage : pContext->mEntityComponentStorageList) {
			if (pStorage->GetArchetype()->ContainAllComponents<ComponentTypes...>()) {
				vecStorages.push_back(pStorage);
			}
		}
		if (vecStorages.empty())
			return;

#if DIVIDE_PARALLEL_JOB_METHOD == 0
		// sort storages according to their chunkSize
		std::sort(vecStorages.begin(), vecStorages.end(), [](EntityComponentStorage* a, EntityComponentStorage* b) {
			return a->mEntityCountPerChunk > b->mEntityCountPerChunk;
		});

		for (EntityComponentStorage* pStorage : vecStorages) {
			uint16_t chunkCount = pStorage->mChunkCount;
			for (uint16_t i = 0; i < chunkCount; i++) {
				auto pChunk = pStorage->GetChunk((int)i);
				auto threadIndexSelected = std::min_element(threadTaskCounts, threadTaskCounts + threadCount) - threadTaskCounts;
				ParallelJobChunkSegement chunkSegment;
				chunkSegment.pChunk = pChunk;
				chunkSegment.RangeStart = 0;
				chunkSegment.RangeEnd = (int)(pStorage->mEntityCountPerChunk);
				mDividedJobChunkSegmentArray[threadIndexSelected].push_back(chunkSegment);
				threadTaskCounts[threadIndexSelected] += (int)(pStorage->mEntityCountPerChunk);
			}
		}
#elif DIVIDE_PARALLEL_JOB_METHOD == 1
		// divide one chunk only if its entities exceed this threshold
		//int entityCountThreshold = 16 * threadCount;
		for (EntityComponentStorage* pStorage : vecStorages)
		{
			int chunkCount = (int)(pStorage->mChunkCount);
			int entityCountPerChunk = (int)pStorage->mEntityCountPerChunk;
			// divide the entire chunk into many segments, each segment per thread
			int entityCountPerThread = entityCountPerChunk / threadCount;
			entityCountPerThread = entityCountPerThread / 16 * 16;
			
			for (int i = 0; i < chunkCount; i++)
			{
				// the last segment may have more than 'entityCountPerThread' entities
				// we give the last segment to the specific thread with the least tasks currently. 
				auto pChunk = pStorage->GetChunk(i);

				// find the thread with the least tasks
				auto threadIndexSelected = std::min_element(threadTaskCounts, threadTaskCounts + threadCount) - threadTaskCounts;
				int currentStartIndex = 0;

				for (int j = 0; j < threadCount; j++)
				{
					ParallelJobChunkSegement chunkSegment;
					chunkSegment.pChunk = pChunk;

					// assign the last segment to this particular thread
					if (j == threadIndexSelected)
					{
						chunkSegment.RangeStart = entityCountPerThread * (threadCount - 1);
						chunkSegment.RangeEnd = entityCountPerChunk;
					}
					else
					{
						chunkSegment.RangeStart = currentStartIndex;
						chunkSegment.RangeEnd = currentStartIndex + entityCountPerThread;
						currentStartIndex += entityCountPerThread;
					}

					auto rangeCount = chunkSegment.RangeEnd - chunkSegment.RangeStart;
					if (rangeCount > 0)
					{
						mDividedJobChunkSegmentArray[j].push_back(chunkSegment);
						threadTaskCounts[j] += rangeCount;
					}
				}
			}

		}
#endif
	}
	
	ParallelJobBase GetState() const { return mState; }

protected:
	//std::atomic<int>							mThreadCount = 0;
	//bool										mPrepared;
	ParallelJobState							mState = ParallelJobState::Unprepared;
	int											mThreadCount = 0;
	std::atomic<int>							mStartCounter = 0;
	std::atomic<int>							mCompleteCounter = 0;
	//std::atomic<int>							mThreadCount = 0;
	EntityContext*								mContext = nullptr;
	ParallelJobChunkSegementList				mDividedJobChunkSegmentArray[MAX_THREAD_COUNT];
};

/// ParallelJob:
/// can be run across multiple thread and execute ForEach algorithm parallelly
template<bool IsThreadLocalArgRequired, typename...T>
class ParallelJob;

/// ParallelJob without runtime local argument
/// the first template parameter must be true
template<typename ThreadLocalArg, typename...ComponentTypes>
class ParallelJob<true, ThreadLocalArg, ComponentTypes...> : public ParallelJobBase<ComponentTypes...>
{
public:
	using base = ParallelJobBase<ComponentTypes...>;
	//using TJobFunctionType = void(*)(ThreadLocalArg*, Entity*, ComponentTypes *...);

	ParallelJob():mFunction(nullptr){}

	template<typename F>
	ParallelJob(F&& f) : mFunction(std::forward<F>(f))
	{

	}

	template<typename F>
	void SetJobFunction(F&& f)
	{
		mFunction = std::forward<F>(f);
	}

	void Execute(ThreadLocalArg* pThreadLocalArg);
private:
	std::function<void(ThreadLocalArg*, Entity*, ComponentTypes*...)>	mFunction;
	//TJobFunctionType mFunction;
};

/// ParallelJob with a runtime local argument
/// the first template parameter is set to false
template<typename...ComponentTypes>
class ParallelJob<false, ComponentTypes...> : public ParallelJobBase<ComponentTypes...>
{
public:
	using base = ParallelJobBase<ComponentTypes...>;

	ParallelJob() :mFunction(nullptr) {}

	template<typename F>
	ParallelJob(F&& f) : mFunction(std::forward<F>(f))
	{

	}

	template<typename F>
	void SetJobFunction(F&& f) 
	{
		mFunction = std::forward<F>(f);
	}

	void Execute();
private:
	std::function<void(Entity*, ComponentTypes*...)>		mFunction;
};

/// ParallelBatchJob:
/// can be run across multiple thread and execute ForEachBatch algorithm parallelly
template<bool IsThreadLocalArgRequired, typename...T>
class ParallelBatchJob;

template<typename ThreadLocalArg, typename...ComponentTypes>
class ParallelBatchJob<true, ThreadLocalArg, ComponentTypes...> : public ParallelJobBase<ComponentTypes...>
{
public:
	using base = ParallelJobBase<ComponentTypes...>;
	ParallelBatchJob() :mFunction(nullptr) {}

	template<typename F>
	ParallelBatchJob(F&& f) : mFunction(std::forward<F>(f)) { }

	template<typename F>
	void SetJobFunction(F&& f) { mFunction = std::forward<F>(f); }

	void Execute(ThreadLocalArg* pThreadLocalArg);
private:
	std::function<void(ThreadLocalArg*, Entity*, int, ComponentTypes *...)>	mFunction;
};

template<typename...ComponentTypes>
class ParallelBatchJob<false, ComponentTypes...> : public ParallelJobBase<ComponentTypes...>
{
public:
	using base = ParallelJobBase<ComponentTypes...>;
	ParallelBatchJob() :mFunction(nullptr) {}

	template<typename F>
	ParallelBatchJob(F&& f) : mFunction(std::forward<F>(f)) { }

	template<typename F>
	void SetJobFunction(F&& f) { mFunction = std::forward<F>(f); }

	void Execute();
private:
	std::function<void(Entity*, int, ComponentTypes *...)>		mFunction;
};

/// DeferredJob is a wrapper of 'ForEach'
/// which allows to delay the execution of ForEach
template<bool IsLocalArgRequired, typename...T>
class DeferredJob;

template<typename LocalArg, typename...ComponentTypes>
class DeferredJob<true, LocalArg, ComponentTypes...>
{
public:
	DeferredJob() :mFunction(nullptr) {}

	template<typename F>
	DeferredJob(F&& f) : mFunction(std::forward<F>(f)) { }

	template<typename F>
	void SetJobFunction(F&& f) { mFunction = std::forward<F>(f); }
	void Execute(EntityContext* pEntityContext, LocalArg* pLocalArg);
	void Execute(World* pWorld, LocalArg* pLocalArg);
private:
	std::function<void(LocalArg*, Entity*, ComponentTypes *...)>	mFunction;
};

template<typename...ComponentTypes>
class DeferredJob<false, ComponentTypes...>
{
public:
	DeferredJob() :mFunction(nullptr) {}

	template<typename F>
	DeferredJob(F&& f) : mFunction(std::forward<F>(f)) { }

	template<typename F>
	void SetJobFunction(F&& f) { mFunction = std::forward<F>(f); }
	void Execute(EntityContext* pEntityContext);
	void Execute(World* pWorld);
private:
	std::function<void(Entity*, ComponentTypes *...)>		mFunction;
};

/// DeferredJob is a wrapper of 'ForEachBatch'
/// which allows to delay the execution of ForEachBatch, but only in one single thread.
template<bool IsLocalArgRequired, typename...T>
class DeferredBatchJob;

template<typename LocalArg, typename...ComponentTypes>
class DeferredBatchJob<true, LocalArg, ComponentTypes...>
{
public:
	DeferredBatchJob() :mFunction(nullptr) {}

	template<typename F>
	DeferredBatchJob(F&& f) : mFunction(std::forward<F>(f)) { }

	template<typename F>
	void SetJobFunction(F&& f) { mFunction = std::forward<F>(f); }
	void Execute(EntityContext* pEntityContext, LocalArg* pLocalArg);
	void Execute(World* pWorld, LocalArg* pLocalArg);
private:
	std::function<void(LocalArg*, Entity*, int, ComponentTypes *...)>	mFunction;
};

template<typename...ComponentTypes>
class DeferredBatchJob<false, ComponentTypes...>
{
public:
	DeferredBatchJob() :mFunction(nullptr) {}

	template<typename F>
	DeferredBatchJob(F&& f) : mFunction(std::forward<F>(f)) { }

	template<typename F>
	void SetJobFunction(F&& f) { mFunction = std::forward<F>(f); }
	void Execute(EntityContext* pEntityContext);
	void Execute(World* pWorld);
private:
	std::function<void(Entity*, int, ComponentTypes *...)>		mFunction;
};

void Entity::Release()
{
	FASTECS_ASSERT(mValid);
	this->mStorage->mContext->OnEntityDeleted(this);
	mStorage->Deallocate(this, true);
}

/// World must be singleton in the entire system.
/// you can create one by calling World::GetInstance();
/// or just call new operator but make sure it's a singleton in the system
class World
{
	friend class EntityContext;
public:
	static World* GetInstance()
	{
		static World _inst;
		return &_inst;
	}

	EntityContext* CreateContext()
	{
		auto id = FindAvaibableContextId();
		FASTECS_ASSERT(id != -1);
		auto pContext = new EntityContext(id, this, mArchetypeManager);
		mEntityContexts[id] = pContext;
		return pContext;
	}

	template<typename...ComponentTypes, typename F>
	void ForEach(F&& f)
	{
		for (int i = 0; i < MAX_CONTEXT_COUNT; i++)
		{
			EntityContext* pContext = mEntityContexts[i];
			if (pContext)
			{
				pContext->ForEach<ComponentTypes...>(std::forward<F>(f));
			}
		}
	}

	template<typename...ComponentTypes, typename F, typename RuntimeArg>
	void ForEach(F&& f, RuntimeArg* pArg)
	{
		for (int i = 0; i < MAX_CONTEXT_COUNT; i++)
		{
			EntityContext* pContext = mEntityContexts[i];
			if (pContext)
			{
				pContext->ForEach<ComponentTypes...>(std::forward<F>(f), pArg);
			}
		}
	}

	template<typename...ComponentTypes, typename F>
	void ForEachBatch(F&& f)
	{
		for (int i = 0; i < MAX_CONTEXT_COUNT; i++)
		{
			EntityContext* pContext = mEntityContexts[i];
			if (pContext)
			{
				pContext->ForEachBatch<ComponentTypes...>(std::forward<F>(f));
			}
		}
	}

	template<typename...ComponentTypes, typename F, typename RuntimeArg>
	void ForEachBatch(F&& f, RuntimeArg* pArg)
	{
		for (int i = 0; i < MAX_CONTEXT_COUNT; i++)
		{
			EntityContext* pContext = mEntityContexts[i];
			if (pContext)
			{
				pContext->ForEachBatch<ComponentTypes...>(std::forward<F>(f), pArg);
			}
		}
	}

	~World()
	{
		FASTECS_SAFE_DELETE(mArchetypeManager);
	}

	template<typename...ComponentTypes>
	EntityArchetype* CreateArchetype()
	{
		return mArchetypeManager->CreateArchetype<ComponentTypes...>();
	}

	EventManager* CreateEventManager() 
	{
		return new EventManager();
	}

	// Get current Memory Allocator
	IChunkMemoryAllocator* GetChunkMemoryAllocator() { return m_pChunkMemoryAllocator; }
	
	// Set an use-customed memory allocator,
	// if pass in a null, then use the default memory allocator
	void SetChunkMemoryAllocator(IChunkMemoryAllocator* pAllocator) 
	{
		if (pAllocator == nullptr)
			m_pChunkMemoryAllocator = &mStandardChunkMemoryAllocator;
		else 
			m_pChunkMemoryAllocator = pAllocator; 
	}

	World()
	{
		memset(mEntityContexts, 0, sizeof(mEntityContexts));
		mArchetypeManager = new EntityArchetypeManager();
		m_pChunkMemoryAllocator = &mStandardChunkMemoryAllocator;
	}

	// Return an entity by giving an EntityID
	Entity* GetEntity(EntityID eid)
	{
		// get context id 
		uint8_t contextId = Entity::ExtractContextIdFromEntityID(eid);
		if (mEntityContexts[contextId] == nullptr)
			return nullptr;
		return mEntityContexts[contextId]->GetEntity(eid);
	}

private:
	int FindAvaibableContextId() const
	{
		for (int i = 0; i < MAX_CONTEXT_COUNT; i++) {
			if (mEntityContexts[i] == nullptr) {
				return i;
			}
		}
		return -1;
	}

	void RemoveContext(EntityContext* pContext)
	{
		mEntityContexts[pContext->mContextId] = nullptr;
	}
	EntityArchetypeManager*			mArchetypeManager;
	//std::list<EntityContext*>		mEntityContextList;
	EntityContext*					mEntityContexts[MAX_CONTEXT_COUNT] = { 0 };
	//bool							mContextIdsUsed[MAX_CONTEXT_COUNT] = { false };
	IChunkMemoryAllocator*			m_pChunkMemoryAllocator;
	StandardChunkMemoryAllocator	mStandardChunkMemoryAllocator;
};

void EntityContext::Release()
{
	// Release EntityContext, including:
	// memory of all the storages
	// all the references in each related archetype
	// the reference in World
	for (auto pEntityComponentStorage : mEntityComponentStorageList) {
		auto pArchetype = pEntityComponentStorage->GetArchetype();
		pArchetype->mStoragesInContext[mContextId] = nullptr;
		FASTECS_SAFE_DELETE(pEntityComponentStorage);
	}
	mEntityComponentStorageList.clear();
	mWorld->RemoveContext(this);
	delete this;
}

template<typename ComponentType>
ComponentType* Entity::GetComponent()
{
	return mStorage->GetComponent<ComponentType>(this);
}

template<typename ComponentType>
const ComponentType* Entity::GetComponent() const
{
	return mStorage->GetComponent<ComponentType>(this);
}

template<typename ComponentType>
int Entity::GetComponentIndex() const
{
	return mStorage->GetArchetype()->GetComponentIndex<ComponentType>();
}

int Entity::GetComponentIndex(ComponentTypeID componentTypeID) const
{
	return mStorage->GetArchetype()->GetComponentIndex(componentTypeID);
}

template<typename T/*=byte*/>
T* Entity::GetComponentByIndex(int index)
{
	return mStorage->GetComponentByIndex<T>(this, index);
}

template<typename T/*=byte*/>
T* Entity::GetComponentByTypeID(ComponentTypeID typeId)
{
	return mStorage->GetComponentByTypeID<T>(this, typeId);
}

template<typename T/*=byte*/>
const T* Entity::GetComponentByIndex(int index) const
{
	return mStorage->GetComponentByIndex<T>(this, index);
}

template<typename T/*=byte*/>
const T* Entity::GetComponentByTypeID(ComponentTypeID typeId) const
{
	return mStorage->GetComponentByTypeID<T>(this, typeId);
}

template<typename ComponentType>
bool Entity::ContainComponent() const
{
	return mStorage->GetArchetype()->ContainComponent<ComponentType>();
}

template<typename... ComponentTypes>
bool Entity::ContainAllComponents() const
{
	return mStorage->GetArchetype()->ContainAllComponents<ComponentTypes...>();
}

template<typename... ComponentTypes>
bool Entity::ContainAnyComponents() const
{
	return mStorage->GetArchetype()->ContainAnyComponents<ComponentTypes...>();
}

// Get EntityID of this entity.
// EntityID is an unique id representing this entity in the system.
// You can get entity by EntityID throught calling GetEntity() method
EntityID Entity::GetEntityID() const
{
	// an entity id contains:
	// |<-----16:GenID----->||<--8:contextID-->||<----X:storageId---->||<---Z:chunkId--->||<---Z:block--->|
	// Y is MAX_CHUNK_COUNT_BITS
	// Z is MAX_BLOCK_COUNT_BITS
	// X is (40 - X - Y)

	return ((uint64_t)mGenID << 48)
		| ((uint64_t)mStorage->mContext->GetContextId() << 40)
		| ((uint64_t)mStorage->GetIndex() << ((int)MAX_CHUNK_COUNT_BITS + (int)MAX_BLOCK_COUNT_BITS))
		| ((uint64_t)mChunkIndex << MAX_BLOCK_COUNT_BITS)
		| (uint64_t)mBlockIndex;
}

EntityContext* Entity::GetContext()
{
	return mStorage->mContext;
}

void Entity::ParseEntityID(EntityID eid, uint16_t* genid,
	uint8_t* contextId,
	uint16_t* storageIndex,
	uint16_t* chunkIndex, uint16_t* blockIndex)
{
	// an entity id contains:
	// |<-----16:GenID----->||<--8:contextID-->||<----X:storageId---->||<---Z:chunkId--->||<---Z:block--->|
	// Y is MAX_CHUNK_COUNT_BITS
	// Z is MAX_BLOCK_COUNT_BITS
	// X is (40 - X - Y)

	*genid = (uint16_t)((eid >> 48) & 0x0FFFF);
	*contextId = (uint8_t)((eid >> 40) & 0x0FF);
	*storageIndex = (uint16_t)((eid >> ((int)MAX_CHUNK_COUNT_BITS + (int)MAX_BLOCK_COUNT_BITS)) & STORAGE_INDEX_MASK);
	*chunkIndex = (uint16_t)((eid >> MAX_BLOCK_COUNT_BITS) & CHUNK_INDEX_MASK);
	*blockIndex = (uint16_t)(eid & BLOCK_INDEX_MASK);
}

uint8_t Entity::ExtractContextIdFromEntityID(EntityID eid)
{
	return (uint8_t)((eid >> 40) & 0x0FF);
}

EntityArchetype* Entity::GetArchetype() 
{
	return mStorage->GetArchetype();
}

const EntityArchetype* Entity::GetArchetype() const
{
	return mStorage->GetArchetype();
}

int Entity::GetComponentCount() const
{
	return mStorage->mComponentCountPerEntity;
}

// extend current entity by a list of component types.
// the components are given in the function parameters
// return a new entity and retain the current one at the same time
template<typename...ComponentTypes>
Entity* Entity::Extend(ComponentTypes&&... args) const
{
	return mStorage->mContext->ExtendEntity<ComponentTypes...>(this, std::forward<ComponentTypes>(args)...);
}

// extend current entity by a list of component types.
// return a new entity and retain the current one at the same time
template<typename...ComponentTypes>
Entity* Entity::Extend() const
{
	return mStorage->mContext->ExtendEntity<ComponentTypes...>(this);
}

template<typename ComponentType>
bool Entity::SetComponent(ComponentType&& data)
{
	using T = std::decay_t<ComponentType>;
	T* pComponent = GetComponent<T>();
	if (!pComponent) {
		return false;
	}
	*pComponent = std::forward<ComponentType>(data);
	return true;
}

// clone current entity, 
// return a new entity
Entity* Entity::Clone() const
{
	return mStorage->CloneEntity(this);
}

// remove component types from current entity.
// return a new entity and retain the current one at the same time
template<typename...ComponentTypes>
Entity* Entity::Remove() const
{
	return mStorage->mContext->RemoveComponentsFromEntity<ComponentTypes...>(this);
}

IChunkMemoryAllocator* EntityComponentStorage::GetChunkMemoryAllocator()
{
	return mContext->GetWorld()->GetChunkMemoryAllocator();
}


IChunkMemoryAllocator* EntityComponentChunk::GetMemoryAllocator()
{
	return mEntityComponentStorage->GetChunkMemoryAllocator();
}


template<typename ThreadLocalArg, typename...ComponentTypes>
void ParallelJob<true, ThreadLocalArg, ComponentTypes...>::Execute(ThreadLocalArg* pThreadLocalArg)
{
	int threadIndex = base::mStartCounter.fetch_add(1);
	FASTECS_ASSERT(threadIndex < base::mThreadCount);
	base::mState = ParallelJobState::Executing;

	ParallelJobChunkSegementList& chunkList = base::mDividedJobChunkSegmentArray[threadIndex];
	for (auto& it : chunkList) {
		EntityComponentChunk* pChunk = it.pChunk;
		pChunk->ForEach<ComponentTypes...>(mFunction, pThreadLocalArg, it.RangeStart, it.RangeEnd);
	}
	if (base::mCompleteCounter.fetch_add(1) == base::mThreadCount - 1) {
		base::mState = ParallelJobState::Unprepared;
	}
}

template<typename...ComponentTypes>
void ParallelJob<false, ComponentTypes...>::Execute()
{
	int threadIndex = base::mStartCounter.fetch_add(1);
	FASTECS_ASSERT(threadIndex < base::mThreadCount);
	base::mState = ParallelJobState::Executing;

	ParallelJobChunkSegementList& chunkList = base::mDividedJobChunkSegmentArray[threadIndex];
	for (auto& it : chunkList) {
		EntityComponentChunk* pChunk = it.pChunk;
		pChunk->ForEach<ComponentTypes...>(mFunction, it.RangeStart, it.RangeEnd);
	}
	if (base::mCompleteCounter.fetch_add(1) == base::mThreadCount - 1) {
		base::mState = ParallelJobState::Unprepared;
	}
}

template<typename ThreadLocalArg, typename...ComponentTypes>
void ParallelBatchJob<true, ThreadLocalArg, ComponentTypes...>::Execute(ThreadLocalArg* pThreadLocalArg)
{
	int threadIndex = base::mStartCounter.fetch_add(1);
	FASTECS_ASSERT(threadIndex < base::mThreadCount);
	base::mState = ParallelJobState::Executing;

	ParallelJobChunkSegementList& chunkList = base::mDividedJobChunkSegmentArray[threadIndex];
	for (auto& it : chunkList) {
		EntityComponentChunk* pChunk = it.pChunk;
		pChunk->ForEachBatch<ComponentTypes...>(mFunction, pThreadLocalArg, it.RangeStart, it.RangeEnd);
	}
	if (base::mCompleteCounter.fetch_add(1) == base::mThreadCount - 1) {
		base::mState = ParallelJobState::Unprepared;
	}
}

template<typename...ComponentTypes>
void ParallelBatchJob<false, ComponentTypes...>::Execute()
{
	int threadIndex = base::mStartCounter.fetch_add(1);
	FASTECS_ASSERT(threadIndex < base::mThreadCount);
	base::mState = ParallelJobState::Executing;

	ParallelJobChunkSegementList& chunkList = base::mDividedJobChunkSegmentArray[threadIndex];
	for (auto& it : chunkList) {
		EntityComponentChunk* pChunk = it.pChunk;
		pChunk->ForEachBatch<ComponentTypes...>(mFunction, it.RangeStart, it.RangeEnd);
	}
	if (base::mCompleteCounter.fetch_add(1) == base::mThreadCount - 1) {
		base::mState = ParallelJobState::Unprepared;
	}
}

template<typename LocalArg, typename...ComponentTypes>
void DeferredJob<true, LocalArg, ComponentTypes...>::Execute(EntityContext* pEntityContext, LocalArg* pArg)
{
	pEntityContext->ForEach<ComponentTypes...>(mFunction, pArg);
}

template<typename LocalArg, typename...ComponentTypes>
void DeferredJob<true, LocalArg, ComponentTypes...>::Execute(World* pWorld, LocalArg* pArg)
{
	pWorld->ForEach<ComponentTypes...>(mFunction, pArg);
}

template<typename...ComponentTypes>
void DeferredJob<false, ComponentTypes...>::Execute(World* pWorld)
{
	pWorld->ForEach<ComponentTypes...>(mFunction);
}

template<typename...ComponentTypes>
void DeferredJob<false, ComponentTypes...>::Execute(EntityContext* pEntityContext)
{
	pEntityContext->ForEach<ComponentTypes...>(mFunction);
}

template<typename...ComponentTypes>
void DeferredBatchJob<false, ComponentTypes...>::Execute(EntityContext* pEntityContext)
{
	pEntityContext->ForEachBatch<ComponentTypes...>(mFunction);
}

template<typename...ComponentTypes>
void DeferredBatchJob<false, ComponentTypes...>::Execute(World* pWorld)
{
	pWorld->ForEachBatch<ComponentTypes...>(mFunction);
}

template<typename LocalArg, typename...ComponentTypes>
void DeferredBatchJob<true, LocalArg, ComponentTypes...>::Execute(World* pWorld, LocalArg* Arg)
{
	pWorld->ForEachBatch<ComponentTypes...>(mFunction, Arg);
}

template<typename LocalArg, typename...ComponentTypes>
void DeferredBatchJob<true, LocalArg, ComponentTypes...>::Execute(EntityContext* pEntityContext, LocalArg* Arg)
{
	pEntityContext->ForEachBatch<ComponentTypes...>(mFunction, Arg);
}

} // end namespace

#endif
