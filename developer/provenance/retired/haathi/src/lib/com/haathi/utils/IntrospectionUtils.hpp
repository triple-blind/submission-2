/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/utils/IntrospectionUtils.hpp#1 $

/*!
 * @header IntrospectionUtils.hpp
 * 
 * @author Aaron Gibson
 * @date September 19, 2012
 * @brief This file outlines some basic functions for inspecting UNO objects at runtime.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__INTROSPECTION_UTILS_HPP
#	define COM__HAATHI__UTILS__INTROSPECTION_UTILS_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/uno/XInterface.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
// Clients expect this to be defined
namespace com {
	namespace haathi {
		namespace utils {
		};
	};
};
/******************************************************************************
Class
******************************************************************************/
void printTypesToConsole(Reference<com::sun::star::uno::XInterface> object);
void printPropertyToConsole(com::sun::star::beans::Property prop);
void printPropertiesToConsole(Reference<com::sun::star::uno::XInterface> object);
void inspectInterface(Reference<com::sun::star::uno::XInterface> object, Reference<com::sun::star::uno::XComponentContext> xContext);
void inspectAny(com::sun::star::uno::Any val, Reference<com::sun::star::uno::XComponentContext> xContext);
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
