#Documentation: 
#https://docs.espressif.com/projects/esp-idf/en/v3.2/api-guides/build-system-cmake.html 

set(COMPONENT_SRCS "Main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "."  )

#set(INCLUDE_DIRS "."  )

#set(COMPONENT_ADD_INCLUDEDIRS "." )
#set(LIBRARY_SRCS "." )

set(COMPONENT_SRCDIRS "." 
                      "src"   
                      "src/Helpers"  
                      "lib/Timer" ) 

register_component()

