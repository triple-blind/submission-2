/*!
 @file GraphicProvenanceEvent.hpp
 
 @author Aaron Gibson
 @date September 11, 2012
 @brief -- This header declares classes to handle image-based provenance events.
 
 */

#include "com/haathi/utils/Serialization.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"

// Note that the "ProvenanceEvent" header takes care of A LOT of other
// headers that we'd otherwise have to include.

namespace com {
namespace haathi {
namespace provenance {

class AddImageProvenanceEvent: public ProvenanceEvent {
private:
	// For serialization...
	friend class boost::serialization::access;

	template<typename Archive ar>
	void serialize(Archive & ar, const unsigned int version) {
	}

public:
	AddImageProvenanceEvent();
	~AddImageProvenanceEvent();

	// Pure virtual methods that must be implemented for ProvenanceEvent.
	virtual bool handleEvent(Reference<XComponent> document, bool load,
			Sequence<PropertyValue> props = Sequence<PropertyValue>()) const;
	virtual OUString toString() const;
	virtual OUString printToTable() const;

};
// class AddImageProvenanceEvent

}
;
}
;
}
;
// namespace com::haathi::provenance

// Another 
