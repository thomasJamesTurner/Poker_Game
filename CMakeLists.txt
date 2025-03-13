cmake_minimum_required(VERSION 3.20)
project(Poker_Game)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 17)

add_executable(Poker_Game
            Headers/cards.h
            Headers/events.h
            Headers/events.h
            Headers/events_base.h
            Headers/player.h
            Headers/table.h
            Src/main.cpp
 "Headers/event_types.h" "Headers/display.h" "Src/display.cpp" "Headers/table.h" )

# ---------- Spritter ----------
add_subdirectory(lib/Spritter)
target_link_libraries(Poker_Game PUBLIC Spritter)
spritter_set_content(Poker_Game ${CMAKE_SOURCE_DIR}/Content ${CMAKE_BINARY_DIR}/Content)