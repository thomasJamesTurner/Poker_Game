cmake_minimum_required(VERSION 3.20)
project(Poker_Game)

set(CMAKE_CXX_STANDARD 17)

add_executable(Poker_Game
            Headers/cards.h
            Headers/events.h
            Headers/events.h
            Headers/events_base.h
            Headers/player.h
            Headers/table.h
            Src/main.cpp
)

# ---------- Spritter ----------
add_subdirectory(lib/Spritter)
target_link_libraries(Poker_Game PUBLIC Spritter)
#spritter_set_content(Poker_Game ${CMAKE_SOURCE_DIR}/MyContentDir ${CMAKE_BINARY_DIR}/MyContentDir)