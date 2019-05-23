#ifndef KLONDIKE_H
#define KLONDIKE_H

#include "core.h"

#include <QDialog>
#include <QWidget>
#include <QApplication>
#include <QObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QString>
#include <QPalette>
#include <QColor>
#include <QIcon>
#include <QDesktopWidget>
#include <QTabWidget>

enum
{
    card_width = 152,
    card_height = 220,
    card_small_height = 55
};

namespace Ui {
class Klondike;
}

class ClickableLabel : public QLabel                //  class which describes clickable label
{
    Q_OBJECT

public:
    explicit ClickableLabel(const QString & text = "", QWidget * parent = 0);
    void allow_mouse_tracking();                    //  allow mouse tracking events for label
    int line;                                       //  line index
    int field;                                      //  field index
    ~ClickableLabel();

signals:
    void clicked(int, int);                         //  signal with line and field coordinates which is sent when label is clicked
    void entered(int, int);                         //  signal with line and field coordinates which is sent when mouse cursor entered label area
    void leaved(int, int);                          //  signal with line and field coordinates which is sent when mouse cursor leaved label area

protected:
    void mousePressEvent(QMouseEvent *);            //  handle mouse press event
    void enterEvent(QEvent *);                      //  handle mouse cursor enter event
    void leaveEvent(QEvent *);                      //  handle mouse cursor leave event
};

class Klondike : public QDialog, public Core
{
    Q_OBJECT

public:
    explicit Klondike(QWidget *parent = 0);
    ~Klondike();

private:
    Ui::Klondike *ui;
    bool high_resolution;                           //  tells if high resolution is active or not

    int huge[7];                                    //  holds which card of concrete column is at bottom of column(card is huge)
    ClickableLabel ** cards;                        //  graphical representation of seven columns in cards array

    QPixmap * image_downturned;                     //  image of downturned card
    QPixmap * image_empty;                          //  image of empty card
    QPixmap * image_empty_possible;                 //  image of empty possible card
    QPixmap ** images;                              //  images of all cards in cards array
    QPixmap ** images_select;                       //  images of all cards in cards array(selected card)
    QPixmap ** images_possible;                     //  images of all cards in cards array(possible card)

    QLabel * downturned_card_1;                     //  graphical representation of downturned card in second column of cards array
    QLabel * downturned_card_2;                     //  graphical representation of downturned cards in third column of cards array
    QLabel * downturned_card_3;                     //  graphical representation of downturned cards in fourth column of cards array
    QLabel * downturned_card_4;                     //  graphical representation of downturned cards in fifth column of cards array
    QLabel * downturned_card_5;                     //  graphical representation of downturned cards in sixth column of cards array
    QLabel * downturned_card_6;                     //  graphical representation of downturned cards in seventh column of cards array

    ClickableLabel * next_card;                     //  graphical representation of top card in cards deck
    ClickableLabel * top_stack_card;                //  graphical representation of top card in cards stack

    ClickableLabel * final_card;                    //  graphical representation of top cards of four final stacks

    int maximum_size[7];                            //  actual size of covered card in all columns in cards array

    QPixmap help_screen_pixmap;                     //  image of help screen
    QLabel * separator;                             //  cards separator to allow correct alignment of game screen layouts
    QPixmap new_game_pixmap;                        //  image of main menu button
    QPixmap help_pixmap;                            //  image of help button
    QPixmap about_pixmap;                           //  image of about button
    QPixmap exit_pixmap;                            //  image of exit button
    ClickableLabel * new_game_button;               //  new game button from main menu
    ClickableLabel * help_button;                   //  help button from main menu
    ClickableLabel * about_button;                  //  about button from main menu
    ClickableLabel * exit_button;                   //  exit button from main menu

    QVBoxLayout * main_v_layout;                    //  main vertical layout of this dialog
    QHBoxLayout * up_h_layout;                      //  horizontal layout which holds cards deck, cards stack, buttons and four final cards
    QHBoxLayout * down_h_layout;                    //  horizontal layout which holds cards array and all downturned cards
    QVBoxLayout * down_v_layout;                    //  vector of vertical layouts which holds cards of one column from cards array

    QPalette * background_palette;                  //  palette which holds background color

    void keyPressEvent(QKeyEvent *);                //  reaction for key press

    void allocate_memory();                         //  allocate memory for all graphical elements
    void init_palettes();                           //  set default values for all colors
    void init_images();                             //  load images into all cards
    void init_layouts();                            //  allocate memory for all layout and initialize them
    void init_menu();                               //  initialize all buttons from game screen
    void init_cards();                              //  initialize all cards from game screen
    void refresh_default_maximum_size(int &);       //  update maximum value of cards covered by another card
    void refresh_labels();                          //  refresh content of all tool tips
    void new_game();                                //  reset game progress to default state
                                                    //  below are redefinitions of pure virtual methods from core source files
    void refresh_sizes(int x)
    {
        refresh_default_maximum_size(x);            //  refresh maximum height of covered cards in concrete column

        cards[x][huge[x] - 2].setMinimumHeight(maximum_size[x]);
        cards[x][huge[x] - 2].setMaximumHeight(maximum_size[x]);

        for (int i = 13; i >= 0; i--)               //  find maximum card in concrete column
        {
            if (visible[x][i])                      //  check last visible card from bottom of concrete column
            {
                cards[x][i].setMinimumHeight(card_height);
                cards[x][i].setMaximumHeight(card_height);
                huge[x] = i + 2;                    //  update huge value for concrete column
                break;                              //  break when bottom card found
            }
        }
    }

    void refresh_card(int & x, int y, int type)     //  concrete coordinates of desired card are specified by arguments
    {
        if (type == 2)                              //  desired card is selected
        {                                           //  get color of desired card from colors array and refresh pixmap of that card
            if (colors[x][y] == hearts)
                cards[x][y].setPixmap(images_select[0][y - 1]);
            else if (colors[x][y] == diamonds)
                cards[x][y].setPixmap(images_select[1][y - 1]);
            else if (colors[x][y] == clubs)
                cards[x][y].setPixmap(images_select[2][y - 1]);
            else if (colors[x][y] == spades)
                cards[x][y].setPixmap(images_select[3][y - 1]);
        }
        else if (type)                              //  desired card is possible
        {                                           //  get color of desired card from colors array and refresh pixmap of that card
            if (colors[x][y] == hearts)
                cards[x][y].setPixmap(images_possible[0][y - 1]);
            else if (colors[x][y] == diamonds)
                cards[x][y].setPixmap(images_possible[1][y - 1]);
            else if (colors[x][y] == clubs)
                cards[x][y].setPixmap(images_possible[2][y - 1]);
            else if (colors[x][y] == spades)
                cards[x][y].setPixmap(images_possible[3][y - 1]);
            else
                cards[x][y].setPixmap(* image_empty_possible);
        }
        else                                        //  desired card is in default state
        {                                           //  get color of desired card from colors array and refresh pixmap of that card
            if (colors[x][y] == hearts)
                cards[x][y].setPixmap(images[0][y - 1]);
            else if (colors[x][y] == diamonds)
                cards[x][y].setPixmap(images[1][y - 1]);
            else if (colors[x][y] == clubs)
                cards[x][y].setPixmap(images[2][y - 1]);
            else if (colors[x][y] == spades)
                cards[x][y].setPixmap(images[3][y - 1]);
            else
                cards[x][y].setPixmap(* image_empty);
        }
    }

    void refresh_final_card(int x, int type)        //  concrete final card is indexed using argument
    {
        if (type == 2)                              //  desired card is selected
            final_card[x].setPixmap(images_select[final_stack[x].top().color - 1][final_stack[x].top().type - 1]);
        else if (type)                              //  desired card is possible
        {                                           //  update final card image using content of top card from concrete final stack
            if (final_stack[x].top().color == hearts)
                final_card[x].setPixmap(images_possible[0][final_stack[x].top().type - 1]);
            else if (final_stack[x].top().color == diamonds)
                final_card[x].setPixmap(images_possible[1][final_stack[x].top().type - 1]);
            else if (final_stack[x].top().color == clubs)
                final_card[x].setPixmap(images_possible[2][final_stack[x].top().type - 1]);
            else if (final_stack[x].top().color == spades)
                final_card[x].setPixmap(images_possible[3][final_stack[x].top().type - 1]);
            else
                final_card[x].setPixmap(* image_empty_possible);
        }
        else                                        //  desired card is in default state
        {                                           //  update final card image using content of top card from concrete final stack
            if (final_stack[x].top().color == hearts)
                final_card[x].setPixmap(images[0][final_stack[x].top().type - 1]);
            else if (final_stack[x].top().color == diamonds)
                final_card[x].setPixmap(images[1][final_stack[x].top().type - 1]);
            else if (final_stack[x].top().color == clubs)
                final_card[x].setPixmap(images[2][final_stack[x].top().type - 1]);
            else if (final_stack[x].top().color == spades)
                final_card[x].setPixmap(images[3][final_stack[x].top().type - 1]);
            else
                final_card[x].setPixmap(* image_empty);
        }
    }

    void refresh_stack_card(bool select)
    {
        if (select)                                 //  top stack card is selected
        {                                           //  update top stack card image using content of cards stack description stack
            if (cards_stack.top().color == hearts)
                top_stack_card->setPixmap(images_select[0][cards_stack.top().type - 1]);
            else if (cards_stack.top().color == diamonds)
                top_stack_card->setPixmap(images_select[1][cards_stack.top().type - 1]);
            else if (cards_stack.top().color == clubs)
                top_stack_card->setPixmap(images_select[2][cards_stack.top().type - 1]);
            else if (cards_stack.top().color == spades)
                top_stack_card->setPixmap(images_select[3][cards_stack.top().type - 1]);
        }
        else                                        //  top stack card is in default state
        {                                           //  update top stack card image using content of cards stack description stack
            if (cards_stack.size())
            {
                if (cards_stack.top().color == hearts)
                    top_stack_card->setPixmap(images[0][cards_stack.top().type - 1]);
                else if (cards_stack.top().color == diamonds)
                    top_stack_card->setPixmap(images[1][cards_stack.top().type - 1]);
                else if (cards_stack.top().color == clubs)
                    top_stack_card->setPixmap(images[2][cards_stack.top().type - 1]);
                else if (cards_stack.top().color == spades)
                    top_stack_card->setPixmap(images[3][cards_stack.top().type - 1]);
            }
            else
                top_stack_card->setPixmap(* image_empty);
        }
    }

    void refresh_cards_deck()
    {                                               //  update cards deck image by condition below
        if (cards_deck.size())                      //  cards deck is not empty
            next_card->setPixmap(* image_downturned);
        else                                        //  cards deck is empty
            next_card->setPixmap(* image_empty);
    }

    void show_card(int x, int y)
    {
        cards[x][y].show();                         //  show card specified by coordinates in arguments
    }

    void hide_card(int x, int y)
    {
        cards[x][y].hide();                         //  hide card specified by coordinates in arguments
    }

    void use_downturned_card(int & x, int column)
    {
        switch (column)                             //  select concrete column from cards array
        {
            case 1:                                 //  second column from cards array
                show_card(x, downturned_stack_1.top().type);    //  index concrete card using content of downturned card
                downturned_card_1->hide();          //  hide used downturned card
                break;
            case 2:                                 //  third column from cards array
                show_card(x, downturned_stack_2.top().type);    //  index concrete card using content of downturned card
                downturned_card_2[downturned_stack_2.size() - 1].hide();    //  hide used downturned card
                break;
            case 3:                                 //  fourth column from cards array
                show_card(x, downturned_stack_3.top().type);    //  index concrete card using content of downturned card
                downturned_card_3[downturned_stack_3.size() - 1].hide();    //  hide used downturned card
                break;
            case 4:                                 //  fifth column from cards array
                show_card(x, downturned_stack_4.top().type);    //  index concrete card using content of downturned card
                downturned_card_4[downturned_stack_4.size() - 1].hide();    //  hide used downturned card
                break;
            case 5:                                 //  sixth column from cards array
                show_card(x, downturned_stack_5.top().type);    //  index concrete card using content of downturned card
                downturned_card_5[downturned_stack_5.size() - 1].hide();    //  hide used downturned card
                break;
            default:                                //  seventh column from cards array
                show_card(x, downturned_stack_6.top().type);    //  index concrete card using content of downturned card
                downturned_card_6[downturned_stack_6.size() - 1].hide();    //  hide used downturned card
                break;
        }
    }

    void handle_win()
    {
    }

private slots:
    void click_reaction(int, int);                  //  slot to handle click on clickable label
    void enter_reaction(int, int);                  //  slot to handle enter event of clickable label
    void leave_reaction(int, int);                  //  slot to handle leave event of clickable label
};

#endif // KLONDIKE_H
