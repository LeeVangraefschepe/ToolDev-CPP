#Parser (static library)
add_library(Parser "SceneParser.h" "SceneParser.cpp")

Set(ParserIncludeDir "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)