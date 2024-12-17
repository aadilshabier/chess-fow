# Build networking stack
add_library(dyad dyad.h dyad.c)
set_target_properties(dyad PROPERTIES
  PUBLIC_HEADER dyad.h
  VERSION 0.2.1
  SOVERSION 021
)

set(PROJECT_SOURCE_FILES
  comms.h
  comms_client.h
  board.h
  gamestate.h
  loadingscreen.h
  multiscreen.h
  piece.h
  piecemovement.h
  playscreen.h

  board.c
  comms.c
  comms_client.c
  game.c
  loadingscreen.c
  multiscreen.c
  piecemovement.c
  playscreen.c
)

set(SERVER_SOURCE_FILES
  comms.h
  comms_server.h
  piece.h

  comms.c
  comms_server.c
  server.c
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} raylib dyad)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

add_executable(${SERVER_NAME})
target_sources(${SERVER_NAME} PRIVATE ${SERVER_SOURCE_FILES})
target_link_libraries(${SERVER_NAME} dyad)
