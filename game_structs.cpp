#include "game_structs.h"
#include "../inc/ST7735.h"
#include "Sound.h"
#include "gameimages.h"

// --- State ---

//change to size of bitmap -> function to be overwritten
void State::onEnter(Game* game){}

void State::logic(Game* game){}

void State::display(Game* game){
    ST7735_DrawBitmap(0, 0, image, 10, 10);
}

void State::play_sound(Game* game){
    // play the sound at wav_filepath
}


// --- StartState ---
StartState::StartState(){
    //image = START_SCREEN_IMAGE;
}

void StartState::onEnter(Game* game){
    
}

void StartState::logic(Game* game) {
    if (game->button1 == 1) {
        game->current_state = game->states[ATK1];
    }
}

void StartState::display(Game* game){
        ST7735_DrawString(0, 0, "START", ST7735_WHITE);
    //ST7735_DrawBitmap(0, 0, image, 160, 128);
}

void StartState::play_sound(Game* game){
    
}

// --- Atk1State ---
Atk1State::Atk1State(){}

void Atk1State::onEnter(Game* game){
    round_time = 30 * ROUND_TIME;
}

void Atk1State::logic(Game* game) {
    if (round_time == 0) {
        uint8_t direction_went = game->directions();
        if (direction_went != (uint8_t)-1) {
            game->prevAttacks[0] = direction_went;
            game->switchStates(ATK2);
        } else {
            game->switchStates(SWITCH_OFFENCE);
        }
    }
    round_time--;
}

void Atk1State::display(Game* game){
    // if (game->attacker == &(game->p1)) {
    //     image = RED_PUNCHES_BLUE_IMAGE;
    // }
    // else{
    //     image = RED_PUNCHES_BLUE_IMAGE;
    // }
    // ST7735_DrawBitmap(0, 0, image, 160, 128);
    ST7735_DrawString(0, 0, "ATK1", ST7735_WHITE);
}

void Atk1State::play_sound(Game* game){

}

// --- Atk2State ---
Atk2State::Atk2State(){};

void Atk2State::onEnter(Game* game){
    round_time = 30 * ROUND_TIME;
}

void Atk2State::logic(Game* game) {
    if (round_time == 0) {
        uint8_t direction_went = game->directions();
        // Fixed: was transitioning to ATK2 again instead of ATK3
        if (direction_went != (uint8_t)-1 && direction_went != game->prevAttacks[0]) {
            game->prevAttacks[1] = direction_went;
            game->switchStates(ATK3);
        } else {
            game->switchStates(SWITCH_OFFENCE);
        }
    }
    round_time--;
}

void Atk2State::display(Game* game){
    // if (game->attacker == &game->p1) {
    //     image = RED_PUNCHES_BLUE_IMAGE;
    // }
    // else{
    //     image = RED_PUNCHES_BLUE_IMAGE;
    // }
    // ST7735_DrawBitmap(0, 0, image, 160, 128);
        ST7735_DrawString(0, 0, "ATK2", ST7735_WHITE);
}

void Atk2State::play_sound(Game* game){
    
}

// --- Atk3State ---
Atk3State::Atk3State(){};

void Atk3State::onEnter(Game* game){
    round_time = 30 * ROUND_TIME;
}

void Atk3State::logic(Game* game) {
    if (round_time == 0) {
        uint8_t direction_went = game->directions();
        if (direction_went != (uint8_t)-1
            && direction_went != game->prevAttacks[0]
            && direction_went != game->prevAttacks[1]) {
            game->switchStates(WIN);
        } else {
            game->switchStates(SWITCH_OFFENCE);
        }
    }
    round_time--;
}

void Atk3State::display(Game* game){
    // if (game->attacker == &game->p1) {
    //     image = RED_PUNCHES_BLUE_IMAGE;
    // }
    // else{
    //     image = RED_PUNCHES_BLUE_IMAGE;
    // }
    // ST7735_DrawBitmap(0, 0, image, 160, 128);
        ST7735_DrawString(0, 0, "ATK3", ST7735_WHITE);
}

void Atk3State::play_sound(Game* game){
    
}

// --- WinState ---
WinState::WinState(){};

void WinState::onEnter(Game* game){
}

void WinState::logic(Game* game) {
    if (game->button1 == 1) {
        game->switchStates(START);
    }
}

void WinState::display(Game* game){
        ST7735_DrawString(0, 0, "WIN", ST7735_WHITE);
}

void WinState::play_sound(Game* game){
    
}

// --- SwitchOffenceState ---
SwitchOffenceState::SwitchOffenceState(){};

void SwitchOffenceState::onEnter(Game* game){
    state_time = 30 * SWITCH_TIME;
}

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
        game->switchStates(ATK1);
    }
}

void SwitchOffenceState::display(Game* game){
        ST7735_DrawString(0, 0, "SWITCH", ST7735_WHITE);
}

void SwitchOffenceState::play_sound(Game* game){
    
}

// --- Game ---
void Game::init() {
    states[START]         = &start_s;
    states[ATK1]          = &atk1_s;
    states[ATK2]          = &atk2_s;
    states[ATK3]          = &atk3_s;
    states[WIN]           = &win_s;
    states[SWITCH_OFFENCE] = &switch_offence_s;

    current_state = states[ATK1];
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

bool Game::switchStates(int state_index){
    if (state_index < 0 || state_index >= AMT_OF_STATES) {
        return -1;
    }
    current_state = states[state_index];
    current_state->onEnter(this);
    return 1;
}