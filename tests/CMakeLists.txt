include(FetchContent)
FetchContent_Declare(catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.7
)

FetchContent_MakeAvailable(catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)

add_executable(mips_emulator_tests
	main.cpp
	
	register_file.cpp
	instruction.cpp

	# Executor
	executor.cpp
	executor/rtype.cpp
	executor/itype.cpp
	executor/jtype.cpp
	executor/special3.cpp
	executor/regimm.cpp
	executor/pcrel.cpp
)

target_link_libraries(mips_emulator_tests
	PRIVATE
		mips_emulator
		Catch2::Catch2
)

catch_discover_tests(mips_emulator_tests)
