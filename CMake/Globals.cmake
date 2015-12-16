macro(add_directories PATH SRC_FILES)
	file(GLOB_RECURSE TMP_FILES ${PATH}/*.*)
	
	foreach(FILENAME ${TMP_FILES})
		file(RELATIVE_PATH FILENAME_RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${FILENAME})
		set(FILENAME_RELATIVE "Code/${FILENAME_RELATIVE}")
		string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GROUP ${FILENAME_RELATIVE})
		string(REPLACE / \\ SRC_GROUP ${SRC_GROUP})
		source_group("${SRC_GROUP}" FILES ${FILENAME})
	endforeach(FILENAME)
	
	set(${SRC_FILES} ${TMP_FILES})
endmacro(add_directories)

macro(add_files PATH SRC_FILES)
	file(GLOB TMP_FILES ${PATH}/*.*)

	foreach(FILENAME ${TMP_FILES})
		file(RELATIVE_PATH FILENAME_RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${FILENAME})
		set(FILENAME_RELATIVE "Code/${FILENAME_RELATIVE}")
		string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GROUP ${FILENAME_RELATIVE})
		string(REPLACE / \\ SRC_GROUP ${SRC_GROUP})
		source_group("${SRC_GROUP}" FILES ${FILENAME})
	endforeach(FILENAME)
	
	set(${SRC_FILES} ${TMP_FILES})
endmacro(add_files)