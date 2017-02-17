/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/ProvenanceEvent.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/10/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__PROVENANCE_EVENT_HPP
#	define COM__HAATHI__PROVENANCE__PROVENANCE_EVENT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/provenance/ProvenanceTime.hpp"
#include "com/haathi/text/Range.hpp"
#include "com/haathi/utils/Serialization.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/util/Color.hpp>
#include "sdkEnd.hpp"

#include <string>
#include <vector>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
			class IDocumentStateTracker;
		};
	};
};

namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
ProvenanceEvent
******************************************************************************/
/*
Events are displayed in at least four different formats:
   -In a list box that requires a single line of text provided by printToTable()
   -In a label displaying only the type of event provided by getEventType()
   -In a text component, possibly multi-lined, that shows details provided by getEventDescription()
   -To stdout, stderr, etc. for debugging as provided by toString()
*/

class ProvenanceEvent {
	/**
	 Declaring this as a friend class allows for us to serialize using
	 Boost's internal libraries.
	 
	 Similarly, for this class and any subclass of ProvenanceEvent, the
	 function "serialize" as declared below must be implemented. Note 
	 that implementing this function is generic and easy in most cases;
	 just follow the template below.
	 */
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const {
		std::string ts(time.save());

		ar & BOOST_SERIALIZATION_NVP(ts);
		ar & HAATHI_SERIALIZATION_NVP("pi", username);
	}

	template<class Archive>
	void load(Archive& ar, const unsigned int version) {
		std::string ts;

		ar & BOOST_SERIALIZATION_NVP(ts);
		ar & HAATHI_SERIALIZATION_NVP("pi", username);
		time.load(ts);
	}
    BOOST_SERIALIZATION_SPLIT_MEMBER()

	private:
		// Provenance Storage details here.
		/*!
		 This variable stores the time at which the provenance was acquired. Currently, the time
		 is acquired by querying the OS; this might be insecure if the attacker hampers their 
		 OS to use a bogus time... This can be dealt with later. Maybe you want to obfuscate
		 the function to get the time? Or you could call some "trusted" server to give you the
		 time.
		 */
		ProvenanceTime time;

		/*!
		 This variable stores the username at the time the ProvenanceEvent is generated. 
		 Note that this might be insecure because the username is simply the one used to log 
		 into the Trusted Server. I don't know. Obfuscate?
		 */
		std::string username;

	public:
		virtual void track(com::haathi::document::IDocumentStateTracker& documentStateTracker) {
			// The default implementation is no-op.
		}

		// Need to have a range for watermarking
		virtual com::haathi::text::Range getWatermarkRange() {
			return com::haathi::text::Range::invalidRange;
		};

		/*!
		 This is the 'default' constructor that should be used to generate new 
		 ProvenanceEvent objects. It stores the event-type (not), as well as the location
		 in the document. The implementation of the exact type of Provenance that is
		 being stored should be implemented by the subclasses, depending on the
		 provenance type.
		 @param newEventType -- a string indicating the type of event.
		 The parameter is deprecated.  The event type is wholly encoded in the class.
		 If some subclass needs to distinguish between "sub-event" types, it can
		 use its own variable.
		 */
		ProvenanceEvent();

		void setUsername(std::string username);

		/*!
		 The destructor has nothing to do in this base class.
		 */
		virtual ~ProvenanceEvent();

		/*!
		 We allow for copying provenance events. This may not be needed, or could pose
		 a security risk. I don't think so, but I can't say for certain.
		 
		 @param other -- the event to copy.
		 */
		ProvenanceEvent(const ProvenanceEvent& other);

		/*!
		 We allow for the assigning of provenance events. This may not be needed, or
		 could pose a security risk. I don't think so, but I can't say for certain.

		 @param other -- the event to assign.
		 @result -- this instance of ProvenanceEvent after the assignment.
		 */
//		ProvenanceEvent& operator=(const ProvenanceEvent& other);

		/*!
		 This function returns the date and time that this instance of ProvenanceEvent 
		 was created (and hence the time that the action this instance corresponds to 
		 was performed).
		 
		 @result -- the ProvenanceTime for this event.
		 */
		ProvenanceTime getTime() const {
			return time;
		}

		/*!
		 This function returns a string corresponding to the user who performed 
		 the action that this ProvenanceEvent corresponds to.

		 @return -- a string corresponding to the user creating this Provenance.
		 */
		std::string getUser() const {
			return username;
		}

		/*!
		 This function returns a string corresponding to the type of ProvenanceEvent
		 this instance is. For instance, if this corresponds to a situation where
		 the user added text, the string returned would be "text:ADD_EVENT". It may be 
		 better later to use an enum or something similar, but the use of a string
		 allows for extending the types of Provenance later without changing the file
		 that the enum would otherwise be declared in.
		 
		 @return -- a string corresponding to the event-type of this ProvenanceEvent.
		 */
		virtual std::string getEventType() const = 0;

		/*!
		 This function will "load" the given ProvenanceEvent to the component passed in.
		 
		 This function is what distinuishes different types of Provenance; hence it is
		 purely virtual, leaving the implementation to the subclasses.
		 
		 Note that this function will assume that the provenance event can actually
		 be performed; if not, then an exception should be thrown.
		 
		 @param document -- the instance of the document to load to.
		 @param names -- the names of any properties that might be needed to handle this 
		 ProvenanceEvent.
		 @param values -- the values of the respective properties that help with handling
		 the ProvenanceEvent.
		 @return -- true if the call was successful, false otherwise.
		 */
		virtual bool redo(com::sun::star::uno::Reference<com::sun::star::lang::XComponent> document, com::sun::star::util::Color color = 0xffffffff) const {
			// By default do nothing
			return true;
		}

		/*!
		 This function will "unload" the given ProvenanceEvent to the component passed in.
		 
		 This function is what distinuishes different types of Provenance; hence it is
		 purely virtual, leaving the implementation to the subclasses.
		 
		 Note that this function will assume that the provenance event can actually
		 be performed; if not, then an exception should be thrown.
		 
		 @param document -- the instance of the document to load to.
		 @param names -- the names of any properties that might be needed to handle this 
		 ProvenanceEvent.
		 @param values -- the values of the respective properties that help with handling
		 the ProvenanceEvent.
		 @return -- true if the call was successful, false otherwise.
		 */
		virtual bool undo(com::sun::star::uno::Reference<com::sun::star::lang::XComponent> document, com::sun::star::util::Color color = 0xffffffff) const {
			// By default do nothing
			return true;
		}

		virtual OUString apply(OUString input) {
			return input;
		}

		/*!
		 This function will return a string indicating the (terse) details of this 
		 ProvenanceEvent.
		 
		 Any subclasses of ProvenanceEvent should override this method, while still calling
		 this version first.
		 */
		virtual rtl::OUString toString() const;

		/*!
		 This function will return a string that describes this provenance event in more
		 detail. This should not be terse, since this is used to describe the event in the
		 dialog.
		 
		 @result -- a string describing this ProvenanceEvent in more detail.
		 */
		virtual rtl::OUString getEventDescription() const {
			// By default just call the other one
			return printToTable();
		}

		/*!
		 This function will return a string that labels this ProvenanceEvent in a table.
		 @result -- a string describing this ProvenanceEvent.
		 */
		virtual rtl::OUString printToTable() const = 0;

		/*!
		 This function will print this object to the stream. This should be used only for
		 debugging.
		 
		 @param stream -- the stream to print to.
		 @param event -- the ProvenanceEvent to print.
		 @result -- the instance "stream" after the ProvenanceEvent has been printed.
		 */
		friend std::ostream& operator<<(std::ostream& stream,
				const ProvenanceEvent& event) {
			// This is a double conversion, but it is just for debugging
			return operator<<(stream, com::haathi::utils::convertToString(event.toString()));
		}
};
/******************************************************************************
ProvenanceEvent subclasses
******************************************************************************/
// This shortcut is to reduce the size of the XML
#define ProvenanceDocumentEvent ProvenanceEvent
#define ProvenanceTextEvent ProvenanceEvent

/*
class ProvenanceDocumentEvent: public ProvenanceEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ProvenanceEvent);
	}
};

class ProvenanceTextEvent: public ProvenanceEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ProvenanceEvent);
	}
};
*/
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// This macro is used by Boost to recognize that this class is abstract. This can
// pose difficulties in serialization because we must know which instance of
// provenance to actually load when serializing. This macro indicates to the serializer
// that this class and its subclasses must register their types in order to 
// properly serialize.
BOOST_SERIALIZATION_ASSUME_ABSTRACT(com::haathi::provenance::ProvenanceEvent);
//BOOST_SERIALIZATION_ASSUME_ABSTRACT(com::haathi::provenance::ProvenanceDocumentEvent);
//BOOST_SERIALIZATION_ASSUME_ABSTRACT(com::haathi::provenance::ProvenanceTextEvent);
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
