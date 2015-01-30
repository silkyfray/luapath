#ifndef LUATYPES_HPP
#pragma once

#include <string>
#include <map>
#include <queue>
#include <iostream>

#include "luapath.hpp"

namespace luapath
{
	class Table;

	static const char NUMBER_TOKEN = '#';
	static const char STRING_TOKEN = '.';
	static const int INDENT_SIZE = 5;

	/** @brief It represents LHS of the '=' of a lua object.
		@details A Key has a value and a type. 
		There might be objects with no explicit key assigned. In such cases Lua automatically
		assign a number key to the object.
		The value of the key is stored as a string to forego fancy template programming
	*/
	struct  Key
	{

		/** @enum A lua key can be either a string or a integer number*/
		enum class Type{ STRING, NUMBER };
	
		/**@brief Instantiate a Key with Type NUMBER and key value @p index*/
		explicit Key(int index);

		/**@brief Instantiate a Key with Type STRING and key value @p key*/
		explicit Key(const std::string &key);
		
		Key(Key::Type type, const std::string &key);
		
		operator std::string() const;
		operator int() const;

		/**true iff both type and key value are the same*/
		bool operator==(const Key &other) const;

		/**true iff either type or key value are not the same*/
		bool operator!=(const Key &other) const;

		/**compare this object with @p other
		   a number key is considered less than a string key. This has consequences when ordering the keys in a Table
		*/
		bool operator<(const Key &other) const;
		friend std::ostream& operator<< (std::ostream& out, const Key &key);
		friend class Table;

		Type type;
		std::string key;
	};

	/** @brief Represents the RHS of the '=' of a lua object.
		A Value has a type and value.
	*/
	struct  Value
	{
		/**A Value with Type TABLE will be recursively traversed*/
		enum class Type{ BOOL, STRING, NUMBER, TABLE };

		Value(Value::Type type, const std::string &val);
		
		/**Needed because under vc12 compiler(at the very least) char* is promoted to bool 
		   and not string when a ctor with bool overload is present
		*/
		Value(Value::Type type, const char *val);

		Value(Value::Type type, bool val);

		Value(Value::Type type, float val);

		operator std::string() const;

		operator int() const;

		operator float() const;

		operator bool() const;

		/** true iff type and value are the same*/
		bool operator==(const Value &other) const;

		/** true iff type or value differ*/
		bool operator!=(const Value &other) const;

		/** If both values are of the Type NUMBER then compares as floats
			If both values are of Type BOOL then compares as bool
			Otherwise performs lexicographical_compare as strings
		*/
		bool operator<(const Value &other) const;
		friend std::ostream& operator<< (std::ostream& out, const Value &value);

		Type type;
		std::string value;
	};


	/** @brief Represents the lua table as a C++ object.
		@todo escape token characters 
	*/
	class  Table
	{
	public:
		/** a Value which doesn't have nested tables */
		typedef std::map<Key, Value> LeafSet;
		
		/**a Table which may or may not have nested tables*/
		typedef std::map<Key, Table> NestedSet;

		/**queue of Key objects constructed from a string searchPath 
			which is the input parameter to Table::getValue and Table::getTable*/
		typedef std::queue<Key> KeyPath;

		/**@p tableKey the name of the table*/
		explicit Table(const Key &tableKey);

		/** Get a Value object of the Key that is the last field of the @p searchPath
			hash '#' token infront of a field denotes that the field is a number key
			. '.' token infront of a field denotes that the field is a string key
			e.g if searchKey == "vehicles.cheap#5" then the Value object will be returned representing
			the 5th value of the "cheap" table of the "vehicles" table 
		*/
		Value getValue(const std::string &searchPath);

		/** Get a Value object of the Key that is the last field of the @p searchPath */
		Table getTable(const std::string &searchPath);

		/** Get a an array of type T of the leafSet of the current table*/
		template<class T>
		std::vector<T> toArray();

		friend class LuaState;
	private:
		/**Converts a string @p searchPath to a queue of Key which is used to search the Table*/
		KeyPath tokenizePath(const std::string &searchPath) const;

		bool isToken(std::string::const_iterator &it) const;

		friend std::ostream& operator<< (std::ostream& out, const Table &table);

		void print(std::ostream &out, const Table& table, int level) const;
	private:
		Key tableKey;
		LeafSet leafSet;
		NestedSet nestedSet;
	};

}

template<class T>
std::vector<T> luapath::Table::toArray()
{
	std::vector<T> result;
	for (LeafSet::const_iterator leafIt = leafSet.begin();
		leafIt != leafSet.end();
		++leafIt)
	{   // takes advantage of the overloaded type operators of the Value class
		//throws an exception if T != Value.Type
		result.push_back(static_cast<T>(leafIt->second));
	}
	return result;
}
#endif // !LUATYPES_HPP

