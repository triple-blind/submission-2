#ifndef UNO_WINDOW_HPP
#define UNO_WINDOW_HPP

// System includes
// In com::sun::star::awt
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
// In com::sun::star::frame
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XFramesSupplier.hpp>
// In com::sun::star::lang
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
// In com::sun::star::util
#include <com/sun/star/util/Color.hpp>

// Namespaces
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;

namespace com {
namespace haathi {
namespace dialog {

class UnoWindow {
private:
	// I don't know what I want to do with these yet.
	// I'll keep them private in the meantime.
	UnoWindow(const UnoWindow & other);
	UnoWindow& operator=(const UnoWindow& other);

	// Helper Function for initialization
	void initializeUnoWindow(Reference<XMultiServiceFactory> remoteMSF,
			Reference<XFramesSupplier> frameSupplier,
			WindowDescriptor descriptor, OUString name, Color backgroundColor);

protected:
	// Instance variables that are required to create a window.
	Reference<XMultiServiceFactory> xMSF;

	Reference<XToolkit> xToolkit;
	Reference<XWindow> xWindow;
	Reference<XWindowPeer> xWindowPeer;
	Reference<XFrame> xFrame;

public:

	UnoWindow(Reference<XMultiServiceFactory> remoteMSF, sal_Int32 attributes,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, Color color);

	UnoWindow(Reference<XMultiServiceFactory> remoteMSF, sal_Int32 attributes,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name);

	UnoWindow(Reference<XMultiServiceFactory> xMSF);
	UnoWindow(Reference<XMultiServiceFactory> xMSF,
			WindowDescriptor descriptor);
	virtual ~UnoWindow();

	// Initialization Functions
	void setWindowProperties(sal_Int32 posx, sal_Int32 posy, sal_Int32 height,
			sal_Int32 width);
	// Insertion-of-Component functions.
	// Static Functions
	static UnoWindow* createUnoWindow(
			Reference<XMultiServiceFactory> remoteMSF);
};

}
;
}
;
}
;
// namespace com::haathi::dialog

#endif
