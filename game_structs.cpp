#include "game_structs.h"
#include "ST7735"
#include "Sound.h"

// --- State ---

//change to size of bitmap -> function to be overwritten
void display(Game* game){
    ST7735_DrawBitmap(0, 0, image, 10, 10)
}

void play_sound(Game* game){
    // play the sound at wav_filepath
}


// --- StartState ---
void StartState::logic(Game* game) {
    if (game->button1 == 1) {
        game->current_state = game->states[ATK1];
    }
}

// --- Atk1State ---
void Atk1State::logic(Game* game) {
    if (round_time == 0) {
        uint8_t direction_went = game->directions();
        if (direction_went != (uint8_t)-1) {
            game->prevAttacks[0] = direction_went;
            round_time = ROUND_TIME * 30;
            game->current_state = game->states[ATK2];
        } else {
            round_time = ROUND_TIME * 30;
            game->current_state = game->states[SWITCH_OFFENCE];
        }
    }
    round_time--;
}

// --- Atk2State ---
void Atk2State::logic(Game* game) {
    if (round_time == 0) {
        uint8_t direction_went = game->directions();
        // Fixed: was transitioning to ATK2 again instead of ATK3
        if (direction_went != (uint8_t)-1 && direction_went != game->prevAttacks[0]) {
            game->prevAttacks[1] = direction_went;
            round_time = ROUND_TIME * 30;
            game->current_state = game->states[ATK3];
        } else {
            round_time = ROUND_TIME * 30;
            game->current_state = game->states[SWITCH_OFFENCE];
        }
    }
    round_time--;
}

// --- Atk3State ---
void Atk3State::logic(Game* game) {
    if (round_time == 0) {
        uint8_t direction_went = game->directions();
        if (direction_went != (uint8_t)-1
            && direction_went != game->prevAttacks[0]
            && direction_went != game->prevAttacks[1]) {
            round_time = ROUND_TIME * 30;
            game->current_state = game->states[WIN];
        } else {
            round_time = ROUND_TIME * 30;
            game->current_state = game->states[SWITCH_OFFENCE];
        }
    }
    round_time--;
}

// --- WinState ---
void WinState::logic(Game* game) {
    if (game->button1 == 1) {
        game->current_state = game->states[START];
    }
}

// --- SwitchOffenceState ---
void SwitchOffenceState::logic(Game* game) {
    if (state_time != 0) {
        state_time--;
    } else {
        if (game->attacker == &game->p1) {
            game->attacker = &game->p2;
        } else {
            game->attacker = &game->p1;
        }
        game->prevAttacks[0] = 0;
        game->prevAttacks[1] = 0;
        state_time = 30 * 2;
        game->current_state = game->states[ATK1];
    }
}

// --- Game ---
void Game::init() {
    states[START]         = &start_s;
    states[ATK1]          = &atk1_s;
    states[ATK2]          = &atk2_s;
    states[ATK3]          = &atk3_s;
    states[WIN]           = &win_s;
    states[SWITCH_OFFENCE] = &switch_offence_s;

    current_state = states[START];
    attacker = &p1;
}

//gets input from various hardware and stores in member vars in the Game class
void Game::get_inputs() {
}

void Game::updateState() {
    current_state->logic(this);
}

//takes member vars and computes if both went the same direction, if they did it returns 0,1,2,3 if they didnt it returns -1
uint8_t Game::directions(){
    return -1;
}