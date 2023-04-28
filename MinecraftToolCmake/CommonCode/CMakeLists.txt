#Common code
#Header only
add_library(CommonCode INTERFACE "Block.h" "Structs.h")

Set(CommonCodeIncludeDir "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)