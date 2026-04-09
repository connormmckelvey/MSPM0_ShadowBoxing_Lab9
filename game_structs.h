#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

class State{}

class Player{}

class Game{
    public:
        State state;
        int[2] prevAttacks;
        Player* attacker;
        Player p1;
        Player p2;
}