/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/utils/IntrospectionUtils.cpp#1 $

//
// Author: Aaron Gibson
// Date: September 21, 2012
//
// This file implements the functions of IntrospectionUtils.hpp.
// 
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/IntrospectionUtils.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp" // OUSTRING

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyConcept.hpp>
#include <com/sun/star/beans/XIntrospection.hpp>
#include <com/sun/star/beans/XIntrospectionAccess.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include "sdkEnd.hpp"

#include <iostream> // std::cout
/******************************************************************************
Using
******************************************************************************/
using namespace com::sun::star::beans;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

using namespace rtl;
/******************************************************************************
Class
******************************************************************************/
static OUString introspectionServiceStr =
		OUSTRING("com.sun.star.beans.Introspection");

void printTypesToConsole(Reference<XInterface> object) {
	Reference<XTypeProvider> xtype(object, UNO_QUERY);
	if (!xtype.is()) {
		std::cout << "This object is not an instance of XTypeProvider!" << std::endl;
		std::cout << "Cannot query for types." << std::endl;
		return;
	}
	Sequence<Type> types = xtype->getTypes();
	std::cout << "Types:" << std::endl;
	for (sal_Int32 i = 0; i < types.getLength(); ++i) {
		std::cout << types[i].getTypeName() << std::endl;
	}
	std::cout << std::endl;
}

void printPropertyToConsole(Property prop) {
	std::cout << "Property: " << prop.Name << "\tType: "
			<< prop.Type.getTypeName();
}

void printPropertiesToConsole(Reference<XInterface> object) {
	Reference<XPropertySetInfo> xprop;
	Reference<XPropertySet> xset(object, UNO_QUERY);
	if (!xset.is()) {
		xprop = Reference<XPropertySetInfo>(object, UNO_QUERY);
	} else {
		xprop = xset->getPropertySetInfo();
	}

	if (!xprop.is()) {
		std::cout << "This object is not an instance of XPropertySetInfo!" << std::endl;
		std::cout << "Cannot print properties." << std::endl;
		return;
	}
	Sequence<Property> props = xprop->getProperties();
	for (sal_Int32 i = 0; i < props.getLength(); ++i) {
		printPropertyToConsole(props[i]);
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void inspectInterface(Reference<XInterface> interface, Reference<XComponentContext> xContext) {
	Any val;
	val <<= interface;
	inspectAny(val, xContext);
}

void inspectAny(Any object, Reference<XComponentContext> xContext) {
	Reference<XMultiComponentFactory> localMCF =
			xContext->getServiceManager();
	Reference<XInterface> introspect = localMCF->createInstanceWithContext(
			introspectionServiceStr, xContext);
	Reference<XIntrospection> xintrospect(introspect, UNO_QUERY);

	if (!xintrospect.is()) {
		std::cout << "Error: Could not create Introspection service!"
				<< std::endl;
		return;
	}

	Reference<XIntrospectionAccess> xaccess = xintrospect->inspect(object);
	Sequence<Property> props = xaccess->getProperties(PropertyConcept::ALL);
	std::cout << "Properties:" << std::endl;
	for (sal_Int32 i = 0; i < props.getLength(); ++i) {
		printPropertyToConsole(props[i]);
		std::cout << std::endl;
	}
	Sequence<Type> listeners = xaccess->getSupportedListeners();
	std::cout << std::endl << "SupportedListeners:" << std::endl;
	for (sal_Int32 i = 0; i < listeners.getLength(); ++i) {
		std::cout << listeners[i].getTypeName() << std::endl;
	}
}
/*****************************************************************************/
