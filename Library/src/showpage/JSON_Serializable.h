#ifndef SRC_JSON_SERIALIZABLE_H
#define SRC_JSON_SERIALIZABLE_H

#include <string>
#include <chrono>
#include <vector>
#include <json.hpp>

/**
 * If you subclass from this, then we have clean methods of converting to/from JSON.
 */
class JSON_Serializable {
public:
    JSON_Serializable();
    virtual ~JSON_Serializable();
	
    /** Implement this method to populate ourself from this json. */
	virtual void fromJSON(const nlohmann::json & json) = 0;

    /** Implement this method to populate this json from ourself. */
	virtual void toJSON(nlohmann::json & json) const = 0;


    /**
     * This is a form of GET for a string.
     */
	void fillString(std::string & output, const nlohmann::json &json, const std::string &key) {
		output = "";

		auto iter = json.find(key);
		if (iter != json.end()) {
			auto foo = *iter;
			nlohmann::json::value_t type = foo.type();

			if (type == nlohmann::json::value_t::string) {
				output = json.value(key, output);
			}
		}
	}

    std::string toString() const;

	static std::string stringValue(const nlohmann::json &propertyTree, const std::string &key);
	static int intValue(const nlohmann::json &propertyTree, const std::string &key);
	static long longValue(const nlohmann::json &propertyTree, const std::string &key);
	static double doubleValue(const nlohmann::json &propertyTree, const std::string &key);
	static bool boolValue(const nlohmann::json &propertyTree, const std::string &key);
	static nlohmann::json jsonValue(const nlohmann::json &propertyTree, const std::string &key);
	static std::chrono::system_clock::time_point timeValue(const nlohmann::json &propertyTree, const std::string &key);

    static void setStringValue(nlohmann::json &json, const std::string &key, const std::string &value);
};

/**
 * This provides an easy base type for any vector of objects that are JSON_Seriablizable.
 */
template <class ObjectType>
class JSON_Serializable_Vector : public std::vector<ObjectType>, public JSON_Serializable {
public:

    /**
     * Read ourself from this JSON. It's expected to be an array.
     */
    void fromJSON(const nlohmann::json & array) {
        for (auto iter = array.begin(); iter != array.end(); ++iter) {
            nlohmann::json obj = *iter;

            ObjectType thisDiff;
            thisDiff.fromJSON(obj);
            this->push_back(thisDiff);
        }
    }

    /**
     * Write ourself to this json.
     */
    void toJSON(nlohmann::json & json) const {
        for (const ObjectType & obj: *this) {
            nlohmann::json childJson = nlohmann::json::object();
            obj.toJSON(childJson);
            json.push_back(childJson);
        }
    }

};

#endif /* SRC_JSON_SERIALIZABLE_H */
