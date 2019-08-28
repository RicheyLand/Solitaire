#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <random>
using namespace std;

class Coordinates                                   //  represents coordintes of cards from cards array
{
public:
    int x;
    int y;
};

class Card                                          //  represents single card with its color
{
public:
    int type;
    int color;
};

enum                                                //  all possible colors of card
{
    empty,
    hearts,
    diamonds,
    clubs,
    spades
};

class Core                                          //  represents core of solitaire
{
protected:
    ifstream fin;                                   //  input file stream object
    ofstream fout;                                  //  output file stream object

    bool game_over;                                 //  holds if game is over

    vector<Card> cards_deck;                        //  all 52 cards sorted in random order
    stack<Card> cards_stack;                        //  stack of all cards in cards stack
    stack<Card> final_stack[4];                     //  stacks of all four final cards
    stack<Card> downturned_stack_1;                 //  stack of all downturned cards in second column of cards array
    stack<Card> downturned_stack_2;                 //  stack of all downturned cards in third column of cards array
    stack<Card> downturned_stack_3;                 //  stack of all downturned cards in fourth column of cards array
    stack<Card> downturned_stack_4;                 //  stack of all downturned cards in fifth column of cards array
    stack<Card> downturned_stack_5;                 //  stack of all downturned cards in sixth column of cards array
    stack<Card> downturned_stack_6;                 //  stack of all downturned cards in seventh column of cards array
    bool visible[7][14];                            //  holds which cards from cards array are currently visible
    int colors[7][14];                              //  holds colors of every card from down card array

    int redeals_left;                               //  holds how many redeals of cards deck left
    bool stack_selected;                            //  holds if actual selected card is on cards stack
    bool final_selected;                            //  holds if actual selected card is one of final cards
    bool selected;                                  //  holds if actual selected card is from cards array
    Coordinates select;                             //  concrete coordinates of selected card
    vector<Coordinates> possible;                   //  allow cards in which is possible to move currently selected card

    void init_variables();                          //  initialize all variables
    void init_deck();                               //  generate cards deck content and initialize downturned cards
    void clear_cards();                             //  clear all stacks and arrays of cards to default state
    void check_win();                               //  check if winning state of game has been reached
                                                    //  all these pure virtual methods must be redefined in interface subclass
    virtual void refresh_sizes(int) = 0;            //  refresh all cards from one column of cards array
    virtual void refresh_card(int &, int, int) = 0; //  refresh type and color of card from cards cards array
    virtual void refresh_final_card(int, int) = 0;  //  refresh type and color of one of final cards
    virtual void refresh_stack_card(bool) = 0;      //  refresh type and color of top stack card
    virtual void refresh_cards_deck() = 0;          //  refresh cards deck visibility to empty or non-empty
    virtual void show_card(int, int) = 0;           //  show card on concrete coordinates of cards array
    virtual void hide_card(int, int) = 0;           //  hide card on concrete coordinates of cards array
    virtual void use_downturned_card(int &, int) = 0;   //  hide downturned card and rebuild concrete cards array column again
    virtual void handle_win() = 0;                  //  specific reaction when winning state of game has been reached

    void handle_command(int &, int &);              //  reaction for selected command from concrete interface

    virtual ~Core() = 0;
};

#endif
