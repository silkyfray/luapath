#include "utils.hpp"
#include "luapath/luapath.hpp"

#define BOOST_TEST_MAIN
//#define BOOST_TEST_NO_LIB
//#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::ostream;
using namespace luapath;

struct luaStateInitFixture
{
	luaStateInitFixture()
	{
		expectedStringArray.push_back("10");
		expectedStringArray.push_back("20");
		expectedStringArray.push_back("30");
		expectedStringArray.push_back("40");
		expectedStringArray.push_back("50");

		testFile = "testFile.lua";
		testString = ""
		"cars = { "
		"	honda = { price = 40.8, speed = 20, owner = \"Jack\"}, "
		"	bmw = { price = 40, speed = 250}, "
		"	[5] = { price = 0, class = { model =\"S\", engine = 1.5 , 8,9}, old = true}, "
		"	[\"yello\"] = \"hey\", "
		"	\"x\", "
		"	y = nil "
		"	} "
		""
		"t1 = { "
		"	4,5,6,7 "
		" } "
		""
		"N1 = 5.0 "
		"bool1 = false "
		"str1 = \"hello\" "
		"table1 = {} "
		""
		"company = { "
		"	what = \"Business\", "
		"	really = false, "
		"	[\"why?\"] = \"its fictional\", "
		"	established = 2014, "
		"	sector = \"Construction\", "
		"	buildings = { "
		"		{city = \"Dublin\", "
		"		 employees = 200, "
		"		 revenue = 5.0, "
		"		 bosses = {\"Jack\",\"John\"} "
		"		}, "
		"		{city = \"Carlow\", "
		"		 bosses = 0 "
		"		} "
		"	}, "
		"	empty = {}, "
		"	{1,2,3}, "
		"	[-1] = \"ten\", "
		"	good = true, "
		"	good = true, "
		"	money = nil "
		""
		"} "
		""
		"superStructure = {"
		"	[1] = {"
		"		level2 = {"
				"   arrays = { "
						"	[6]={10,20,30,40.4,50}, "
						"	[7]={10,20,30,40.4,50,true}, "
						"	[8]={10,20, allowed = 30,40.4,50}, "
						"	[9]={10,20, notAllowed = \"wrong\",40.4,50}, "
						"	[10]={\"10\",\"20\",\"30\",\"40\",\"50\"},"
						"	[11]={true}"
		"			}, "
		"			[3] = {"
		"				[\"4\"] = {"
		"					[\"5\"] = 6,"
		"					[\"5.1\"] = {\"x\",\"y\",\"z\"},"
		"					[\"5#2\"] = {false,false}"
		"				}	"
		"			}"
		"		}"
		"	}"
		"} ";
	};
	~luaStateInitFixture()
	{
	};

	string testFile;
//C:\Users\Tony-Laptop\Documents\Visual Studio 2013\Projects\LuaCWalk\Output-Debug\UnitTest
	string testString;
	vector<string> expectedStringArray;
	luapath::LuaState state;
};

struct luaStateLoadedFixture
	:public luaStateInitFixture
{
	luaStateLoadedFixture()
	{
		//state.loadFile(testFile);
		state.loadString(testString);
		BOOST_REQUIRE(state.isLoaded());
	}
	~luaStateLoadedFixture()
	{
	};
};



BOOST_FIXTURE_TEST_SUITE(readValues, luaStateLoadedFixture);
BOOST_AUTO_TEST_CASE(readValue)
{
//	state.printGlobal("cars");
	Value num1 = state.getGlobalValue("N1");
	BOOST_CHECK_CLOSE((float)num1, 5.0f, 0.01f);
	BOOST_CHECK_MESSAGE((int)num1 == 5, "result: " << (int)num1);
	Value str1 = state.getGlobalValue("str1");
	BOOST_CHECK_MESSAGE(((string)str1) == "hello", "result: " << (string)str1);
	Value bool1 = state.getGlobalValue("bool1");
	BOOST_CHECK_MESSAGE((bool)bool1 == false, "result: " << (string)bool1);

}
BOOST_AUTO_TEST_CASE(readKeyValueThrows)
{
	BOOST_CHECK_THROW(state.getGlobalValue("company"), type_mismatch_exception);
	BOOST_CHECK_THROW(state.getGlobalValue("nonExistent"), path_lookup_exception);
}
BOOST_AUTO_TEST_CASE(constructTable)
{

	Table company = state.getGlobalTable("company");
	ostream dummy(0);
	dummy << company;

}

BOOST_AUTO_TEST_CASE(readTableValues)
{
	Table company = state.getGlobalTable("company");
	Value what = company.getValue(".what");
	BOOST_CHECK_MESSAGE((string)what == "Business", what);
	Value established = company.getValue(".established");
	BOOST_CHECK_MESSAGE((int)established == 2014, established);
	Value really = company.getValue(".really");
	BOOST_CHECK_MESSAGE(!(bool)really, really);

	Table cars = state.getGlobalTable("cars");
	Table honda = cars.getTable(".honda");
	Value price = honda.getValue(".price");
	BOOST_CHECK_CLOSE((float)price, 40.8, 0.01f);
	Value bmwPrice = cars.getValue(".bmw.price");
	BOOST_CHECK_MESSAGE((int)bmwPrice == 40, bmwPrice);
	Value someModel = cars.getValue("#5.class.model");
	BOOST_CHECK_MESSAGE((string)someModel == "S", someModel);

	Table superStructure = state.getGlobalTable("superStructure");
	Value reallyNestedSix = superStructure.getValue("#1.level2#3.4.5");
	BOOST_CHECK_MESSAGE((int)reallyNestedSix == 6, reallyNestedSix);
}

BOOST_AUTO_TEST_CASE(readTableValuesReturnTrue)
{
	Table company = state.getGlobalTable("company");
	Value what;
	BOOST_CHECK_MESSAGE(company.getValue(".what",what), what);
	BOOST_CHECK_MESSAGE((string)what == "Business", what);

	Table superStructure = state.getGlobalTable("superStructure");
	Value reallyNestedSix; 
	BOOST_CHECK_MESSAGE(superStructure.getValue("#1.level2#3.4.5", reallyNestedSix), reallyNestedSix);
	BOOST_CHECK_MESSAGE((int)reallyNestedSix == 6, reallyNestedSix);
}

BOOST_AUTO_TEST_CASE(readTableValuesThrows)
{
	Table cars = state.getGlobalTable("cars");
	//Empty string when getting value is not allowed
	BOOST_CHECK_THROW(cars.getValue(""), path_lookup_exception);
	//Search value where a middle search key is there but there is extra junk path 
	BOOST_CHECK_THROW(cars.getValue(".honda.price.JUNK"), path_lookup_exception);
	//same story
	BOOST_CHECK_THROW(cars.getValue("#5.NON_EXISTANT_TABLE.class.model"), path_lookup_exception);
	//try to get a value from a path which ends on a table
	BOOST_CHECK_THROW(cars.getValue("#5.class"), path_lookup_exception);
	//invalid starting character. Only '#' and '.' allowed
	BOOST_CHECK_THROW(cars.getValue("Wrong"), path_lookup_exception);
	//! @todo add escape characters
	// Two consecutive tokens: probably trying to reference a table which has a dot as starting character
	BOOST_CHECK_THROW(cars.getValue("#5..class"), path_lookup_exception);
	// try to get a non existent table
	BOOST_CHECK_THROW(cars.getTable("#5.class.NON_EXISTANT_TABLE"), path_lookup_exception);

}

BOOST_AUTO_TEST_CASE(readTableValuesReturnFalse)
{
	Table cars = state.getGlobalTable("cars");
	Value dummy;
	Table dummyt;
	//Empty string when getting value is not allowed
	BOOST_CHECK(!cars.getValue("", dummy));
	//Search value where a middle search key is there but there is extra junk path 
	BOOST_CHECK(!cars.getValue(".honda.price.JUNK", dummy));
	//same story
	BOOST_CHECK(!cars.getValue("#5.NON_EXISTANT_TABLE.class.model", dummy));
	//try to get a value from a path which ends on a table
	BOOST_CHECK(!cars.getValue("#5.class", dummy));
	//invalid starting character. Only '#' and '.' allowed
	BOOST_CHECK(!cars.getValue("Wrong", dummy));
	//! @todo add escape characters
	// Two consecutive tokens: probably trying to reference a table which has a dot as starting character
	BOOST_CHECK(!cars.getValue("#5..class", dummy));
	// try to get a non existent table
	BOOST_CHECK(!cars.getTable("#5.class.NON_EXISTANT_TABLE", dummyt));
}

BOOST_AUTO_TEST_CASE(readTableArrays)
{
	Table arrays = state.getGlobalTable("superStructure").getTable("#1.level2.arrays");

	vector<string> arr1 = arrays.getTable("#10").toArray<string>();
	for (string str : expectedStringArray)
	{
		BOOST_CHECK_MESSAGE(std::find(arr1.begin(), arr1.end(), str) != arr1.end(), "Could not find : " << str);
	}

	vector<float> arr2 = arrays.getTable("#6").toArray<float>();
	BOOST_CHECK_MESSAGE(arr2.size() == 5, "Array not expected size : " << arr2.size());

	vector<int> arr3 = arrays.getTable("#8").toArray<int>();
	for (string str : expectedStringArray)
	{
		BOOST_CHECK_MESSAGE(std::find(arr3.begin(), arr3.end(), std::stoi(str)) != arr3.end(), "Could not find : " << str);
	}

	vector<bool> arr4 = arrays.getTable("#11").toArray<bool>();
	BOOST_CHECK_MESSAGE(arr4.size() == 1, "Array not expected size : " << arr4.size());


}
BOOST_AUTO_TEST_CASE(readTableArraysThrows)
{
	Table arrays = state.getGlobalTable("superStructure").getTable("#1.level2.arrays");
	//Array contains a value which is not a number
	BOOST_CHECK_THROW(arrays.getTable("#9").toArray<float>(), type_mismatch_exception);
	//wrong convertion altogether
	BOOST_CHECK_THROW(arrays.getTable("#10").toArray<bool>(), type_mismatch_exception);
}

BOOST_AUTO_TEST_SUITE_END();