#ifndef GAME_STATE_H_
#define GAME_STATE_H_

typedef enum GameStateType {
	GAME_STATE_LOAD,
	GAME_STATE_PLAY,
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

#endif // GAME_STATE_H_
