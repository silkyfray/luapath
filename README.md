# Luapath
Luapath is a small C++ library that can load  and traverse .lua files.

# Usage
Let's say we have the following lua file called **test.lua**: 
```lua
skinnedModels = {
barbarian = {
		modelDir = "models/barbarian/exported/barbarian7.dae",
		vertexShader = "skinning",
		fragmentShader = "texture_d",
		animationName = "wait",
		additionalAnimations = {
			{animationName = "dance" , fileDir = "models/barbarian/exported/animations/dance.dae"},
			{animationName = "run" , fileDir = "models/barbarian/exported/animations/run.dae"},
			{animationName = "walk" , fileDir = "models/barbarian/exported/animations/walk.dae"},
			{animationName = "lie" , fileDir = "models/barbarian/exported/animations/lie.dae"},
		},
		aabb = {
			min = {x = -0.5, y = 0.0, z = -0.5},
			max = {x = 0.5, y = 2.2, z = 0.5}
		}
	},
```
### Load file
```cpp
#include <luapath/luapath.hpp>
...
luapath::LuaState myfile("test.lua");
```
If the file can't be loaded a **lua_state_exception** exception is thrown.
### Traversal
Traversing the file is very easy. 
"**.**" is used to traverse string keys and "**#**" is used to traverse number keys. If there is an error traversing a **path_lookup_exception** exception is thrown.

The function **getValue** returns either a string, int or a float depending on what C++ value we are trying to cast the lua value to. If it can't cast to the desired type a **type_mismatch_exception** exception is thrown.

The function **getGlobalTable** returns a Table object by searching the top most level of the file.

The function **getTable** returns an intermediary table.

For example if we want to get the vertex shader of the barbarian we do:
```cpp
std::string shader;
shader = myfile.getGlobalTable("skinnedModels").getValue(".barbarian.vertexShader");
```
and if we wanted to get the second additional animation:
```cpp
std::string anim;
anim = myfile.getGlobalTable("skinnedModels").getValue(".barbarian.additionalAnimations#2");
```
also we can play around with intermediary table so we don't have to repeat long sequences:
```cpp
luapath::Table models;
model = myfile.getGlobalTable("skinnedModels");
luapath::Table currModel = models.getTable(".barbarian");
std::string shader = currModel.getValue(".vertexShader");
std::string anim = currModel.getValue(".additionalAnimations#2");
```
sometimes we are not sure if a key even exists. For example if we wanted to iterate over the "additionalAnimations" table until the end we can do the following:
```cpp
luapath::Table additionalAnimTable = currModel.getTable(".additionalAnimations");
int additionalAnimNum = 1; // lua indexing is not 0 based
luapath::Table currAnim;
while (additionalAnimTable.getTable(string("#") + std::to_string(additionalAnimNum), currAnim))
{
    //do something cool
    //
	additionalAnimNum++;
}
```
# Installation
Include the **include** folder for the header files.
The library has a dependency on the lua C++ library so you need to include and link against it too. It is available under the **3rdparty** folder.

Alternatively, if you use CMake you can add the following to your Cmake file:
```cmake
add_subdirectory(${DEVLIB_DIR}/luapath "luapath")
include_directories(${LUAPATH_INCLUDE_DIR})
target_link_libraries(${APP_NAME} ${LUAPATH_LIBRARIES})
```

# To Do
Add functionality to escape the characters "." and "#" in the search string
