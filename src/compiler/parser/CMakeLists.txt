# Carburetta parser generate command:
file(GLOB CARB_EXE ${CARB_DIR}/build/*/Release/carburetta.exe)
add_custom_target(parser_generate
	COMMAND ${CARB_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/parser.cbrt --c ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp --h
)
