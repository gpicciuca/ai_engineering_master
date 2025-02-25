cmake_minimum_required(VERSION 3.0)

project("CRM App per AI Engineering")

add_executable(crm 
	main.cpp
	app.cpp
	crm.cpp
	database.cpp
	utilities.cpp
)
target_compile_options(crm PUBLIC -std=c++14 -O2)
target_include_directories(crm PUBLIC ./)
