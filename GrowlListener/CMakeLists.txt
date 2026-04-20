set(SOURCES 
	GrowlListener.cpp
)

set(HEADERS 
	GrowlListener.h
)

add_executable (GrowlListener ${SOURCES} ${HEADERS})
target_link_libraries(GrowlListener PUBLIC miniaudio)

if(WIN32) 
	target_link_libraries(GrowlListener PUBLIC ws2_32)
endif()
