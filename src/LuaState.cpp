#include <iomanip>

#include "luapath/LuaState.hpp"
#include "luapath/LuaTypes.hpp"
#include "luapath/exceptions.hpp"


#include <lua.hpp>

using std::cout;
using std::endl;
using std::setw;
using std::string;

namespace luapath{


LuaState::LuaState()
	:m_L(luaL_newstate()), loaded(false)
{
}


LuaState::~LuaState()
{
	close();
}

void LuaState::close()
{
	loaded = false;
	if (m_L)
		lua_close(m_L);
	m_L = nullptr;
}

void LuaState::loadString(const std::string& str)
{
	int err = luaL_dostring(m_L, str.c_str());
	if (err)
	{
		string errorStr(lua_tostring(m_L, -1));
		close();
		throw lua_state_exception(errorStr);
	}
	loaded = true;
}

void LuaState::loadFile(const string& filepath)
{
	int err = luaL_dofile(m_L, filepath.c_str());
	if (err)
	{
		string errorStr(lua_tostring(m_L, -1));
		close();
		throw lua_state_exception(errorStr);
	}
	loaded = true;

}

bool LuaState::isLoaded() const
{
	return loaded;
}

Value LuaState::getGlobalValue(const string &fieldName) 
{
	lua_getglobal(m_L, fieldName.c_str());
	int t = lua_type(m_L, -1);
	switch (t)
	{
	case LUA_TSTRING:
		return Value(Value::Type::STRING, lua_tostring(m_L, -1));
	case LUA_TBOOLEAN:
		return Value(Value::Type::BOOL, lua_toboolean(m_L, -1) ? true : false);
	case LUA_TNUMBER:
		return Value(Value::Type::NUMBER, (float)lua_tonumber(m_L, -1));
	case LUA_TNIL:
		throw path_lookup_exception(string("The search field - ").append(fieldName).append(" - could not be found"));
	default:
		throw type_mismatch_exception("The type of the result value is not one of : string, number or boolean");
	}
	
}

Table LuaState::getGlobalTable(const string &tableName) 
{
	lua_getglobal(m_L, tableName.c_str());
	int t = lua_type(m_L, -1);
	switch (t)
	{
	case LUA_TTABLE:{
		Key key(tableName);
		Table root(key);
		getTableContents(root, -1);
		return root;
	}
	case LUA_TNIL:
		throw path_lookup_exception(string("The search field - ").append(tableName).append(" - could not be found"));
	default:
		throw type_mismatch_exception("The type of the result value is not a table");
	}

}

void LuaState::getTableContents(Table &currTable, int tableIndex)
{
	lua_pushnil(m_L);
	tableIndex--;
	try
	{
	while (lua_next(m_L, tableIndex) != 0)
	{
		Key key = getKey(-2);
		Value value = getValue(-1);
		if (value.type == Value::Type::TABLE)
		{
			currTable.nestedSet.emplace(std::make_pair(key, Table(key)));
			getTableContents(currTable.nestedSet.at(key), -1);
		}
		else
		{
			currTable.leafSet.emplace(std::make_pair(key, value));
		}
		lua_pop(m_L, 1);
	}
	}
	catch (std::exception &e){
		cout << "error: " << e.what() << endl;
	}

}

Key LuaState::getKey(int index)
{
	int keyType = lua_type(m_L,index);
	switch (keyType)
	{
	case LUA_TSTRING:
		return Key(lua_tostring(m_L, index));
	case LUA_TNUMBER:
		return Key(lua_tointeger(m_L, index));
	default:
		throw type_mismatch_exception("Construction of a Key not possible from a stack value that is not either NUMBER or STRING");
	}
}

Value LuaState::getValue(int index)
{
	int valueType = lua_type(m_L, index);
	switch (valueType)
	{
	case LUA_TSTRING:
		return Value(Value::Type::STRING, lua_tostring(m_L, index));
	case LUA_TBOOLEAN:
		return Value(Value::Type::BOOL, lua_toboolean(m_L, -1) ? true : false);
	case LUA_TNUMBER:
		return Value(Value::Type::NUMBER, (float)lua_tonumber(m_L, -1));
	case LUA_TTABLE:
		//we still need to represent a table in a Value object because
		// later it will help with the traversal of the Table object
		return Value(Value::Type::TABLE, "->");
	default:
		throw type_mismatch_exception("Construction of a Key not possible from a stack value that is not either NUMBER, STRING, BOOL or TABLE");
	}
}


}