#include <iomanip>

#include "luapath/LuaTypes.hpp"
#include "luapath/exceptions.hpp"

namespace luapath{
	using std::endl;
	using std::setw;
	using std::string;
	using std::ostream;

	Key::Key(int index)
		:type(Key::Type::NUMBER), key(std::to_string(index))
	{

	}
	Key::Key(const string &key)
		: type(Key::Type::STRING), key(key)
	{

	}
	Key::Key(Key::Type type, const string& key)
		: type(type), key(key)
	{

	}
	Key::operator string() const
	{
		return key;
	}
	Key::operator int() const
	{
		try
		{
			return std::stoi(key);
		}
		catch (std::invalid_argument &e)
		{
			throw type_mismatch_exception(e.what());
		}
	}
	bool Key::operator==(const Key &other) const
	{
		return type == other.type && key == other.key;
	}
	bool Key::operator!=(const Key &other) const
	{
		return !(*this == other);
	}
	bool Key::operator<(const Key &other) const
	{
		if (type == Key::Type::NUMBER && other.type == Key::Type::STRING)
			return true;
		else if (type == Key::Type::STRING && other.type == Key::Type::NUMBER)
			return false;
		if (type == Key::Type::NUMBER)
			return (int)*this < (int)other;
		return key < other.key;
	}
	std::ostream& operator<< (std::ostream& out, const Key &key)
	{
		if (key.type == Key::Type::NUMBER)
			out << "[" << (int)key << "]";
		else
			out << (string)key;
		return out;
	}

	Value::Value(Value::Type type, const string& val)
		: type(type), value(type == Value::Type::TABLE ? "->" : val)
	{

	}
	Value::Value(Value::Type type, const char *val)
		: type(type), value(type == Value::Type::TABLE ? "->" : val)
	{

	}
	Value::Value(Value::Type type, bool val)
		: type(type), value(val ? "true" : "false")
	{

	}
	Value::Value(Value::Type type, float val)
		: type(type), value(std::to_string(val))
	{

	}
	Value::operator string() const
	{
		return value;
	}
	Value::operator int() const
	{
		try
		{
			return std::stoi(value);
		}
		catch (std::invalid_argument &e)
		{
			throw type_mismatch_exception(e.what());
		}
	}

	Value::operator float() const
	{
		try
		{
			return std::stof(value);
		}
		catch (std::invalid_argument &e)
		{
			throw type_mismatch_exception(e.what());
		}
	}

	Value::operator bool() const
	{
		return value == "true" ? true :
			value == "false" ? false :
			throw type_mismatch_exception("invalid bool argument");
	}
	bool Value::operator==(const Value &other) const
	{
		return type == other.type && value == other.value;
	}
	bool Value::operator!=(const Value &other) const
	{
		return !(*this == other);
	}
	bool Value::operator<(const Value &other) const
	{
		if (type == other.type)
		{
			if (type == Value::Type::NUMBER)
				return (float)*this < (float)other;
			else if (type == Value::Type::BOOL)
				return (bool)*this < (bool)other;
		}
		return value < other.value;
	}

	std::ostream& operator<< (std::ostream& out, const Value &value)
	{
		switch (value.type)
		{
		case Value::Type::STRING:
			out << "\"" << (string)value << "\"";
			break;
		case Value::Type::NUMBER:
			out << (float)value;
			break;
		case Value::Type::BOOL:
		case Value::Type::TABLE:
			out << (string)value;
			break;
		}
		return out;
	}



	Table::Table(const Key &tableKey)
		: tableKey(tableKey)
	{

	}

	Value Table::getValue(const string &searchPath)
	{
		if (searchPath.size() == 0)
			throw path_lookup_exception("empty search path parameter not allowed for Table::getValue");
		KeyPath keyPath = tokenizePath(searchPath);
		const Table *currTable = this;
		do
		{
			Key currKey = keyPath.front();
			keyPath.pop();
			LeafSet::const_iterator leafIt = currTable->leafSet.find(currKey);
			
			if (leafIt != currTable->leafSet.end() && !keyPath.empty())
				throw path_lookup_exception("Found value but not at the end of the search path");
			else if (leafIt != currTable->leafSet.end())
				return leafIt->second;
			else
			{
				NestedSet::const_iterator nestedIt = currTable->nestedSet.find(currKey);
				if (nestedIt == currTable->nestedSet.end())
					throw path_lookup_exception("Could not find value at specified key");
				else
				{
					currTable = &nestedIt->second;
				}
			}
		} while (!keyPath.empty());
		throw path_lookup_exception("Exhausted search path but did not find a value");
	}

	Table Table::getTable(const string &searchPath)
	{
		if (searchPath.size() == 0)
			return *this;
		KeyPath keyPath = tokenizePath(searchPath);
		if (keyPath.size() == 0)
			return *this;
		const Table *currTable = this;
		do
		{
			Key currKey = keyPath.front();
			keyPath.pop();
			
			NestedSet::const_iterator nestedIt = currTable->nestedSet.find(currKey);
			if (nestedIt == currTable->nestedSet.end())
				throw path_lookup_exception("Could not find table at specified key");
			else
			{
				currTable = &nestedIt->second;
			}
		} while (!keyPath.empty());

		return *currTable;
	}
	Table::KeyPath Table::tokenizePath(const std::string &searchPath) const
	{
		KeyPath keyPath;
		if (searchPath.size() == 0)
			return keyPath;
		string::const_iterator currIndex = searchPath.begin();
		if (!isToken(currIndex))
			throw path_lookup_exception("Invalid starting token character");

		while (currIndex != searchPath.end())
		{
			// the index of the first character of the field
			string::const_iterator startIndex = currIndex + 1;
			char token = *currIndex;
			//iterate to next token. If we are at the end then this'd be
			// the last iteration
			while (++currIndex != searchPath.end() && !isToken(currIndex));
			
			string field(startIndex, currIndex);
			switch (token)
			{
			case STRING_TOKEN:
				keyPath.emplace(Key(field));
				break;
			case NUMBER_TOKEN:
				keyPath.emplace(Key(Key::Type::NUMBER,field));
				break;
			}
		}

		return keyPath;
	}
	bool Table::isToken(string::const_iterator &it) const
	{
		return (*it == STRING_TOKEN ||*it == NUMBER_TOKEN );
	}

	ostream& operator<< (ostream& out, const Table &table)
	{
		table.print(out, table, 1);
		return out;
	}

	void Table::print(ostream &out, const Table& table, int level) const
	{
		out << setw(level*INDENT_SIZE) << table.tableKey << " = {" << endl;
	
		if (!table.leafSet.empty())
		{
			LeafSet::const_iterator leafIt = table.leafSet.begin();
			LeafSet::const_iterator leafItEnd = --table.leafSet.end();
			for (; leafIt != leafItEnd; ++leafIt)
				out << setw(level*INDENT_SIZE) << "\t" << leafIt->first << " = " << leafIt->second << "," << endl;
			out << setw(level*INDENT_SIZE) << "\t" << leafIt->first << " = " << leafIt->second << endl;
		}

		NestedSet::const_iterator nestedIt = table.nestedSet.begin();
		for (; nestedIt != table.nestedSet.end(); ++nestedIt)
			print(out, nestedIt->second, level + 1);
		
		out << setw(level*INDENT_SIZE) << "}" << endl;
	}

}