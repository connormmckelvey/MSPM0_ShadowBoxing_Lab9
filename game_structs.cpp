#include "game_structs.h"

void State::State(char* name){
    this.state_name = name;
}

void Game::init() {
    playing = true;

    // Setup START State
    states[START].state_name = "Start Menu";
    states[START].wav_filepath = "sounds/menu.wav";
    states[START].updateState_func = &Game::startLogic; // Point to the member function
    
    // Setup OFFENCE_ATK State
    states[OFFENCE_ATK].state_name = "Attacking";
    states[OFFENCE_ATK].wav_filepath = "sounds/slash.wav";
    states[OFFENCE_ATK].updateState_func = &Game::offenceAtkLogic;

    // Start the game at the START state
    current_state = &states[START];
}

void Game::startLogic() {
    // Logic for button press to start
    // if (buttonPressed) transition to OFFENCE_ATK
}

void Game::offenceAtkLogic() {
    // Logic for attacking
}

void Game::updateState() {
    // Execute the member function pointer of the current state
    // Syntax: (instancePointer->*functionPointer)()
    if (current_state != 0 && current_state->behavior != 0) {
        (this->*(current_state->behavior))();
    }
}