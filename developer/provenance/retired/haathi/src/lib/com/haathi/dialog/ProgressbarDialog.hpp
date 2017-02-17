/*
 *  Created by Aaron Gibson on 4/5/12.
 */

#ifndef ProgressbarDialog_H
#define ProgressbarDialog_H

// UNO includes
#include <osl/mutex.hxx>

// My includes
#include "com/haathi/dialog/BaseDialog.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

namespace com {
namespace haathi {
namespace dialog {

class ProgressbarDialog: public BaseDialog {
private:
	// These are not implemented.
	ProgressbarDialog& operator=(const ProgressbarDialog& other);
	ProgressbarDialog(const ProgressbarDialog& other);
protected:
	/*!
	 This is the lock to acquire to perform an action on this dialog.
	 We need this lock because it is possible to update the dialog while
	 it is still loading the previous update, which causes strange 
	 behavior. (Even if it doesn't, it can't hurt.)
	 */
	osl::Mutex lock;

	/*!
	 This stores the progressbar. Accessing this allows to change the bar 
	 to the desired percentage.
	 */
	Reference<XProgressBar> xProgressBar;

	/*!
	 This stores the text that is displayed below the progressbar. The text
	 should indicate what is going on with the progressbar.
	 */
	Reference<XPropertySet> text_field;

	/*!
	 This stores the current amount of progress in the dialog.
	 */
	sal_Int32 progress;

public:
	/*!
	 This constructor will create a basic ProgressbarDialog from the specified XMultiServiceFactory.
	 Two additional parameters are specified to identify the ends of the progress bar. If they are not
	 explicitly specified, then the range of the progressbar is assumed to be on a percent scale:
	 I.e., from 0-100.
	 
	 @param xMSF -- the service factory to create the dialog
	 @param begin -- (optional) the beginning value for the progress bar.
	 @param end -- (optional) the ending value for the progress bar.
	 */
	ProgressbarDialog(Reference<XComponentContext> xContext, sal_Int32 begin = 0,
			sal_Int32 end = 100);
	/*!
	 This is the standard destructor. If the dialog is still running, this will force-quit the thread.
	 */
	virtual ~ProgressbarDialog();

	/*!
	 This function will set the progressbar to the specified value.
	 */
	void setProgress(sal_Int32 new_progress);

	/*!
	 This function will return the current status of the progressbar.
	 
	 @result -- The current status of the progressbar.
	 */
	sal_Int32 getProgress();

	/*!
	 This function will set the text displayed (below the bar) to the specified string.
	 @param str -- the string to display below the bar.
	 */
	void setText(OUString str);

	/*!
	 This function will return the current string displayed (below the bar).
	 
	 @result -- the string currently displayed below the dialog.
	 */
	OUString getText();

	/*!
	 This function will return whether the progressbar is completely full, i.e. done.
	 
	 @result -- true if the progressbar is complete, false otherwise.
	 */
	bool isDone() const;

	/*!
	 This function will create a new ProgressbarDialog with the given service factory.
	 This function will also run the progressbar in a separate thread. This allows the 
	 dialog to update as some action is being performed.
	 
	 @param _xMSF -- the service factory to create the ProgressbarDialog with.
	 @result -- a pointer to the created ProgressbarDialog.
	 */
	static ProgressbarDialog* createProgressbarDialog(
			Reference<XComponentContext> xContext);
};

}
;
}
;
}
;
// namespace com::haathi::dialog

#endif
