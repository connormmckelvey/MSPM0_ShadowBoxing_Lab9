#ifndef GAME_H
#define GAME_H

#include <cstdint>

#define ROUND_TIME 5 // in secs

enum StateIndex {
    START = 0,
    ATK1,
    ATK2,
    ATK3,
    WIN,
    SWITCH_OFFENCE,
    AMT_OF_STATES
};

// Forward declaration — breaks the circular dependency.
// State methods can accept Game* without Game being fully defined yet.
class Game;

class State {
public:
    unsigned short* image;
    char* wav_filepath;

    virtual void logic(Game* game);
    virtual void display(Game* game);
    virtual void play_sound(Game* game);
};

class StartState : public State {
public:
    StartState();
    void logic(Game* game) override;
    void display(Game* game) override;
    void play_sound(Game* game) override;
};

class Atk1State : public State {
private:
    int round_time = ROUND_TIME * 30;
public:
    Atk1State();
    void logic(Game* game) override;
    void display(Game* game) override;
    void play_sound(Game* game) override;};

class Atk2State : public State {
private:
    int round_time = ROUND_TIME * 30;
public:
Atk2State();
    void logic(Game* game) override;
};

class Atk3State : public State {
private:
    int round_time = ROUND_TIME * 30;
public:
    Atk3State();
    void logic(Game* game) override;
};

class WinState : public State {
public:
    Atk1State();
    void logic(Game* game) override;
};

class SwitchOffenceState : public State {
private:
    int state_time = 30 * 2;
public:
    SwitchOffenceState();
    void logic(Game* game) override;
};

class Player {
    // Player data here
};

class Game {
private:
    StartState start_s;
    Atk1State atk1_s;
    Atk2State atk2_s;
    Atk3State atk3_s;
    WinState win_s;
    SwitchOffenceState switch_offence_s;
public:
    State* states[AMT_OF_STATES];
    bool playing;
    int button1 = 0;
    State* current_state;
    uint8_t prevAttacks[2];
    Player* attacker;
    Player p1;
    Player p2;

    void init();
    void get_inputs();
    void updateState();
    uint8_t directions();
};

#endif