/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/ListenerTypedefs.hpp#1 $

/*
 *  Created by Aaron Gibson on 9/5/11.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__LISTENER_TYPEDEFS_HPP
#	define COM__HAATHI__DIALOG__LISTENER_TYPEDEFS_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/awt/XActionListener.hpp>
#include <com/sun/star/awt/XItemListener.hpp>
#include <com/sun/star/awt/XAdjustmentListener.hpp>
#include <com/sun/star/awt/XTextListener.hpp>
#include <com/sun/star/awt/XTopWindowListener.hpp>
#include <com/sun/star/util/XCloseListener.hpp>
#include <cppuhelper/implbase1.hxx>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
Content
******************************************************************************/
/*!
 These typedefs are for convenience. Each Listener that is called to listen on some
 UNO component must itself be an UNO component. This means that there are certain 
 functions that must be implemented. The cppu::WeakImplHelper classes implement the
 details of these functions with no additional work. The typedef makes this easier
 to read.
 */
typedef ::cppu::WeakImplHelper1<com::sun::star::awt::XActionListener> XActionListenerHelper;
typedef ::cppu::WeakImplHelper1<com::sun::star::awt::XAdjustmentListener> XAdjustmentListenerHelper;
typedef ::cppu::WeakImplHelper1<com::sun::star::awt::XItemListener> XItemListenerHelper;
typedef ::cppu::WeakImplHelper1<com::sun::star::awt::XTextListener> XTextListenerHelper;
typedef ::cppu::WeakImplHelper1<com::sun::star::awt::XTopWindowListener> XTopWindowListenerHelper;
typedef ::cppu::WeakImplHelper1<com::sun::star::util::XCloseListener> XCloseListenerHelper;
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
