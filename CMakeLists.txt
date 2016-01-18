cmake_minimum_required(VERSION 3.0)
project(subsimulation)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)
# find_package(Bullet REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(subsimulation
	main.cxx
	simulation.cxx
)

target_include_directories(subsimulation PUBLIC
# 	${PROJECT_SOURCE_DIR}/lib/
	${EIGEN3_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
)
target_link_libraries(subsimulation
	${SDL2_LIBRARIES}
	${OPENGL_LIBRARIES}
# 	${BULLET_MATH_LIBRARY}
# 	${BULLET_COLLISION_LIBRARY}
# 	${BULLET_DYNAMICS_LIBRARY}
)
target_compile_definitions(subsimulation PUBLIC
	EIGEN_DONT_ALIGN
)
target_compile_options(subsimulation PRIVATE
	-Wno-deprecated-declarations
)
target_compile_features(subsimulation PUBLIC
	cxx_uniform_initialization
)

install(TARGETS subsimulation RUNTIME DESTINATION bin)
