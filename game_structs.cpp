#include "game_structs.h"
#include "../inc/ST7735.h"
#include "Sound.h"
#include "gameimages.c"
#include "../inc/LaunchPad.h"
#include "connors_display_helper.h"

// --- State ---

//change to size of bitmap -> function to be overwritten
void State::onEnter(Game* game){
    first_display = true;
}

void State::logic(Game* game){}

void State::display(Game* game){
    if (first_display) {
        ST7735_FillScreen(ST7735_GREEN);
        first_display = false;
    }
    ST7735_DrawBitmap(0, 0, image, 0, 10);
}

void State::play_sound(Game* game){
    // play the sound at wav_filepath
}


// --- StartState ---
StartState::StartState(){
    image = cover_splash[0];
    image_width = COVER_SPLASH_FRAME_WIDTH;
    image_height = COVER_SPLASH_FRAME_HEIGHT;
}

void StartState::onEnter(Game* game){
    first_display = true;
}

void StartState::logic(Game* game) {
    if (game->button1 == 1) {
        game->language = ENGLISH;
        game->switchStates(ATK1);
    }
    if (game->button2 == 1) {
        game->language = SPANISH;
        game->switchStates(ATK1);
    }
}

void StartState::display(Game* game){
    if (first_display) {
        ST7735_FillScreen(ST7735_BLACK);
        first_display = false;
    }
    // ST7735_DrawPixel(1,1,ST7735_RED);      //top left where pin 8
    // ST7735_DrawPixel(100,1,ST7735_YELLOW); //top right (where 16 is on screen)
    // ST7735_DrawPixel(100,100,ST7735_BLUE); // bottom right (where pin 1 is on the 16 side)
    // ST7735_DrawPixel(1,100,ST7735_GREEN);  // bottom left
    CDH_DrawBitmap2x(-10, 100, image, image_width, image_height);
    ST7735_DrawString(0, 11, "Press S1 for English", ST7735_WHITE);
    ST7735_DrawString(0, 12, "Press S2 for Espanol", ST7735_WHITE);
}

void StartState::play_sound(Game* game){
    
}

// --- Atk1State ---
Atk1State::Atk1State(){}

void Atk1State::onEnter(Game* game){
    first_display = true;
    round_time = 30 * ROUND_TIME;
    game->prevAttackIndex = 0;
    if (game->attacker == &(game->p1)) {
        image = valvano[0];
        image_height = VALVANO_FRAME_HEIGHT;
        image_width = VALVANO_FRAME_WIDTH;
    }
    else{
        image = yerballi[0];
        image_height = YERBALLI_FRAME_HEIGHT;
        image_width = YERBALLI_FRAME_WIDTH;    
    }
}

void Atk1State::logic(Game* game) {
    if (round_time == 0) {
        uint8_t direction_went = game->directions();
        game->prevAttacks[0] = direction_went;
        if (direction_went != (uint8_t)-1) {
            game->switchStates(ROUND_FEEDBACK);
        } else {
            game->switchStates(SWITCH_OFFENCE);
        }
    }
    round_time--;
}

void Atk1State::display(Game* game){
    if (first_display) {
        ST7735_FillScreen(0x8BE0);
        ST7735_DrawBitmap(0,BOXING_RING_BG_FRAME_HEIGHT - 5,boxing_ring_bg[0],BOXING_RING_BG_FRAME_WIDTH,BOXING_RING_BG_FRAME_HEIGHT);
        first_display = false;
    }
    CDH_DrawBitmapTransparent(30, 115, image, image_width, image_height);
    if(game->language == ENGLISH){
        CDH_DrawString(0, 12, "Combo: 0", ST7735_BLACK, 0x8BE0, 1);
        CDH_OutUDec(18, 7, round_time/30, ST7735_BLACK, 0x8BE0, 4);
    }
    else{
        CDH_DrawString(0, 12, "ATK1", ST7735_BLACK, 0x8BE0, 1);
        CDH_OutUDec(18, 7, round_time/30, ST7735_BLACK, 0x8BE0, 4);
    }

}

void Atk1State::play_sound(Game* game){

}

// --- Atk2State ---
Atk2State::Atk2State(){};

void Atk2State::onEnter(Game* game){
    first_display = true;
    round_time = 30 * ROUND_TIME;
    game->prevAttackIndex = 1;
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
    if (first_display) {
        ST7735_FillScreen(ST7735_GREEN);
        first_display = false;
    }
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
    first_display = true;
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
    if (first_display) {
        ST7735_FillScreen(ST7735_GREEN);
        first_display = false;
    }
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
    first_display = true;
}

void WinState::logic(Game* game) {
    if (game->button1 == 1) {
        game->switchStates(START);
    }
}

void WinState::display(Game* game){
    if (first_display) {
        ST7735_FillScreen(ST7735_GREEN);
        first_display = false;
    }
        ST7735_DrawString(0, 0, "WIN", ST7735_WHITE);
}

void WinState::play_sound(Game* game){
    
}

// --- SwitchOffenceState ---
SwitchOffenceState::SwitchOffenceState(){};

void SwitchOffenceState::onEnter(Game* game){
    first_display = true;
    state_time = 30 * SWITCH_TIME;
    // if yerballi atking valano
    if (game->attacker == &(game->p1)) {
        image = valvano[0];
        image_height = VALVANO_FRAME_HEIGHT;
        image_width = VALVANO_FRAME_WIDTH;
    }
    else{
        image = yerballi[0];
        image_height = YERBALLI_FRAME_HEIGHT;
        image_width = YERBALLI_FRAME_WIDTH;    
    }
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
    if (first_display) {
        ST7735_FillScreen(0x8BE0);
        ST7735_DrawBitmap(0,BOXING_RING_BG_FRAME_HEIGHT - 5,boxing_ring_bg[0],BOXING_RING_BG_FRAME_WIDTH,BOXING_RING_BG_FRAME_HEIGHT);
        first_display = false;
    }
    CDH_DrawBitmapTransparent(30, 115, image, image_width, image_height);
    if(game->language == ENGLISH){
        CDH_DrawString(0, 12, "Blocked!", ST7735_BLACK, 0x8BE0, 1);
        CDH_DrawString(14, 7, "!", ST7735_BLACK, 0x8BE0,3);
    }
    else{
        CDH_DrawString(0, 12, "ATK1", ST7735_BLACK, 0x8BE0, 1);
    }
}

void SwitchOffenceState::play_sound(Game* game){
    
}

// --- RoundFeedback ---
RoundFeedbackState::RoundFeedbackState(){};

void RoundFeedbackState::onEnter(Game* game){
    first_display = true;
    state_time = 30 * SWITCH_TIME;
    // if yerballi atking valano
    if (game->attacker == &(game->p1)) {
        // if last attack went up
        if (game->prevAttacks[game->prevAttackIndex] == RIGHT) {
            image = valvano[1];
        }
        else if (game->prevAttacks[game->prevAttackIndex] == LEFT) {
            image = valvano[2];
        }
        // no punch
        else{
            image = valvano[0];
        }
        image_height = VALVANO_FRAME_HEIGHT;
        image_width = VALVANO_FRAME_WIDTH;
    }
    else{
        // if last attack went up
        if (game->prevAttacks[game->prevAttackIndex] == RIGHT) {
            image = yerballi[1];
        }
        else if (game->prevAttacks[game->prevAttackIndex] == LEFT) {
            image = yerballi[2];
        }
        // no punch
        else{
            image = yerballi[0];
        }
        image_height = YERBALLI_FRAME_HEIGHT;
        image_width = YERBALLI_FRAME_WIDTH;    
    }
}

// waits state_time and then goes to next attack state
void RoundFeedbackState::logic(Game* game) {
    if (state_time != 0) {
        state_time--;
    } else {
        if (game->prevAttackIndex == 0) {
            game->switchStates(ATK2);
        }
        else if (game->prevAttackIndex == 1) {
            game->switchStates(ATK3);
        }
    }
}

void RoundFeedbackState::display(Game* game){
    if (first_display) {
        ST7735_FillScreen(0x8BE0);
        ST7735_DrawBitmap(0,BOXING_RING_BG_FRAME_HEIGHT - 5,boxing_ring_bg[0],BOXING_RING_BG_FRAME_WIDTH,BOXING_RING_BG_FRAME_HEIGHT);
        first_display = false;
    }
    CDH_DrawBitmapTransparent(30, 115, image, image_width, image_height);
    if(game->language == ENGLISH){
        CDH_DrawString(0, 12, "Prepare for Next Round!", ST7735_BLACK, 0x8BE0, 1);
        CDH_DrawString(12, 7, "AH!", ST7735_BLACK, 0x8BE0,3);
    }
    else{
        CDH_DrawString(0, 12, "ATK1", ST7735_BLACK, 0x8BE0, 1);
    }
}

void RoundFeedbackState::play_sound(Game* game){
    
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

    IOMUX->SECCFG.PINCM[PB21INDEX] = 0x00040081;
    IOMUX->SECCFG.PINCM[PB18INDEX] = 0x00040081;
}

//gets input from various hardware and stores in member vars in the Game class
void Game::get_inputs() {
    button1 = ((GPIOA->DIN31_0 >> 18) & 0x1);
    button2 = !((GPIOB->DIN31_0 >> 21) & 0x1);
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