/*
Andrew Hoyt MW 9:00 AM - 10:40 AM

Ghetto Palmetto
Game made by Andrew Hoyt for Introduction to C++ Final Project

EXTRA CREDIT:
1. A 18x18 grid with every grid square made of 4x2 ascii characters
2. Music that loops (made by me!)
3. Direct keyboard input for character movement
4. Menu for attacks that can choose the enemy
5. Instructional side box with that details lives/actions remaining/movement instructions/dialogue
6. Obstacles such as palm trees and houses
7. Color for border/enemies/player
8. Directional sprites and stunned sprites (enemies/player change sprites depending on situation and movement)

KEY:
[>>]
[--]   Player

[**]
[()]   Player Stunned

\--/
{[]}   Bug

\~~/
{[]}   Bug Stunned

NOTE:
For the music, I have the link library winmm.lib

Feel free to contact me at andrew@chillhammer.com for any inquiries/questions.
*/
#include <iostream>
#include <iomanip>
#include <cstring>
#include <conio.h>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <windows.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include "color.h"
#include "MMSystem.h"

using namespace std;
using namespace Petter;
const int GRID_SIZE = 18;
const int INSTRUCTIONAL_BOX_SIZE = 18;
enum GridSpace { DEFAULT, EMPTY, COLLISION_POINT, PLAYERR, PLAYERL, PLAYER_STUNNED, TLHOUSE, TRHOUSE, BLHOUSE, BRHOUSE, TLTREE, TRTREE, MLTREE, MRTREE, BLTREE, BRTREE,
                 ENEMY_POINT, STBUGU, STBUGU_STUNNED, STBUGD, STBUGD_STUNNED, SMBUGU, SMBUGU_STUNNED, SMBUGD, SMBUGD_STUNNED, ENEMY_POINT_END
               };
enum GameState { PLAYER_TURN, ENEMY_TURN, GAME_OVER_POINT, LOST, WON };
enum MenuOption { RAID, ATTACKT, ATTACKTR, ATTACKR, ATTACKBR, ATTACKB, ATTACKBL, ATTACKL, ATTACKTL, MUTE, EXIT };
enum ActionMessage { NO_MESSAGE, RAID_MISSED, RAID_HIT, ATTACK_MISSED, ATTACK_HIT, FLYING_WINGS, LEG_ASSAULT };
int randomInt(int);
struct Controller
{
    GameState state = PLAYER_TURN;
    int maxMoves = 2;
    int usedMoves = 1;
    const int RAID_CHANCE = 68;
    const int STOMP_CHANCE = 60;
    const int STOMP_STUNNED_CHANCE = 90;
    const int LEG_ASSAULT_CHANCE = 72;
    const int FLYING_WINGS_CHANCE = 80;
    const int MAX_RESOLVE_POINTS = 5;
    int resolvePoints = MAX_RESOLVE_POINTS;
    ActionMessage lastAction = NO_MESSAGE;
};
struct GameGrid
{
    int margin;
    GridSpace position[GRID_SIZE][GRID_SIZE];

    void initGrid();
    void drawGrid(Controller *const);
};
struct Entity
{
    int x;
    int y;
    Entity(int xpos,int ypos)
    {
        x = xpos;
        y = ypos;
    }
    Entity(GameGrid *const gridPtr)
    {
        srand(time(NULL));
        do
        {
            x = rand() % GRID_SIZE;
            y = rand() % GRID_SIZE;
        }
        while(gridPtr->position[y][x] > COLLISION_POINT);
    }
};
struct PlayerEntity:Entity
{
    bool isMove(char,GameGrid *const);
    int horizontalFacing; //-1 or 1
    bool attackedThisTurn = false;
    bool stunned = false;
    GridSpace sprite;
    //using Entity::Entity;  //inherits from derived struct
    PlayerEntity(int xpos, int ypos,GameGrid *const gridPtr) : Entity(xpos, ypos)
    {
        gridPtr->position[ypos][xpos] = PLAYERR;
        horizontalFacing = 1;
    }
};
struct House:Entity
{
    House(int xpos, int ypos,GameGrid *const gridPtr) : Entity(xpos, ypos)
    {
        gridPtr->position[ypos][xpos]     = TLHOUSE;
        gridPtr->position[ypos][xpos+1]   = TRHOUSE;
        gridPtr->position[ypos+1][xpos]   = BLHOUSE;
        gridPtr->position[ypos+1][xpos+1] = BRHOUSE;
    }
};
struct PalmTree:Entity
{
    PalmTree(int xpos, int ypos,GameGrid *const gridPtr) : Entity(xpos, ypos)
    {
        gridPtr->position[ypos][xpos]     = TLTREE;
        gridPtr->position[ypos][xpos+1]   = TRTREE;
        gridPtr->position[ypos+1][xpos]   = MLTREE;
        gridPtr->position[ypos+1][xpos+1] = MRTREE;
        gridPtr->position[ypos+2][xpos]   = BLTREE;
        gridPtr->position[ypos+2][xpos+1] = BRTREE;
    }
};
struct Bug:Entity
{
    bool isMove(GameGrid *const);
    bool isAttack(GameGrid *const, PlayerEntity *const, Controller *const);
    int verticalFacing = -1;
    bool stunned = false;
    Bug(int xpos, int ypos) : Entity(xpos,ypos) {};
    Bug(GameGrid *const gridPtr):Entity(gridPtr) {}
};
struct StupidBug:Bug
{
    StupidBug(int xpos, int ypos, GameGrid *const gridPtr): Bug(xpos,ypos)
    {
        gridPtr->position[ypos][xpos] = STBUGU;
    };
    StupidBug(GameGrid *const gridPtr):Bug(gridPtr)
    {
        gridPtr->position[y][x] = STBUGU;
    };
};
struct SmartBug:Bug
{
    SmartBug(int xpos, int ypos, GameGrid *const gridPtr): Bug(xpos,ypos)
    {
        gridPtr->position[ypos][xpos] = SMBUGU;
    };
    SmartBug(GameGrid *const gridPtr):Bug(gridPtr)
    {
        gridPtr->position[y][x] = SMBUGU;
    };
};
struct ActionMenu
{
    int margin = 40;
    vector<MenuOption> options;
    MenuOption selected;
    bool updateMenu(char,vector<Bug*>&, PlayerEntity *const, GameGrid *const, Controller *const);
    void updateOptions(GameGrid *const, PlayerEntity *const);
    void drawMenu();
    ActionMenu()
    {
        options.push_back(RAID);
        //options.push_back(MUTE);
        options.push_back(EXIT);
        selected = options.at(0);
    }
};
void drawHeader();
void drawSpace(int);
void drawRefresh();
int sign(int);
void printCString(char[],Color);
void fillCString(char[],char);

int main()
{
    //Initialize
    GameGrid grid;
    Controller game;
    ActionMenu menu;
    GameGrid *const GRID_PTR = &grid;
    Controller *const GAME_PTR = &game;
    grid.initGrid();

    PlayerEntity player(5,6,GRID_PTR);
    PlayerEntity *const PLAYER_PTR = &player;

    House house1(3,2,GRID_PTR);
    House house2(6,2,GRID_PTR);
    House house3(8,2,GRID_PTR);
    House house4(10,2,GRID_PTR);
    House house5(13,2,GRID_PTR);

    PalmTree tree1(2,8,GRID_PTR);
    PalmTree tree2(15,8,GRID_PTR);

    House house6(3,15,GRID_PTR);
    House house7(6,15,GRID_PTR);
    House house8(8,15,GRID_PTR);
    House house9(10,15,GRID_PTR);
    House house10(13,15,GRID_PTR);
    vector<Bug*> enemies;
    //5 Randomly Spawned Bugs
    enemies.push_back(new StupidBug(GRID_PTR));
    enemies.push_back(new StupidBug(GRID_PTR));
    enemies.push_back(new StupidBug(GRID_PTR));
    enemies.push_back(new StupidBug(GRID_PTR));
    enemies.push_back(new StupidBug(GRID_PTR));


    //Music
    PlaySound(TEXT("GhettoPalmettoMusic.wav"),NULL,SND_ASYNC | SND_LOOP);
    drawRefresh();
    drawHeader();
    grid.drawGrid(GAME_PTR);
    menu.updateOptions(GRID_PTR,PLAYER_PTR);
    menu.drawMenu();
    //Draw
    while(game.state < GAME_OVER_POINT)
    {
        //Game Win/Lose Conditions
        if(enemies.empty())
        {
            game.state = WON;
            drawRefresh();
            drawHeader();
            grid.drawGrid(GAME_PTR);
        }
        if(game.resolvePoints < 1)
        {
            game.state = LOST;
            grid.position[player.y][player.x] = DEFAULT;
            drawRefresh();
            drawHeader();
            grid.drawGrid(GAME_PTR);
        }
        ////////////////////////////

        //Player Turn
        if(game.state == PLAYER_TURN)
        {
            char ch = getch();
            game.lastAction = NO_MESSAGE;
            bool menuChange = menu.updateMenu(ch, enemies, PLAYER_PTR, GRID_PTR, GAME_PTR);
            bool playerMove = player.isMove(ch,GRID_PTR);
            //Updating due to Menu change or Player movement
            if(menuChange || playerMove)
            {
                if(playerMove) //already added a move
                {
                    game.usedMoves++;
                }
                if(game.usedMoves > game.maxMoves) //only 2 actions per turn
                {
                    game.usedMoves = 1;
                    game.state = ENEMY_TURN;
                    if(player.stunned)
                    {
                        player.stunned = false;
                        grid.position[player.y][player.x] = PLAYERR;
                    }
                }
                while(kbhit())
                {
                    getch();
                }
                drawRefresh();
                drawHeader();
                grid.drawGrid(GAME_PTR);
                if(game.state == PLAYER_TURN)
                {
                    if(!menuChange)
                    {
                        menu.updateOptions(GRID_PTR, PLAYER_PTR);
                    }
                    menu.drawMenu();
                }
            }

        }
        else
            //Enemy Turn
            if(game.state == ENEMY_TURN)
            {
                for(int i=0; i<enemies.size(); i++)
                {
                    if(!enemies.at(i)->stunned)
                    {
                        if(!player.attackedThisTurn)
                        {
                            player.attackedThisTurn = enemies.at(i)->isAttack(GRID_PTR, PLAYER_PTR, GAME_PTR);
                            if(!player.attackedThisTurn) //move if cannot hit
                            {
                                enemies.at(i)->isMove(GRID_PTR);
                            }
                        }
                        else  //only move if player has already been attacked
                        {
                            enemies.at(i)->isMove(GRID_PTR);
                        }

                    }
                }
                Sleep(1000);
                if(++game.usedMoves > game.maxMoves)
                {
                    game.usedMoves = 1;
                    //Player is back to normal
                    player.attackedThisTurn = false;

                    if(!player.stunned)
                    {
                        game.state = PLAYER_TURN;
                        grid.position[player.y][player.x]=player.sprite;
                    }
                    else
                    {
                        player.stunned = false;
                        player.attackedThisTurn = true;
                    }
                    //Enemies are not stunned anymore
                    for(int i=0; i<enemies.size(); i++)
                    {
                        Bug* b = enemies.at(i);
                        if(b->stunned)
                        {
                            b->stunned = false;
                            GridSpace unstunned = static_cast<GridSpace>(GRID_PTR->position[b->y][b->x]-1);
                            GRID_PTR->position[b->y][b->x] = unstunned;
                        }
                    }
                }
                while(kbhit())
                {
                    getch();
                }
                drawRefresh();
                drawHeader();
                grid.drawGrid(GAME_PTR);
                if(game.state == PLAYER_TURN)
                {
                    menu.updateOptions(GRID_PTR, PLAYER_PTR);
                    menu.drawMenu();
                }
            }
    }
    //Ending of the Game
    cout << WHITE;
    if(game.state == WON)
    {
        cout << "\n\n";
        drawSpace(grid.margin);
        cout <<"Congratulations, you squashed all the bugs! You won!\n\n\n";
    }
    else if (game.state == LOST)
    {
        cout << "\n\n";
        drawSpace(grid.margin);
        cout << "Wow, you managed to die to bugs. Game Over...\n\n";
    }
    return 0;
}


void drawHeader()
{
    int hm = 30;
    cout << "\n\n";
    cout << setw(hm) << WHITE << " "               << " __        __ _____ _____  __       __    _                  __ _____ _____  __\n";
    cout << setw(hm) << " "                        << "|  ' |  | |     |     |   |  |     |__|  / \\   |   |\\    /| |     |     |   |  |\n";
    cout << setw(hm) << NORMAL
            << "Andrew Hoyt Produces    " << WHITE         << "| _  |--| |-    |     |   |  |     |    /___\\  |   | \\  / | |-    |     |   |  |\n";
    cout << setw(hm) << " "                        << "|__| |  | |__   |     |   |__|     |   /     \\ |__ |  \\/  | |__   |     |   |__|\n";
}
void drawSpace(int s)
{
    cout << setw(s) << " ";
}
void drawRefresh()
{
    //cout << string(10000,'\n');
    system("cls");
}
int randomInt(int num)
{
    srand(time(NULL));
    return rand() % num;
}
int sign(int num)
{
    if(num < 0) return -1;
    if(num == 0) return 0;
    if(num > 0) return 1;
}
void printCString(char str[], Color col=NORMAL)
{
    int i = 0;
    //while(str[i]!='\0')
    while(strlen(str)>i)
    {
        cout << col << str[i];
        i++;
    }
}
void fillCString(char str[],char c)
{
    for(int i=0; i<strlen(str); i++)
        str[i]=c;
}

void GameGrid::initGrid()
{
    margin = 32;
    for(int i=0; i<GRID_SIZE; i++)
    {
        for(int o=0; o<GRID_SIZE; o++)
        {
            position[i][o] = DEFAULT;
        }
    }
}

void GameGrid::drawGrid(Controller *const gameController)
{
    //border variables
    char tl = 201;
    char bl = 200;
    char tr = 187;
    char br = 188;
    char hor = 205;
    char ver = 186;
    //c-string to-be-drawn
    char tbd[] = "OOOO";
    //color of grid space
    Color col = NORMAL;
    //string for instructional box
    string iStr = " ";
    //last action for instruction box
    ActionMessage action = gameController->lastAction;
    //draw top border
    drawSpace(margin);
    cout << YELLOW << tl;
    for(int i=0; i<GRID_SIZE; i++)
    {
        cout << hor << hor << hor << hor;
    }
    cout << tr;
    //instructional box
    drawSpace(margin*0.25);
    cout << tl;
    for(int i=0; i<INSTRUCTIONAL_BOX_SIZE; i++)
    {
        cout << hor;
    }
    cout << tr << endl;

    ///////////////////////////////////////
    //    THE GRID    //
    ///////////////////////////////////////
    for(int i=0; i<GRID_SIZE; i++)
    {
        //Top Half //-- //
        drawSpace(margin);
        for(int o=0; o<GRID_SIZE; o++)
        {
            if(o==0)
            {
                cout << YELLOW << ver;
            }
            col = WHITE;
            switch(position[i][o])
            {
            case DEFAULT:
                fillCString(tbd,177);
                col = GREEN;
                break;
            case EMPTY:
                fillCString(tbd,176);
                break;
            case PLAYERL:
                strcpy(tbd,"[<<]");
                break;
            case PLAYERR:
                strcpy(tbd,"[>>]");
                break;
            case PLAYER_STUNNED:
                strcpy(tbd,"[**]");
                break;
            case TLHOUSE:
                strcpy(tbd," /--");
                break;
            case TRHOUSE:
                strcpy(tbd,"--\\ ");
                break;
            case BLHOUSE:
                strcpy(tbd,"| ` ");
                break;
            case BRHOUSE:
                strcpy(tbd," ` |");
                break;

            case TLTREE:
                strcpy(tbd,"  _\\");
                break;
            case TRTREE:
                strcpy(tbd,"/_  ");
                break;
            case MLTREE:
                strcpy(tbd," //\\");
                break;
            case MRTREE:
                strcpy(tbd,"/\\\\ ");
                break;
            case BLTREE:
                strcpy(tbd,"   (");
                break;
            case BRTREE:
                strcpy(tbd,")   ");
                break;

            case STBUGU:
                strcpy(tbd,"\\--/");
                break;
            case STBUGD:
                strcpy(tbd,"{[]}");
                break;
            case STBUGU_STUNNED:
                strcpy(tbd,"\\~~/");
                break;
            case STBUGD_STUNNED:
                strcpy(tbd,"{[]}");
                break;
            case SMBUGU:
                strcpy(tbd,"\\oo/");
                break;
            case SMBUGD:
                strcpy(tbd,"{[]}");
                break;
            case SMBUGU_STUNNED:
                strcpy(tbd,"\\xx/");
                break;
            case SMBUGD_STUNNED:
                strcpy(tbd,"{[]}");
                break;
            }
            if(position[i][o] > ENEMY_POINT)
            {
                col = RED;
            }
            if(position[i][o] >= PLAYERR && position[i][o] <= PLAYER_STUNNED)
            {
                col = BLUE;
            }
            printCString(tbd, col);
            if(o==GRID_SIZE-1)
            {
                cout << YELLOW << ver;
            }

        }
        //instructional box top half
        drawSpace(margin*0.25);
        cout << YELLOW << ver << setw(INSTRUCTIONAL_BOX_SIZE) << left;
        stringstream sst;
        switch(i)
        {
        case 1:
            iStr = "   Turn ";
            if(gameController->state == PLAYER_TURN)
            {
                iStr +="PLAYER";
            }
            else
            {
                iStr +="ENEMY";
            };
            break;
        case 3:
            iStr = "\\ \\    | |    / / ";
            break;
        case 4:
            iStr = "  \\Q\\  |W|  /E/   ";
            break;
        case 5:
            iStr = "____\\ \\| |/ /_____";
            break;
        case 6:
            iStr = "    / /| |\\ \\     ";
            break;
        case 7:
            iStr = "  /Z/  |S|  \\C\\   ";
            break;
        case 8:
            iStr = "/ /    | |    \\ \\ ";
            break;
        case 10:
            sst << "       ";
            sst << gameController->resolvePoints;
            sst << "/";
            sst << gameController->MAX_RESOLVE_POINTS;
            iStr = sst.str();
            break;
        case 13:
            if(action == RAID_HIT || action == RAID_MISSED)
            {
                iStr = "  You used RAID!";
            }
            else if(action == ATTACK_HIT || action == ATTACK_MISSED)
            {
                iStr = "   You stomped!";
            }
            else if(action == FLYING_WINGS || action == LEG_ASSAULT)
            {
                iStr =" You've been hurt!";
            };
            break;
        default:
            iStr = " ";
            break;
        }
        cout << NORMAL << iStr;
        cout << YELLOW << ver << endl;

        //Bottom Half //--//
        drawSpace(margin);
        for(int o=0; o<GRID_SIZE; o++)
        {
            if(o==0)
            {
                cout << ver;
            }
            col = WHITE;
            switch(position[i][o])
            {
            case DEFAULT:
                fillCString(tbd,177);
                col = GREEN;
                break;
            case EMPTY:
                fillCString(tbd,176);
                break;
            case PLAYERR:
                strcpy(tbd,"[--]");
                break;
            case PLAYERL:
                strcpy(tbd,"[--]");
                break;
            case PLAYER_STUNNED:
                strcpy(tbd,"[()]");
                break;
            case TLHOUSE:
                strcpy(tbd,"/---");
                break;
            case TRHOUSE:
                strcpy(tbd,"---\\");
                break;
            case BLHOUSE:
                strcpy(tbd,"|  [");
                break;
            case BRHOUSE:
                strcpy(tbd,"]  |");
                break;

            case TLTREE:
                strcpy(tbd," /_ ");
                break;
            case TRTREE:
                strcpy(tbd," _\\ ");
                break;
            case MLTREE:
                strcpy(tbd,"   (");
                break;
            case MRTREE:
                strcpy(tbd,")   ");
                break;
            case BLTREE:
                strcpy(tbd,"  ()");
                break;
            case BRTREE:
                strcpy(tbd,"    ");
                break;

            case STBUGU:
                strcpy(tbd,"{[]}");
                break;
            case STBUGD:
                strcpy(tbd,"/--\\");
                break;
            case STBUGU_STUNNED:
                strcpy(tbd,"{[]}");
                break;
            case STBUGD_STUNNED:
                strcpy(tbd,"/~~\\");
                break;
            case SMBUGU:
                strcpy(tbd,"{[]}");
                break;
            case SMBUGD:
                strcpy(tbd,"/oo\\");
                break;
            case SMBUGU_STUNNED:
                strcpy(tbd,"{[]}");
                break;
            case SMBUGD_STUNNED:
                strcpy(tbd,"/xx\\");
                break;
            }
            if(position[i][o] > ENEMY_POINT)
            {
                col = RED;
            }
            if(position[i][o] >= PLAYERR && position[i][o] <= PLAYER_STUNNED)
            {
                col = BLUE;
            }
            printCString(tbd, col);
            if(o==GRID_SIZE-1)
            {
                cout << YELLOW << ver;
            }
        }
        //instructional box bottom half
        drawSpace(margin*0.25);
        cout << ver << setw(INSTRUCTIONAL_BOX_SIZE) << left;
        stringstream ssb;
        switch(i)
        {
        case 1:
            ssb << "       ";
            ssb << gameController->usedMoves;
            ssb << "/";
            ssb << gameController->maxMoves;
            iStr = ssb.str();
            break;
        case 3:
            iStr = " \\ \\   | |   / /  ";
            break;
        case 4:
            iStr = "   \\ \\ | | / /    ";
            break;
        case 5:
            iStr = "__A__MOVEMENT__D__";
            break;
        case 6:
            iStr = "   / / | | \\ \\    ";
            break;
        case 7:
            iStr = " / /   | |   \\ \\  ";
            break;
        case 9:
            iStr = "  Resolve Points  ";
            break;
        case 13:
            switch(action)
            {
            case RAID_HIT:
                iStr = "You stunned a bug.";
                break;
            case RAID_MISSED:
                iStr = " Nothing Happened";
                break;
            case ATTACK_HIT:
                iStr = "    He died :o";
                break;
            case ATTACK_MISSED:
                iStr = "The bug dodged it.";
                break;
            case LEG_ASSAULT:
                iStr = " It's Leg Assault!";
                break;
            case FLYING_WINGS:
                iStr = " You are stunned.";
                break;
            };
            break;
        default:
            iStr = " ";
            break;
        }
        cout << NORMAL << iStr;

        cout << YELLOW << ver << endl;
    }
    ////////////////////////////////////////////////////
    //draw bottom border
    drawSpace(margin);
    cout << bl;
    for(int i=0; i<GRID_SIZE; i++)
    {
        cout << hor << hor << hor << hor;
    }
    cout << br;
    //instructional box border
    drawSpace(margin*0.25);
    cout << bl;
    for(int i=0; i<INSTRUCTIONAL_BOX_SIZE; i++)
    {
        cout << hor;
    }
    cout << br << endl;
}
void ActionMenu::drawMenu()
{
    drawSpace(margin);
    cout << NORMAL;
    cout << "/ \\  Use the Arrow Keys to Navigate Menu. Enter to Select\n";
    for(int i=0; i<options.size(); i++)
    {
        string opstr = " ";
        string select = "   ";
        string attack = "";
        if(selected == options.at(i))
            select = "-> ";
        switch(options.at(i))
        {
        case RAID:
            opstr = "Use RAID on the area around you to stun";
            break;
        case ATTACKTL:
            attack = "top-left";
            break;
        case ATTACKT:
            attack = "top";
            break;
        case ATTACKTR:
            attack = "top-right";
            break;
        case ATTACKR:
            attack = "right";
            break;
        case ATTACKBR:
            attack = "bottom-right";
            break;
        case ATTACKB:
            attack = "bottom";
            break;
        case ATTACKBL:
            attack = "bottom-left";
            break;
        case ATTACKL:
            attack = "left";
            break;
        case MUTE:
            opstr = "Toggle Music";
            break;
        case EXIT:
            opstr = "Exit";
            break;
        }
        if(attack != "")
        {
            opstr = "Use Hypersonic Foot Stomp on the "+attack+" bug";
        }
        drawSpace(margin);
        cout << " | \t" << select << opstr << endl;
    }
    drawSpace(margin);
    cout << "\\ /";
}
bool ActionMenu::updateMenu(char input, vector<Bug*> &bugs, PlayerEntity *const plyr, GameGrid *const gridPtr, Controller *const gameController)
{
    int index = find(options.begin(),options.end(),selected)-options.begin();
    int prevIndex = index;
    switch(input)
    {
    case 72: /*up*/
        index-- ;
        break;
    case 80: /*down*/
        index++;
        break;
    case '\r': // clicked enter
        if(selected == RAID)
        {
            gameController->usedMoves++;
            gameController->lastAction = RAID_MISSED;
            for(int i=0; i<bugs.size(); i++)
            {
                int px = plyr->x;
                int py = plyr->y;
                int bx = bugs.at(i)->x;
                int by = bugs.at(i)->y;
                if(abs( bx-px )<=2 && abs( by-py )<=2)
                {
                    if(rand()%100 <= gameController->RAID_CHANCE)
                    {
                        if(!bugs.at(i)->stunned)
                        {
                            gameController->lastAction = RAID_HIT;
                            bugs.at(i)->stunned = true;
                            gridPtr->position[by][bx]=static_cast<GridSpace>(gridPtr->position[by][bx]+1);
                        }
                    }
                }
            }
        }
        if(selected > RAID && selected < MUTE) // attack options
        {
            gameController->usedMoves++;
            gameController->lastAction = ATTACK_MISSED;
            int bx = plyr->x;
            int by = plyr->y;
            switch(selected)
            {
            case ATTACKTL:
                by--;
                bx--;
                break;
            case ATTACKT :
                by--;
                break;
            case ATTACKTR:
                by--;
                bx++;
                break;
            case ATTACKL :
                bx--;
                break;
            case ATTACKR :
                bx++;
                break;
            case ATTACKBL:
                by++;
                bx--;
                break;
            case ATTACKB :
                by++;
                break;
            case ATTACKBR:
                by++;
                bx++;
                break;
            }
            for(int i=0; i<bugs.size(); i++)
            {
                if(bugs.at(i)->x == bx && bugs.at(i)->y == by) //found bug
                {
                    int chance = gameController->STOMP_CHANCE;
                    if(bugs.at(i)->stunned)
                    {
                        chance = gameController->STOMP_STUNNED_CHANCE;
                    }
                    if(rand()%100 <= chance)
                    {
                        gameController->lastAction = ATTACK_HIT;
                        bugs.erase(bugs.begin()+i);
                        gridPtr->position[by][bx]=DEFAULT;
                    }
                }
            }
        }
        if(selected == EXIT)
        {
            exit(0);
        }

        ;
        break;
    }
    //clamping in bounds
    if(index<0 || index>=options.size())
        index = prevIndex;
    //ending function
    if(index != prevIndex || input == '\r')
    {
        selected = options.at(index);
        return true;
    }
    else
        return false;
}
void ActionMenu::updateOptions(GameGrid *const gridPtr, PlayerEntity *const plyr)
{
    int xx;
    int yy;

    vector<MenuOption>::iterator iterMenu = options.begin()+1;
    //wiping out attack options
    while(*iterMenu > RAID && *iterMenu < MUTE)
    {
        if(selected == *iterMenu)
            selected = options.at(0);
        options.erase(iterMenu);
        iterMenu = options.begin()+1;
    }
    for(xx = plyr->x+1; xx>=plyr->x-1; xx--)
    {
        for(yy = plyr->y+1; yy>=plyr->y-1; yy--)
        {
            //adding enemy attack choices
            if(gridPtr->position[yy][xx] > ENEMY_POINT && gridPtr->position[yy][xx] < ENEMY_POINT_END)
            {
                MenuOption attackDir;
                switch((xx-plyr->x+2)+(yy-plyr->y+1)*3)
                {
                case 1:
                    attackDir = ATTACKTL;
                    break;
                case 2:
                    attackDir = ATTACKT;
                    break;
                case 3:
                    attackDir = ATTACKTR;
                    break;
                case 4:
                    attackDir = ATTACKL;
                    break;
                case 6:
                    attackDir = ATTACKR;
                    break;
                case 7:
                    attackDir = ATTACKBL;
                    break;
                case 8:
                    attackDir = ATTACKB;
                    break;
                case 9:
                    attackDir = ATTACKBR;
                    break;
                }
                options.insert(iterMenu,attackDir);
            }
        }
    }
}
bool PlayerEntity::isMove(char input,GameGrid *const gridPtr)
{
    int previousx = x;
    int previousy = y;
    switch(toupper(input))
    {
    case 'Q':
        y--;
        x--;
        break;
    case 'W':
        y--;
        break;
    case 'E':
        y--;
        x++;
        break;
    case 'A':
        x--;
        break;
    case 'S':
        y++;
        break;
    case 'C':
        y++;
        x++;
        break;
    case 'Z':
        y++;
        x--;
        break;
    case 'D':
        x++;
        break;
    }
    if(y < 0 || y >= GRID_SIZE)
    {
        y = previousy;
    }
    if(x < 0 || x >= GRID_SIZE)
    {
        x = previousx;
    }
    //collision
    if(gridPtr->position[y][x] > COLLISION_POINT || stunned)
    {
        x = previousx;
        y = previousy;
    }
    //horizontal facing
    if(sign(x-previousx)!=0)
        horizontalFacing = sign(x-previousx);
    //updating grid
    if(previousx != x || previousy != y)
    {
        if(horizontalFacing == 1)
        {
            sprite = PLAYERR;
        }
        else if(horizontalFacing == -1)
        {
            sprite = PLAYERL;
        }
        gridPtr->position[y][x] = sprite;
        gridPtr->position[previousy][previousx] = DEFAULT;
        return true;
    }
    else
    {
        return false;
    }
}
bool Bug::isMove(GameGrid *const gridPtr)
{
    int previousx = x;
    int previousy = y;
    //moving
    do
    {
        x = previousx;
        y = previousy;
        x += (rand()%3)-1;
        y += (rand()%3)-1;
        //clamp
        if(y < 0 || y >= GRID_SIZE)
            y = previousy;
        if(x < 0 || x >= GRID_SIZE)
            x = previousx;

    }
    while(gridPtr->position[y][x]>COLLISION_POINT);

    //facing
    if(sign(y-previousy)!=0)
        verticalFacing = sign(y-previousy);
    //ending function
    if(previousx != x || previousy != y)
    {
        if(verticalFacing == -1)
        {
            gridPtr->position[y][x] = STBUGU;
        }
        else if(verticalFacing == 1)
        {
            gridPtr->position[y][x] = STBUGD;
        }
        gridPtr->position[previousy][previousx] = DEFAULT;
        return true;
    }
    else
    {
        return false;
    }
}
bool Bug::isAttack(GameGrid *const gridPtr, PlayerEntity *const plyr, Controller *const gameController)
{
    int px = plyr->x;
    int py = plyr->y;
    int distx = abs(x-px);
    int disty = abs(y-py);
    if(distx<=2 && disty<=2)
    {
        if(distx <= 1 && disty <= 1)
        {
            //Leg Crawl Assault
            if(rand()%100 <= gameController->LEG_ASSAULT_CHANCE)
            {
                gameController->resolvePoints--;
                gameController->lastAction=LEG_ASSAULT;
                //knock-back
                int knockback = 5;
                int kx = 0;
                int ky = 0;
                do
                {
                    kx = plyr->x + rand()%(knockback*2+1) - knockback;
                    //kx = min(0,kx);
                    //kx = max(GRID_SIZE,kx);

                    ky = plyr->y + rand()%(knockback*2+1) - knockback;
                    //ky = min(0,ky);
                    //ky = max(GRID_SIZE,ky);
                }
                while(gridPtr->position[ky][kx] > COLLISION_POINT);
                gridPtr->position[plyr->y][plyr->x] = DEFAULT;
                plyr->x = kx;
                plyr->y = ky;
                gridPtr->position[plyr->y][plyr->x] = plyr->sprite;
            }
        }
        else
        {
            //Flying Wing Attack
            if(rand()%100 <= gameController->FLYING_WINGS_CHANCE)
            {
                gameController->resolvePoints--;
                gameController->lastAction=FLYING_WINGS;
                plyr->stunned = true;
                gridPtr->position[plyr->y][plyr->x] = PLAYER_STUNNED;
            }

        }
        return true;
    }
    else
    {
        return false;
    }
}
