/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/text/CutTextProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__TEXT__CUT_TEXT_PROVENANCE_H
#	define COM__HAATHI__PROVENANCE__TEXT__CUT_TEXT_PROVENANCE_H
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
CutTextProvenance
******************************************************************************/
class CutTextProvenance: public ProvenanceTextEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void save(Archive& ar, const unsigned int version) const {
		std::string dataString = com::haathi::utils::convertToString(data);
		
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
		ar & HAATHI_SERIALIZATION_NVP("pos", offset);
		ar & HAATHI_SERIALIZATION_NVP("txt", dataString);
	}

	template<typename Archive>
	void load(Archive& ar, const unsigned int version) {
		std::string dataString;
		
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
		ar & HAATHI_SERIALIZATION_NVP("pos", offset);
		ar & HAATHI_SERIALIZATION_NVP("txt", dataString);
		data = com::haathi::utils::convertToOUString(dataString);
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()

	protected:
		CutTextProvenance();
		int offset;
		OUString data;

	public:
		CutTextProvenance(int offset, OUString data);
		//virtual ~CutTextProvenance();
		CutTextProvenance& operator=(const CutTextProvenance& other);
		//CutTextProvenance(const CutTextProvenance& other);

		virtual std::string getEventType() const;
		virtual rtl::OUString printToTable() const;
		virtual rtl::OUString getEventDescription() const;

		virtual bool redo(Reference<com::sun::star::lang::XComponent> xComponent, com::sun::star::util::Color color) const;
		virtual bool undo(Reference<com::sun::star::lang::XComponent> xComponent, com::sun::star::util::Color color) const;
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
