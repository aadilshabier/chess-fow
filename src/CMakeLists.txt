set(PROJECT_SOURCE_FILES
  board.h
  gamestate.h
  loadingscreen.h
  piecemovement.h
  playscreen.h
  board.c
  game.c
  loadingscreen.c
  piecemovement.c
  playscreen.c
)

set(SERVER_SOURCE_FILES
  server.c
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} raylib)

add_executable(${SERVER_NAME})
target_sources(${SERVER_NAME} PRIVATE ${SERVER_SOURCE_FILES})
