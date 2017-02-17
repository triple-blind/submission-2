/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/NamesValuesHelper.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/NamesValuesHelper.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::sun::star::uno::Any;
using com::sun::star::uno::Sequence;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
NamesValuesHelper::NamesValuesHelper():
		size(0), names(Sequence<OUString>(0)), values(Sequence<Any>(0)) {
	pos = 0;
}

NamesValuesHelper::NamesValuesHelper(int size):
		size(size), names(Sequence<OUString>(size)), values(Sequence<Any>(size)) {
	pos = 0;
}

NamesValuesHelper::~NamesValuesHelper() {
}

NamesValuesHelper& NamesValuesHelper::add(const OUString name, Any value)
		throw (HaathiException) {
	if (pos < size) {
		names[pos] = name;
		values[pos] = value;
		pos++;
	}
	else
		throw HaathiException("Capacity exceeded!");
	return *this;
}

NamesValuesHelper& NamesValuesHelper::add(const char* name, Any value)
		throw (com::haathi::utils::HaathiException) {
	return add(OUString::createFromAscii(name), value);
}

NamesValuesHelper& NamesValuesHelper::add(NamesValuesHelper& other)
		throw (com::haathi::utils::HaathiException) {
	for (int i = 0; i < other.size; i++)
		add(other.names[i], other.values[i]);
	return *this;
}

Sequence<OUString>& NamesValuesHelper::getNames() {
	return names;
}

Sequence<Any>& NamesValuesHelper::getValues() {
	return values;
}

int NamesValuesHelper::getSize() {
	return size;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
