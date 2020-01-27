#include <iostream>
#include <sstream>

#include "JSON_Serializable.h"
#include "date.h"

using namespace std;
using namespace nlohmann;

/**
 * Constructor.
 */
JSON_Serializable::JSON_Serializable() {
}

/**
 * Destructor.
 */
JSON_Serializable::~JSON_Serializable() {
}

/**
 * Convert the object to a string.
 */
std::string
JSON_Serializable::toString() const {
    nlohmann::json json;
    toJSON(json);
    return json.dump(2);
}

/**
 * Get this key's value as a string.
 *
 * @returns The object in string representation or an empty string if the key isn't found.
 */
std::string
JSON_Serializable::stringValue(const json &propertyTree, const std::string &key)
{
	auto it = propertyTree.find(key);
	if (it != propertyTree.end()) {
		json value = *it;
		if (!value.is_null()) {
			return value.get<string>();
		}
	}
	return "";
}

/**
 * Get this key's value as an int. May throw an exception if the key is found but
 * it's a non-integer value.
 *
 * @returns the value or 0 if not found.
 */
int
JSON_Serializable::intValue(const json &propertyTree, const std::string &key)
{
	auto it = propertyTree.find(key);
	if (it != propertyTree.end()) {
		json value = *it;
		if (!value.is_null()) {
			return value.get<int>();
		}
	}
	return 0;
}

/**
 * Get this key's value as a double. May throw an exception if the key is found but
 * it's a non-floating value.
 *
 * @returns the value or 0.0 if not found.
 */
double
JSON_Serializable::doubleValue(const json &propertyTree, const std::string &key)
{
	auto it = propertyTree.find(key);
	if (it != propertyTree.end()) {
		json value = *it;
		if (!value.is_null()) {
			return value.get<double>();
		}
	}
    return 0.0;
}

/**
 * Get this key's value as a long. May throw an exception if the key is found but
 * it's a non-numeric value.
 *
 * @returns the value or 0 if not found.
 */
long
JSON_Serializable::longValue(const json &propertyTree, const std::string &key)
{
	auto it = propertyTree.find(key);
	if (it != propertyTree.end()) {
		json value = *it;
		if (!value.is_null()) {
			return value.get<long>();
		}
	}
	return 0;
}

/**
 * Get this key's value as a boolean. May throw an exception if the key is found but
 * it's a non-boolean value.
 *
 * @returns the value or false if not found.
 */
bool
JSON_Serializable::boolValue(const json &propertyTree, const std::string &key)
{
	auto it = propertyTree.find(key);
	if (it != propertyTree.end()) {
		json value = *it;
		if (!value.is_null()) {
			return value.get<bool>();
		}
	}
	return false;
}

/**
 * Find this key and return the corresponding JSON object.
 *
 * @returns the value or an empty JSON object.
 */
nlohmann::json
JSON_Serializable::jsonValue(const nlohmann::json &propertyTree, const std::string &key) {
	auto it = propertyTree.find(key);
	if (it != propertyTree.end()) {
		return *it;
	}
	return nlohmann::json::object();
}

/**
 * Returns this key's value as a time_point from the string representation.
 */
chrono::system_clock::time_point
JSON_Serializable::timeValue(const json &propertyTree, const std::string &key)
{
	chrono::system_clock::time_point retVal;
	string strValue = stringValue(propertyTree, key);
	if (strValue.length() > 0) {
		cout << "Try to parse: " << strValue << endl;
		istringstream in{strValue};
		in >> date::parse("%Y-%m-%d %H:%M:%S", retVal);
	}
	return retVal;
}

/**
 * Sets a string value in this JSON only if we have content.
 */
void
JSON_Serializable::setStringValue(nlohmann::json &obj, const std::string &key, const std::string &value) {
    if (key.length() > 0 && value.length() > 0) {
        obj[key] = value;
    }
}
