cmake_minimum_required(VERSION 3.13.5)

project(CppPropertiesExample)

###########################################
# Configure the demo project file generator
###########################################

set(CppPropertiesDemoProjectGeneratorTarget CppPropertiesDemoProjectGenerator)
add_executable(${CppPropertiesDemoProjectGeneratorTarget}
					Generator/Source/CppPropsParser.cpp
					
					Generator/Source/main.cpp)

target_compile_features(${CppPropertiesDemoProjectGeneratorTarget} PUBLIC cxx_std_17)

target_include_directories(${CppPropertiesDemoProjectGeneratorTarget} PRIVATE
							Generator/Include)

add_dependencies(${CppPropertiesDemoProjectGeneratorTarget} ${KodgenTargetLibrary})

target_link_libraries(${CppPropertiesDemoProjectGeneratorTarget} PRIVATE ${KodgenTargetLibrary})

###########################################
# Configure the demo project
###########################################

set(CppPropertiesDemoProjectTarget CppPropertiesDemoProject)
add_executable(${CppPropertiesDemoProjectTarget}
					Source/SomeClass.cpp
					Source/SomeOtherClass.cpp

					Source/main.cpp)

target_compile_features(${CppPropertiesDemoProjectTarget} PUBLIC cxx_std_17)

set(RunGeneratorTarget RunCppPropertiesGenerator)
add_custom_target(${RunGeneratorTarget}
                    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                    COMMAND ${CppPropertiesDemoProjectGeneratorTarget} ${PROJECT_SOURCE_DIR})

# Run the generator BEFORE building CppPropertiesDemoProjectTarget to refresh generated files
add_dependencies(${CppPropertiesDemoProjectTarget} ${RunGeneratorTarget})
add_dependencies(${RunGeneratorTarget} ${CppPropertiesDemoProjectGeneratorTarget})

target_include_directories(${CppPropertiesDemoProjectTarget} PRIVATE
							Include)


add_test(NAME ${CppPropertiesDemoProjectTarget} COMMAND ${CppPropertiesDemoProjectTarget})