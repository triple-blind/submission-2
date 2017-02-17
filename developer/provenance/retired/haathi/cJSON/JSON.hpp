/*
 *  JSON.hpp
 *  
 *
 *  Created by Aaron Gibson on 2/20/12.
 *  Copyright 2012 Aaron Gibson. All rights reserved.
 *
 */

#ifndef JSON_HPP
#define JSON_HPP

namespace com {
namespace haathi {
namespace utils {

class JSON {
private:
	cJSON* handle;
	mutable int refCount;
public:
	JSON();
	JSON(bool val);
	JSON(int val);
	JSON(double val);
	JSON(std::string val);
	JSON(JSON& other);
	JSON& operator=(JSON& other);
	virtual ~JSON();
};

}
;
}
;
}
;
// namespace com::haathi::utils

#endif

