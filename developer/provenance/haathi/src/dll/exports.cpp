/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

//#include "com/haathi/extension/ExtensionConfig.hpp"
#include "com/haathi/menu/ExtensionMenu.hpp"
#include "com/haathi/menu/MenuConfig.hpp"

#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>

#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>

using namespace com::sun::star::registry;
using namespace com::sun::star::uno;

using namespace rtl;

namespace framework {
	namespace ExtensionMenu {
		static ::cppu::ImplementationEntry const s_impl_entries[] = {
			{
				::com::haathi::menu::ExtensionMenu_createInstance,
				::com::haathi::menu::ExtensionMenu_getImplementationName,
				::com::haathi::menu::ExtensionMenu_getSupportedServiceNames,
				::cppu::createSingleComponentFactory,
				0,
				0
			},
			{ 0, 0, 0, 0, 0, 0 }
		};
	}
}

 static void writeInfo(const Reference<XRegistryKey>& xRegistryKey,
		OUString pImplementationName, OUString pServiceName) {
	OUStringBuffer sKey(256);

 	sKey.append(pImplementationName);
	sKey.appendAscii("/UNO/SERVICES/");
	sKey.append(pServiceName);
	xRegistryKey->createKey(sKey.makeStringAndClear());
}

extern "C"
{
	SAL_DLLPUBLIC_EXPORT sal_Bool SAL_CALL component_writeInfo(
			void* pServiceManager, void* pRegistryKey) {
		 if (!pRegistryKey)
			 return sal_False;
		 try {
			 Reference<XRegistryKey> xKey(reinterpret_cast<XRegistryKey*>(pRegistryKey), UNO_QUERY);

			 writeInfo(xKey, OUSTRING(PROVENANCE_MENU_IMPLEMENTATIONNAME), OUSTRING(PROVENANCE_MENU_SERVICENAME));
			 return sal_True;
		 }
		 catch(const InvalidRegistryException&) {
			OSL_ENSURE(sal_False, "### InvalidRegistryException!");
		 }
		 return sal_False;
	}

	SAL_DLLPUBLIC_EXPORT void SAL_CALL component_getImplementationEnvironment(
			const sal_Char** ppEnvTypeName, uno_Environment** ppEnv) {
		*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
	}

	SAL_DLLPUBLIC_EXPORT void* SAL_CALL component_getFactory(
		const sal_Char* pImplName, void* pServiceManager, void* pRegistryKey) {
		return ::cppu::component_getFactoryHelper(pImplName, pServiceManager,
				pRegistryKey, framework::ExtensionMenu::s_impl_entries);
	}
}