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

# TODO: Remove this once Spritter actually works properly
add_custom_command(
        TARGET Poker_Game POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:Poker_Game>
        VERBATIM
)