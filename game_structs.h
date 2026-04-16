#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"

#define ROUND_TIME 5 // in secs
#define SWITCH_TIME 5

#define ENGLISH 0
#define SPANISH 1

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
    const uint16_t* image;
    int image_width;
    int image_height;
    char* wav_filepath;
    bool first_display = true;

    virtual void onEnter(Game* game);
    virtual void logic(Game* game);
    virtual void display(Game* game);
    virtual void play_sound(Game* game);
};

class StartState : public State {
public:
    StartState();
    void onEnter(Game* game) override;
    void logic(Game* game) override;
    void display(Game* game) override;
    void play_sound(Game* game) override;
};

class Atk1State : public State {
private:
    int round_time = ROUND_TIME * 30;
public:
    Atk1State();
    void onEnter(Game* game) override;
    void logic(Game* game) override;
    void display(Game* game) override;
    void play_sound(Game* game) override;};

class Atk2State : public State {
private:
    int round_time = ROUND_TIME * 30;
public:
Atk2State();
void onEnter(Game* game) override;
    void logic(Game* game) override;
    void display(Game* game) override;
    void play_sound(Game* game) override;
};

class Atk3State : public State {
private:
    int round_time = ROUND_TIME * 30;
public:
    Atk3State();
    void onEnter(Game* game) override;
    void logic(Game* game) override;
    void display(Game* game) override;
    void play_sound(Game* game) override;
};

class WinState : public State {
public:
    WinState();
    void onEnter(Game* game) override;
    void logic(Game* game) override;
    void display(Game* game) override;
    void play_sound(Game* game) override;
};

class SwitchOffenceState : public State {
private:
    int state_time = 30 * SWITCH_TIME;
public:
    SwitchOffenceState();
    void onEnter(Game* game) override;
    void logic(Game* game) override;
    void display(Game* game) override;
    void play_sound(Game* game) override;
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
    int language; // 0 for english, 1 for spanish
    State* states[AMT_OF_STATES];
    bool playing;
    int button1 = 0;
    int button2 = 0;
    State* current_state;
    uint8_t prevAttacks[2];
    Player* attacker;
    Player p1;
    Player p2;

    void init();
    void get_inputs();
    void updateState();
    uint8_t directions();
    bool switchStates(int state_index);
};

#endif