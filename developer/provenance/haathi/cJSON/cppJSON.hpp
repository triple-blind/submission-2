/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/cJSON/cppJSON.hpp#1 $

// This is a very lightweight wrapper around cJSON that helps ensure that
// there aren't any memory leaks using RAII.
/******************************************************************************
Guard
******************************************************************************/
#ifndef cppJSON_HPP
#	define cppJSON_HPP
/******************************************************************************
Include
******************************************************************************/
// At least one of the targeted platforms was not happy with cJSON.h for lack
// of a size_t definition, so the appropriate include was added here.
#include <stddef.h> // size_t

#include "../../libcpp/cJSON/cJSON.h"

#include <string>

#if defined(_MSC_VER)
#	pragma warning(push)
//2>c:\Users\Keith Alcock\Documents\My Data\Projects\provenance\haathi\sdk\Windows\include\com/sun/star/uno/Reference.h(72) : warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#	pragma warning(disable: 4290)
#endif
/******************************************************************************
Class
******************************************************************************/
template<typename T, void (*F)(T* value)>
class Destructor {
	protected:
		T* value;
	public:
		inline Destructor(T* value): value(value) {
		}

		inline ~Destructor() {
			if (value)
				F(value);
		}
};

void cppJSON_FreeString(char* string);

class json_free_ptr: public Destructor<char, cppJSON_FreeString> {
	public:
		inline json_free_ptr(char* value): Destructor<char, cppJSON_FreeString>(value) {
		}
};

class cppJSON: public Destructor<cJSON, cJSON_Delete> {
	protected:
		// Not implemented
		cppJSON(const cppJSON& other);
		// Not implemented
		cppJSON& operator=(const cppJSON& other);

	public:
		static int getMallocCount();
		static int getFreeCount();
		static void freeString(char* string);
		static std::string print(cJSON* item);

		cppJSON(cJSON* value) throw (std::bad_alloc);
		std::string print();
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
