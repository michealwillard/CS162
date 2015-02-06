//
//  Program:  mew_fp.cpp
//  Assignment:  Final Project
//  Author:  Micheal Willard
//  Created on: 5/16/14
//  Description:  A turn-based-combat adventure game set in a fantastical land of Wizards and whatnot.
//  Input:  Starts with player selction of class and name, then varies with function prompts.
//  Output:  Running updates on temporary stats, displays, and ultimately if the user won or lost.
//  Notes:  N/A, see code comments and final project report.
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
using namespace std;
//Struct Prototypes (character classes)
struct paladin{
    static const int damage_min = 10;
    static const int damage_max = 60;
    static const int hit_points = 200;
    static const int cast_damage_min = 25;
    static const int cast_damage_max = 75;
};
struct warrior{
    static const int damage_min = 25;
    static const int damage_max = 100;
    static const int hit_points = 150;
    static const int cast_damage_min = 10;
    static const int cast_damage_max = 25;
};
struct mage{
    static const int damage_min = 10;
    static const int damage_max = 30;
    static const int hit_points = 100;
    static const int cast_damage_min = 100;
    static const int cast_damage_max = 150;
};
//Class Declarations
class playable_character{
public:
    int m_health_pots;
    int m_strength_pots;
    int m_gold;
    playable_character()
    {
        m_health_pots = 0;
        m_strength_pots = 0;
        m_gold = 100;
    }
    void choose_character(paladin *p, warrior *w, mage *m);
    int character_stats[5];  //class stats = {damage_min, damage_max, hit_points, cast_min, cast_max}
    char m_name[20];
};
class locations{
public:
    static void display_goldshire();
    static void display_goldshire_text(char (&name)[20]);
    static void goldshire_actions(int &gold, int &h_pots, int &s_pots);
    static void leaving_goldshire();
};
class vendor{
public:
    static void display_shop(int &gold, int &h_pots, int &s_pots);
    static void purchase_strength(int &gold, int &s_pots);
    static void purchase_health(int &gold, int &h_pots);
};

class combat{
public:
    static void combat_engine(int (&class_stats)[5], int (&enemy_stats)[3], string &enemy_name, int &h_pots, int &s_pots, void (*taunt)(void));
};
class wizard_boss{
public:
    string name;
    static void display_wizard();
    void wizard_attack();
    static void wizard_taunt();
    int wizard_boss_stats[3];
    //{20, 30, 350} = var[3];
};
class gatehouse_knights{
public:
    string name;
    static void display_knights();
    void knight_attack();
    static void knight_taunt();
    int gatehouse_knights_stats[3];
};

//Function Prototypes
void combat_hud(int &class_temp_hp, int &class_max_hp, int &enemy_temp_hp, const int &enemy_max_hp, int &h_pots, int &s_pots);
void game_menu();

int main()
{
    srand ((unsigned)time(NULL));
    char play_again = 'y';
    while (play_again != 'n' && play_again != 'N')
    {
        //DECLARE VARIABLES
        playable_character new_player;
        paladin p;
        warrior w;
        mage m;
        wizard_boss new_wizard;
        gatehouse_knights new_knights;
        new_wizard.name = "Evil Wizard";
        new_wizard.wizard_boss_stats[0] = 20;  //cheating here because of my initialization problem
        new_wizard.wizard_boss_stats[1] = 30;
        new_wizard.wizard_boss_stats[2] = 350;
        //new_player.m_health_pots = 0;
        //new_player.m_strength_pots = 0;
        //new_player.m_gold = 100;
        new_knights.name = "Gatehouse Knights";
        new_knights.gatehouse_knights_stats[0] = 15;
        new_knights.gatehouse_knights_stats[1] = 25;
        new_knights.gatehouse_knights_stats[2] = 100;
        //Had to initialize down here, I couldn't figure out the constructors as I didn't catch my initialization problem until Sunday, of course. Xcode is running C++11 which I guess allows initialization in the class
        
    game_menu();
    new_player.choose_character(&p, &w, &m);
    
    locations::display_goldshire();
    locations::display_goldshire_text(new_player.m_name);
    locations::goldshire_actions(new_player.m_gold, new_player.m_health_pots, new_player.m_strength_pots);
        gatehouse_knights::display_knights();
    combat::combat_engine(new_player.character_stats, new_knights.gatehouse_knights_stats, new_knights.name, new_player.m_health_pots, new_player.m_strength_pots, gatehouse_knights::knight_taunt);
    wizard_boss::display_wizard();
    combat::combat_engine(new_player.character_stats, new_wizard.wizard_boss_stats, new_wizard.name, new_player.m_health_pots, new_player.m_strength_pots, wizard_boss::wizard_taunt);
    //Game ends when the wizard is defeated.
        
        cout << "Would you like to play again? (y/n)" << endl;
        cin >> play_again;
    }
    cout << "Good bye, for now.\nMay many more adventures await you." << endl;
    return 0;
}
//Purpose:  To enable a combat encounter between the PC and a mob defined as a parameter.
//Entry:  Called from main with a boss defined as a parameter, other parameters are basic to the PC.
//Exit:  When the PC or the mob has lost all it's hit points and has been defeated.
void combat::combat_engine(int (&class_stats)[5], int (&enemy_stats)[3], string &enemy_name, int &h_pots, int &s_pots, void (*taunt)(void)) //wizard or gatehouse mob
{
    //Declare all the Temp Variables
    int turn1 = 0, turn2 = 0;
    int min = class_stats[0];
    int max = class_stats[1];
    int c_min = class_stats[3];
    int c_max = class_stats[4];
    int char_hp = class_stats[2]; //need to init at main so that health doens't reset at combat.
    int enemy_min = enemy_stats[0];
    int enemy_max = enemy_stats[1];
    int damage_done = 0;
    int enemy_hp = enemy_stats[2];
    const int enemy_max_hp_const = enemy_stats[2];
    int attack_modifier = 0;  //used with strength potion, increases melee damage by 25% for duration of combat.
    char action;
    while(char_hp > 0 && enemy_hp > 0)
    {
        //Enemy attacks
        if (turn2 < turn1) {
            combat_hud(char_hp, class_stats[2], enemy_hp, enemy_max_hp_const, h_pots, s_pots);
            cout << "The " << enemy_name << " attacks you." << endl;
            damage_done = rand() % (enemy_max - enemy_min) + enemy_min;
            //percent chance to block, or block_min block_max, extra functionality to be built in
            cout << "* You have been hit for " << damage_done << " points of damage. *\n" << endl;
            char_hp = (char_hp) - (damage_done);
            turn2++;
            //cout << turn1 << turn2; //an example of error checking
        }
        else {
            //Player attacks
            combat_hud(char_hp, class_stats[2], enemy_hp, enemy_max_hp_const, h_pots, s_pots);
            cout << "Choose your action:  ";
            cin >> action;
            while(action != 'm' && action != 'M' && action != 'c' && action != 'C' && action != 's' && action != 'S' && action != 'h' && action != 'H'){
                cin.clear();  //clears failbit in cin
                cin.ignore(256,'\n');
                while(!isspace(cin.get()))
                {
                    continue; //gets rid of bad input
                }
                cout << "Please enter a valid option: ";
                cin >> action;
            }
            if(action == 'm' || action =='M')
            {
                damage_done = rand() % (max - min) + min;
                damage_done+=(damage_done * attack_modifier);
                cout << "* You have dealt " << damage_done << " points of damage to the enemy. *" << endl;
                enemy_hp = (enemy_hp) - (damage_done);
                turn1++;
            }
            if(action == 'c' || action =='C')
            {
                damage_done = rand() % (c_max - c_min) + c_min;
                cout << "* You have dealt " << damage_done << " points of damage to the enemy. *" << endl;
                enemy_hp = (enemy_hp) - (damage_done);
                turn1++;
            }
            if (action == 's' || action == 'S') {
                if (s_pots <= 0) {
                    cout << "You are out of strength potions." << endl;
                }
                else{
                s_pots = s_pots - 1;
                attack_modifier = .25;
                }
            }
            if (action == 'h' || action == 'H') {
                if (h_pots <= 0) {
                    cout << "You are out of health potions." << endl;
                }
                else{
                h_pots = h_pots - 1;
                char_hp = char_hp + 50;
                }
            }
            if (enemy_hp > 0) {
                (*taunt)();
            }
            if (enemy_hp <= 0) {
                cout << "'NOOOOOO!'" << endl;
            }
            //cout << turn1 << turn2;
        }
    }
    if (enemy_hp <= 0) {
        cout << "* You have defeated the " << enemy_name << ". *" << endl;
    }
    if (char_hp <= 0) {
        cout << "Your journey has come to an end.  You are dead." << endl;
    }
}
//Purpose:  To display the game play Head's Up Display, with vital stats.
//Entry:  Called from combat engine, using temp stats.
//Exit:  None, just a cout << function.
void combat_hud(int &class_temp_hp, int &class_max_hp, int &enemy_temp_hp, const int &enemy_max_hp, int &h_pots, int &s_pots)
{
    //const int enemy_max_hp_display = enemy_temp_hp;
    cout << "+--------------------------------------------------------------------------+" << endl;
    cout << "|                              Enemy HP: " << enemy_temp_hp << "/" << enemy_max_hp << "                           " << endl;
    cout << "|            HP: " << class_temp_hp << "/" << class_max_hp << "  Strength Potions: " << s_pots << "  Health Potions: " << h_pots << endl;
    cout << "|  Choose an Action:  (m)elee  (c)ast  (s)trength potion  (h)ealth Potion  " << endl;
    cout << "+--------------------------------------------------------------------------+" << endl;
}
//Purpose:  To welcome the user and give them an indication of the screen size they need.
//Entry:  None, called from main to start game.
//Exit:  None, just a cout << function.
void game_menu()
{
    cout << "|  Welcome to Robes and Wizard Hats.  Please adjust your screen as necessary.  |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|______________________________________________________________________________|" << endl;
}
//Purpose:  Called right after the welcome screen, to allow the user to input their class choice and name.
//Entry:  None, called from main to start game.
//Exit:  When a class and name have been selected.
void playable_character::choose_character(paladin *p, warrior *w, mage *m)  //fix this to grab stats from struct
{
    char character;
    cout << "Choose your class:" << endl;
    cout << "|  Warrior" << "  |  " << "Paladin" << "  |  " << " Mage  " << "  |  " << endl; //mage
    cout << "|    (w)  " << "  |  " << "  (p)  " << "  |  " << "  (m)  " << "  |  " << endl; //(m)
    cout << "|___________|___________|___________|" << endl;
    cin >> character;
    while(character != 'w' && character != 'W' && character != 'p' && character != 'P' && character != 'm' && character != 'M'){
        cin.clear();  //clears failbit in cin
        cin.ignore(256,'\n');
        while(!isspace(cin.get()))
        {
            continue; //gets rid of bad input
        }
        cout << "Please enter a valid option: ";
        cin >> character;
    }
    if (character == 'p' || character =='P')
    {
        character_stats[0] = p->damage_min;
        character_stats[1] = p->damage_max;
        character_stats[2] = p->hit_points;
        character_stats[3] = p->cast_damage_min;
        character_stats[4] = p->cast_damage_max;
    }
    if (character == 'w' || character == 'W')
    {
        for(int i = 0; i < 5; i++)
        {
            character_stats[0] = w->damage_min;
            character_stats[1] = w->damage_max;
            character_stats[2] = w->hit_points;
            character_stats[3] = w->cast_damage_min;
            character_stats[4] = w->cast_damage_max;
        }
    }
    if (character == 'm' || character == 'M')
    {
        for(int i = 0; i < 5; i++)
        {
            character_stats[0] = m->damage_min;
            character_stats[1] = m->damage_max;
            character_stats[2] = m->hit_points;
            character_stats[3] = m->cast_damage_min;
            character_stats[4] = m->cast_damage_max;
        }
    }
    cout << "Enter your character's name: ";
    cin.clear();  //clears failbit in cin
    cin.ignore(256,'\n');
    cin.getline(m_name, 20);
    //cout << m_name << character_stats[0] << character_stats[1] << character_stats[2] << character_stats[3] << character_stats[4] << endl;
}
//Purpose:  To set the location of the game at the main town of Goldshire.
//Entry:  None, called from main to start game.
//Exit:  None, just a cout << function.
void locations::display_goldshire()
{
    cout << "                                  _                                        " << endl;
    cout << "                        .-.      / \\        _                             " << endl;
    cout << "            ^^         /   \\    /^./\\__   _/ \\                          " << endl;
    cout << "          _        .--'\\/\\_ \\__/.      \\ /    \\  ^^  ___              " << endl;
    cout << "         / \\_    _/ ^      \\/  __  :'   /\\/\\  /\\  __/   \\            " << endl;
    cout << "        /    \\  /    .'   _/  /  \\   ^ /    \\/  \\/ .`'\\_/\\           " << endl;
    cout << "       /\\/\\  /\\/ :' __  ^/  ^/    `--./.'  ^  `-.\\ _    _:\\ _         " << endl;
    cout << "      /    \\/  \\  _/  \\-' __/.' ^ _   \\_   .'\\   _/ \\ .  __/ \\      " << endl;
    cout << "    /\\  .-   `. \\/     \\ / -.   _/ \\ -. `_/   \\ /    `._/  ^  \\      " << endl;
    cout << "   /  `-.__ ^   / .-'.--'    . /  __`--./ .-'  `-.  `-. `.  -  `.          " << endl;
    cout << " ^/        `.  / /      `-.   /  /HH\\  / .   .'   \\    \\  \\  .-  \\    " << endl;
    cout << " / #~         /   /H\\           /=‘  \\       —_—-_—-_—            \\     " << endl;
    cout << "/ ~       _______/| |\\         /______\\     // \\/ \\/ \\\\            \\" << endl;
    cout << " /~/     |######/-+-+-\\        \\XXXXXX/     ||o||o||o||                  " << endl;
    cout << "/  /     |######^ |^| ^        |= []- |     ||/||/||/||                    " << endl;
    cout << "          |/^\\=======||        |#-    |     ===========                   " << endl;
    cout << "          ||^|_|__|[]||        |=-|^| |                                    " << endl;
    cout << "                               |+_|_|_|                                    " << endl;
}
//Purpose:  To welcom the character to Goldshire, the first time they arrive.
//Entry:  Called with the user's name as a parameter.
//Exit:  None, just a cout << function.
void locations::display_goldshire_text(char (&name)[20])
{
    cout << "Welcome to Goldshire, " << name << ".  A once bustling and beautiful mining town nestled in the shadow of the imposing Hawksridge Mountains.  You notice those times have changed, and a dark cloud hangs over the town and it's people." << endl;
    cout << "Perhaps you should do some exploring to find what mysterious fate has befallen this town.\n" << endl;

}
void locations::goldshire_actions(int &gold, int &h_pots, int &s_pots)
{
    char action;
    cout << "Do you wish to: \n (l)eave Goldshire and journey to the Hawksridge Mountains. \n (v)isit the vendor in town." << endl;
    cin >> action;
    while(action != 'l' && action != 'L' && action != 'v' && action != 'V'){
        cin.clear();  //clears failbit in cin
        cin.ignore(256,'\n');
        while(!isspace(cin.get()))
        {
            continue; //gets rid of bad input
        }
        cout << "Please enter a valid option: ";
        cin >> action;
    }
    if (action == 'l' || action == 'L') {
        leaving_goldshire();
    }
    if (action == 'v' || action == 'V') {
        vendor::display_shop(gold, h_pots, s_pots);
    }
}
//Purpose:  To inform the player they are leaving goldshire and heading to the mountains.
//Entry:  None, called from main to start game.
//Exit:  None, just a cout << function.
void locations::leaving_goldshire()
{
    cout << " ____________________________________________________________________________________" << endl;
    cout << "You have decided to leave Goldshire.  The vendor you spoke with mentioned a Wizard, residing in the Hawksridge Mountains, who has caused much suffering to the townsfolk.  So you leave and head in that direction.\n\n" << endl;
    cout << " ____________________________________________________________________________________" << endl;
}
//Purpose:  To swelcome the player to the shop and initiated the vendor purchasing options
//Entry:  Called from main with the three parameters (Gold, and potion inventories are set at program start, but totals carry through game).
//Exit:  When the player selects the 'No Thank You' option.
void vendor::display_shop(int &gold, int &h_pots, int &s_pots)
{
    char purchase = 'x'; //reinitializing to anything but 'n'
    cout << "____________________________________________________________________________________" << endl;
    cout << "Welcome to my shop!  You look like an adventurer.  I used to be an adventurer until I took an arrow to the knee.\nI admit my shop isn't much, our town has struggled mightily since that cruel Wizard took over the tower in the mountains.\nBut, please browse my wares.  I recommend some potions in case you run into some of his blood-thirsty 'peacekeepers'." << endl;
    while(purchase != 'n' && purchase != 'N'){
        cout << "____________________________\n|(s)trength potion: 10 gold|\n|(h)ealth potion: 10 gold  |\n|(n)o thank you.           |\n|__________________________|" << endl;
        cin >> purchase;
        while(purchase != 's' && purchase != 'S' && purchase != 'h' && purchase != 'H' && purchase != 'n' && purchase != 'N'){ //need cin.fail() while
            cin.clear();  //clears failbit in cin
            cin.ignore(256,'\n');
            while(!isspace(cin.get()))
            {
                continue; //gets rid of bad input
            }
            cout << "Please enter a valid option: ";
            cin >> purchase;
        }
        if (purchase == 's' || purchase == 'S') {
            //cout << "s";
            vendor::purchase_strength(gold, s_pots);
        }
        if (purchase == 'h' || purchase == 'H') {
            //cout << "h";
            vendor::purchase_health(gold, h_pots);
        }
    }
    cout << "Well, it was nice to meet you traveler.  Enjoy your stay in Goldshire." << endl;
    locations::display_goldshire();
    locations::goldshire_actions(gold, h_pots, s_pots);
}
//Purpose:  To allow the purchase of a health potion
//Entry:  When the user selects the h option.
//Exit:  When a purchase has been made or user selects 0 for the amount to purchase.
void vendor::purchase_health(int &gold, int &h_pots)
{
    int h_pots_temp;
    //-10g from gold inventory, +1 to health pot inventory
    cout << "How many bottles would you like to purchase?"<< endl;
    cin >> h_pots_temp;
    while(cin.fail()){
        cin.clear();  //clears failbit in cin
        cin.ignore(256,'\n');
        while(!isspace(cin.get()))
        {
            continue; //gets rid of bad input
        }
        cout << "Please enter a valid option: ";
        cin >> h_pots_temp;
    }
    if ((h_pots_temp*10) < gold) {
        gold = gold - (h_pots_temp*10);
        h_pots = h_pots + (h_pots_temp);
        cout << "You now have " << h_pots << " health potions." << endl;
        cout << "You now have " << gold << " gold." << endl;
    }
    else{
        cout << "You don't have enough gold!" << endl;
    }
}
//Purpose:  To allow the purchase of a strength potion
//Entry:  When the user selects the s option.
//Exit:  When a purchase has been made or user selects 0 for the amount to purchase.
void vendor::purchase_strength(int &gold, int &s_pots)
{
    int s_pots_temp;
    //-10g from gold inventory, +1 to strength pot inventory
    cout << "How many bottles would you like to purchase?"<< endl;
    cin >> s_pots_temp;
    while(cin.fail()){
        cin.clear();  //clears failbit in cin
        cin.ignore(256,'\n');
        while(!isspace(cin.get()))
        {
            continue; //gets rid of bad input
        }
        cout << "Please enter a valid option: ";
        cin >> s_pots_temp;
    }
    if ((s_pots_temp*10) < gold) {
        gold = gold - (s_pots_temp*10);
        s_pots = s_pots + (s_pots_temp);
        cout << "You now have " << s_pots << " strength potions." << endl;
        cout << "You now have " << gold << " gold." << endl;
    }
    else{
        cout << "You don't have enough gold!" << endl;
    }
}
//Purpose:  To show a depiction of the enemy.
//Entry:  None, called from main when combat with the wizard is initiated.
//Exit:  None, just a cout << function.
 void wizard_boss::display_wizard()
{
    cout << "____________________________________________________________________________________" << endl;
    cout << "You have arrived at the Wizard's Tower.\n" << endl;
    cout << "WHO DARES ENTER MY TOWER!?" << endl;
    cout << "I PUT ON MY ROBE AND WIZARD HAT." << endl;
    cout << "                   ____       " << endl;
    cout << "                  /   /      " << endl;
    cout << "               __/____(_      "  << endl;
    cout << "              / _______ \\    "  << endl;
    cout << "             _\\_)/ v \(_/_   " << endl;
    cout << "             / _(\\o o/)_ \\     " << endl;
    cout << "             \\ \())(O)(()/ /  " << endl;
    cout << "             ' \(((()))/ '   "  << endl;
    cout << "            / ' \\)).))/ ' \\  " << endl;
    cout << "           / _ \\ - | - /_  \\   " << endl;
}
//Purpose:  To have the Wizard give a taunt at each combat turn.
//Entry:  None, called from combat_engine at each turn.
//Exit:  None, just a cout << function.
void wizard_boss::wizard_taunt()
{
    int taunt;
    taunt = rand() % 3;
    switch(taunt)
    {
        case 0:
            cout << "'YOUR DARE TO CHALLENGE THE MIGHTIEST SORCERER IN ALL THE LANDS!'" << endl;
            break;
        case 1:
            cout << "'YOUR POWERS ARE WEAK!'" << endl;
            break;
        case 2:
            cout << "'IS THAT ALL YOU HAVE?!'" << endl;
            break;
        default: cout << "error" << endl;
            break;
    }
    cout << endl;
}
//Purpose:
//Entry:
//Exit:
void gatehouse_knights::display_knights()
{
    cout << "____________________________________________________________________________________" << endl;
    cout << "You have arrived at a gatehouse on your way up the mountain pathway.  Two guards stand at the gate.  You approach cautiously." << endl;
    cout << "                                   _                                        " << endl;
    cout << "                         .-.      / \\       _                              " << endl;
    cout << "            ^^          /   \\    /^./\\_   _/ \\                           " << endl;
    cout << "           _        .--'\\/\\_ \\__/.     \\ /    \\  ^^  ___               " << endl;
    cout << " _   _   _/ \\_    _/ ^     \\/  __  :' _/\\_   _/\\ ___/   \\              " << endl;
    cout << "| |_| |_| |   \\  /   .'  _/  /  \\   ^| /| |_| |_| |.`'\\_/\\              " << endl;
    cout << "|        :|\\  /\\/ :'__ ^/  ^/    `--./  |         |     _:\\ _            " << endl;
    cout << ":       .:| \\/  \\ _/  \\-'__/.' ^ _      :         |   .  __/ \\          " << endl;
    cout << "\\       .:/  _   _  `_   _   _   _   _  \\       .:/  `._/  ^  \\          " << endl;
    cout << " |      :|__| |_| |_| |_| |_| |_| |_| |__|  __  :|  `-. `.  -  `.           " << endl;
    cout << " |      :| =          //---\\\\     `-.    |     .:|   \\    \\  .-  \\     " << endl;
    cout << " |  []  :|  _     =()//_____\\\\()         |   [] :|                \\      " << endl;
    cout << " |      :|; -      |:|-|-|-|-|:|     =   |      :|                 \\       " << endl;
    cout << " |  []  :|:   =    |:|-|-|-|-|:|-        |   [] :|                          " << endl;
    cout << " |      :|         |:|-|-|-|-|:|       = |      :|                          " << endl;
    cout << " |      :|}   =    |:|-|-|-|-|:| -       |      :|                          " << endl;
    cout << " ;      :=:--------|:|       |:|=--------;      :=--------------------------" << endl;
    cout << "(_________)                             (=________)                         " << endl;
}
//Purpose:  To have the Wizard give a taunt at each combat turn.
//Entry:  None, called from combat_engine at each turn.
//Exit:  None, just a cout << function.
void gatehouse_knights::knight_taunt()
{
    int taunt;
    taunt = rand() % 3;
    switch(taunt)
    {
        case 0:
            cout << "'You'll never get past me!'" << endl;
            break;
        case 1:
            cout << "'Lay down your weapon and I will let you live.'" << endl;
            break;
        case 2:
            cout << "'You chose the wrong knight to attack.'" << endl;
            break;
        default: cout << "error" << endl;
            break;
    }
    cout << endl;
}