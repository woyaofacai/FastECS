set(PROJECT_NAME UnitTest)


add_executable(${PROJECT_NAME} 
	"../TinyECS.hpp" 
	"catch.hpp"
	"MathLib.hpp"
	"Common.hpp"
	"main.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-s")
