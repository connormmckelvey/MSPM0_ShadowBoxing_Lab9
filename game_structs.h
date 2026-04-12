#ifndef GAME_H
#define GAME_H

// Using a proper constant for the array size
#define AMT_OF_STATES 2 

enum StateIndex {
    START = 0,
    OFFENCE_ATK = 1
};

class State {
public:
    const char* state_name;
    unsigned short* image;    // Using unsigned short for uint16
    const char* wav_filepath;
    
    // This stores WHICH Game function this state uses
    void (class Game::*updateState_func)(); 
};

class Player {
    // Player data here
};

class Game {
private:
    State states[AMT_OF_STATES];
    

public:
    bool playing;
    State* current_state; // Pointer to the active state in the array
    int prevAttacks[2];
    Player* attacker;
    Player p1;
    Player p2;

    void init();
    void updateState(); // The main loop calls this

    // The actual logic functions
    void start_SU();
    void offenceAtk_SU();
};

#endif