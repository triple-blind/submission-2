/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/text/ReplaceTextProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__TEXT__REPLACE_TEXT_PROVENACE_H
#	define COM__HAATHI__PROVENANCE__TEXT__REPLACE_TEXT_PROVENACE_H
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
ReplaceTextProvenance
******************************************************************************/
class ReplaceTextProvenance: public ProvenanceTextEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void save(Archive& ar, const unsigned int version) const {
		std::string beforeString = com::haathi::utils::convertToString(before);
		std::string afterString = com::haathi::utils::convertToString(after);
		
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
		ar & HAATHI_SERIALIZATION_NVP("pos", offset);
		ar & HAATHI_SERIALIZATION_NVP("old", beforeString);
		ar & HAATHI_SERIALIZATION_NVP("new", afterString);
	}

	template<typename Archive>
	void load(Archive& ar, const unsigned int version) {
		std::string beforeString;
		std::string afterString;
		
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
		ar & HAATHI_SERIALIZATION_NVP("pos", offset);
		ar & HAATHI_SERIALIZATION_NVP("old", beforeString);
		ar & HAATHI_SERIALIZATION_NVP("new", afterString);
		before = com::haathi::utils::convertToOUString(beforeString);
		after = com::haathi::utils::convertToOUString(afterString);
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()

	private:
		ReplaceTextProvenance();
		int offset;
		OUString before;
		OUString after;

	public:
		ReplaceTextProvenance(int offset, OUString before, OUString after);
		//virtual ~ReplaceTextProvenance();
		ReplaceTextProvenance& operator=(const ReplaceTextProvenance& other);
		ReplaceTextProvenance(const ReplaceTextProvenance& other);

		virtual std::string getEventType() const;
		virtual rtl::OUString printToTable() const;
		virtual rtl::OUString getEventDescription() const;

		virtual bool redo(Reference<com::sun::star::lang::XComponent> xComponent, com::sun::star::util::Color color) const;
		virtual bool undo(Reference<com::sun::star::lang::XComponent> xComponent, com::sun::star::util::Color color) const;
		virtual OUString apply(OUString input);
		virtual void track(com::haathi::document::IDocumentStateTracker& documentStateTracker);
		virtual com::haathi::text::Range getWatermarkRange();
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
