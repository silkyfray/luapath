#ifndef LUASTATE_HPP
#pragma once

#include <string>

#include "luapath.hpp"

struct lua_State;

namespace luapath
{
	class  Table;
	struct  Key;
	struct  Value;

	/** @brief Encapsulates the raw Lua state
		@details Provides wrapper function for some commonly used lua functions.
		Intended to be used for loading a lua file and reading a global table
		from the file which can then be further traversed.
	*/
	class  LuaState
	{
	public:
		/** Upon instantiation -- loads a new lua state*/
		LuaState();
		/** @brief Calls @link LuaState::close() */
		virtual ~LuaState();
		/** @brief Releases and unloads the lua state */
		void close();

		/** @brief Loads a string onto the lua state*/
		void loadString(const std::string &str);

		/** @brief Load a file as a string onto the lua state*/
		void loadFile(const std::string &filepath);

		/** @brief Checks whether the state is loaded
			@return true if a successful call to LuaState::loadString or LuaState::loadFile has been made */
		bool isLoaded() const;
		
		/** @brief Get a Value object with name @p fieldName from the global scope in the loaded lua state*/
		Value getGlobalValue(const std::string &fieldName);
		
		/** @brief Get a Table object with name @p tableName from the global scope in the loaded lua state
			Recursively traverse the lua table and constructs a full representation of the lua object by
			populating an instance of Table
		*/
		Table getGlobalTable(const std::string &tableName) ;

	private:
		/** @brief helper function to LuaState::getGlobalTable */
		void getTableContents(Table &currTable, int tableIndex);

		/** @brief constructs a Key object from the value on the given @p index on the lua stack*/
		Key getKey(int index);

		/** @brief constructs a Value object from the value on the given @p index on the lua stack*/
		Value getValue(int index);
	private:
		lua_State *m_L;
		bool loaded;

	};
}
#endif