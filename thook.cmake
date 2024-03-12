# sanitizer coverage hook .cmake
function(enable_thook target_name)
	add_library(thook OBJECT thook.cc)
	target_compile_options(thook PRIVATE -fsanitize=address)
  target_compile_options(${target_name} PRIVATE -fsanitize=address -fsanitize-coverage=trace-pc-guard)
  target_link_options(${target_name} PRIVATE -fsanitize=address)
	# link thook.o to target
  target_link_libraries(${target_name} PRIVATE $<TARGET_OBJECTS:thook>) 
endfunction()

# usage:
# include(path/to/thook.cmake)
# enable_sanitizer_coverage(my_executable)