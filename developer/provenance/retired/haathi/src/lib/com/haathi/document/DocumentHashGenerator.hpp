#ifndef DOCUMENT_HASH_GENERATOR_HPP
#define DOCUMENT_HASH_GENERATOR_HPP

/*!
 @file DocumentHashGenerator.hpp

 @author Aaron Gibson
 @date May 27, 2012

 @brief
 This header outlines a class that handles the generic "Hash Generation" 
 for office documents in this provenance system. A Hash generator was deemed
 to be necessary because all of the different types of office documents may
 support different services and different types of provenance and so for
 simplicity, this was all abstracted into this class.

 The eventual plan is to be able to "register" different types of provenance
 so that when new types of provenance are added to the system, all that needs
 to be implemented is a function that actually collects the provenance. The
 need for this to be universal stems from the fact that different operating 
 systems, or even different computers on the same system might generate the
 same document in different ways. For the provenance scheme to work, this must
 be standardized as much as possible.

 @todo
 Finish the first version of this.
 */

namespace com {
namespace haathi {
namespace document {

/*!
 @class DocumentHashGenerator
 
 This class implements a generic document hash generator that will ideally
 generate the same hash for the same document regardless of OS or other 
 external factors.
 */
class DocumentHashGenerator {

};

}
;
}
;
}
;
// namespace com::haathi::document

#endif

