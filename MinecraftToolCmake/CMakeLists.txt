#Root cmake
cmake_minimum_required(VERSION 3.22)

Project(MinecraftTool VERSION 1.0.0)

add_subdirectory(ThirdPartyHeaderOnly)
message("Include dir: ${ThirdPartyHeaderOnlyIncludeDir}")

add_subdirectory(CommonCode)
message("Include dir: ${CommonCodeIncludeDir}")

add_subdirectory(Parser)
message("Include dir: ${ParserIncludeDir}")

add_subdirectory(ConsoleTool)

target_include_directories(
Parser PUBLIC
"${CommonCodeIncludeDir}"
"${ThirdPartyHeaderOnlyIncludeDir}"
)

target_include_directories(
ConsoleTool PUBLIC
"${CommonCodeIncludeDir}"
"${ParserIncludeDir}"
)

target_link_libraries(
	ConsoleTool PUBLIC Parser
)

add_subdirectory("JUCE")
add_subdirectory("JuceGUI")
target_include_directories(
MyJUCEApp PUBLIC
"${CommonCodeIncludeDir}"
"${ParserIncludeDir}"
)

target_link_libraries(
	MyJUCEApp PUBLIC Parser
)

install(TARGETS ConsoleTool MyJUCEApp DESTINATION bin)
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_NAME "MinecraftTool")

set(CPACK_NSIS_MODIFY_PATH ON)
include(CPack)