/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/graphic/WriterGraphicProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__GRAPHIC__WRITER_GRAPHIC_PROVENANCE_H
#	define COM__HAATHI__PROVENANCE__GRAPHIC__WRITER_GRAPHIC_PROVENANCE_H
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/provenance/ProvenanceEvent.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
WriterGraphicProvenance
******************************************************************************/
class WriterGraphicProvenance: public ProvenanceEvent {
	private:
		std::string filename;
		int location; // Change? More detail may be needed.
		bool add;
	public:
		// Constructors and Destructors
		WriterGraphicProvenance();
		virtual ~WriterGraphicProvenance();
		WriterGraphicProvenance& operator=(const WriterGraphicProvenance& other);
		WriterGraphicProvenance(const WriterGraphicProvenance& other);

		// Virtual Functions that must be implemented from ProvenanceEvent.
		virtual std::string getDetails();
		virtual bool redo(Reference< com::sun::star::lang::XComponent > text_doc, bool load);
};
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
