#ifndef GAME_STATE_H_
#define GAME_STATE_H_

typedef enum GameStateType {
	GAME_STATE_LOAD,
	GAME_STATE_PLAY,
	GAME_STATE_MULTI_MENU,
	NUM_GAME_STATES,
} GameStateType;

typedef struct GameState {
	void (*init)(void*);
	void (*update)(void);
	void (*draw)(void);
	void (*exit)(void);
} GameState;

extern GameState *currentState;
extern GameState *gameStates[];

// implemented in game.c
void replaceGameState(GameStateType new, void *arg);

#endif // GAME_STATE_H_
