// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/ProgressbarDialog.cpp#1 $

/*
 *  Created by Aaron Gibson on 4/5/12.
 */

#include "com/haathi/dialog/ProgressbarDialog.hpp"

// UNO includes
#include <osl/thread.hxx>

namespace com {
namespace haathi {
namespace dialog {

namespace {
class ProgressbarThread: public osl::Thread {
private:
	ProgressbarDialog* dialog;

protected:
	virtual void SAL_CALL run() {
		dialog->run();
	}

public:
	ProgressbarThread(ProgressbarDialog* _dialog) :
			dialog(_dialog) {
	}
	virtual ~ProgressbarThread() {
	}
};
// class ProgressbarThread
}
;
// namespace -- hidden

ProgressbarDialog::ProgressbarDialog(Reference<XComponentContext> xContext,
		sal_Int32 begin, sal_Int32 end) :
		BaseDialog(xContext), progress(0) {
	setWindowProperties(50, 50, 50, 100, OUSTRING("Progress"));
	// Create the progressbar.
	xProgressBar = insertProgressbarOnPage(0, 10, 10, 12, 80,
			OUSTRING("Progressbar"), 0, 100);

	// Create the text_field.
	text_field = insertFixedTextOnPage((sal_Int16) 0, 20, 30, 80,
			OUSTRING("TextField"), OUSTRING("Processing"),
			HORIZONTAL_ALIGN_CENTER);
}

ProgressbarDialog::~ProgressbarDialog() {
	// Nothing to do? Make sure the thread is destroyed.
}

void ProgressbarDialog::setProgress(sal_Int32 new_progress) {
//		std::cout << "Setting progress" << std::endl;
//		osl::MutexGuard guard(lock);
	xProgressBar->setValue(new_progress);
	repaint();
}

sal_Int32 ProgressbarDialog::getProgress() {
//		osl::MutexGuard guard(lock);
	return xProgressBar->getValue();
}

void ProgressbarDialog::setText(OUString new_str) {
//		osl::MutexGuard guard(lock);
//		std::cout << "Setting text" << std::endl;
	Any val;
	val <<= new_str;

	text_field->setPropertyValue(OUSTRING("Label"), val);
	repaint();
}

OUString ProgressbarDialog::getText() {
//		osl::MutexGuard guard(lock);
	return OUSTRING("Processing");
}

bool ProgressbarDialog::isDone() const {
	return true;
}

ProgressbarDialog* ProgressbarDialog::createProgressbarDialog(
		Reference<XComponentContext> xContext) {
	// First, create the dialog. Then, pass this instance of the dialog to the separate thread.
	// This allows for the progressbar to update based on the results of another process.
	ProgressbarDialog* dialog = new ProgressbarDialog(xContext);
	ProgressbarThread* thread = new ProgressbarThread(dialog);
	thread->create();
	// Make sure that the dialog is actively running before returning...?
	osl::Thread::yield();

	return dialog;
}

}
;
}
;
}
;
// namespace com::haathi::dialog

