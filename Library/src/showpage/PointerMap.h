#ifndef SRC_VITAC_POINTERMAP_H_
#define SRC_VITAC_POINTERMAP_H_

#include <map>

/**
 * This is a wrapper around a std::map<KeyType, ObjectType *> -- that is, a map
 * to pointers of some object. If you leave stuff in the map, then when we're destroyed,
 * we delete the data you left inside us. I also provide a few convenience functions,
 * because the standard lib creators made a few really weird decisions.
 */
template <class KeyType, class ObjectType>
class PointerMap : public std::map<KeyType, ObjectType *>
{
public:
    /** Destructor. */
	virtual ~PointerMap() {
		eraseAll();
	}

    /** Do we have this key? */
	bool contains(const KeyType & key) const {
		typename std::map<KeyType, ObjectType *>::const_iterator findValue = find(key);
		return findValue != this->cend();
	}

    /** Return the object represented by this key. */
	ObjectType * get(const KeyType & key) {
		typename std::map<KeyType, ObjectType *>::iterator ptr = this->find(key);
		if (ptr != this->end()) {
			return ptr->second;
		}
		return NULL;
	}

	/**
	 * Erase our contents, freeing any memory we're pointed to.
	 */
	void eraseAll() {
		for (typename std::map<KeyType, ObjectType *>::iterator ptr = this->begin(); ptr != this->end(); ++ptr) {
			if (ptr->second != NULL) {
				delete ptr->second;
			}
		}

		std::map<KeyType, ObjectType *>::clear();
	}
};


#endif /* SRC_VITAC_POINTERMAP_H_ */
