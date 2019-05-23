#include "core.h"

random_device RD;                                   //	initialize seed engine(used only once)
mt19937 RNG(RD());                                  //	initialize random number engine
uniform_int_distribution<int> UNI_cards(1, 13);     //	select interval for distribution(card type)
uniform_int_distribution<int> UNI_color(1, 4);      //	select interval for distribution(card color)
uniform_int_distribution<int> UNI_bool(0, 1);       //  select interval for distribution(bool)

void Core::init_variables()
{
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 14; j++)
        {
            visible[i][j] = false;                  //  all cards from cards array are hidden by default
            colors[i][j] = empty;                   //  all cards from cards array have no color by default
        }
    }

    game_over = false;                              //  set default value of flags
    selected = false;
    stack_selected = false;
    final_selected = false;
    redeals_left = 2;                               //  two redeals of cards deck are default
}

void Core::init_deck()
{
    int N = 0;

    do
    {
        int type = UNI_cards(RNG);                  //  generate card type
        int color = UNI_color(RNG);                 //  generate card color

        int found = false;                          //  holds if generated card is already cards deck

        for (int i = 0; i < N; i++)
        {
            if (cards_deck[i].type == type && cards_deck[i].color == color)
            {
                found = true;                       //  new card found in cards deck
                break;
            }
        }

        if (found)                                  //  new card found in cards deck
            continue;                               //  generate actual card again
        else
        {
            Card temp;
            temp.type = type;
            temp.color = color;
            cards_deck.push_back(temp);             //  push back actual card into cards deck
        }

        N = cards_deck.size();

    }while (N < 52);                                //  break when all 52 cards are generated

    downturned_stack_1.push(cards_deck[cards_deck.size() - 1]);     //  push downturned card into second column of cards array
    cards_deck.pop_back();                          //  remove last card from cards deck

    for (int i = 0; i < 2; i++)
    {
        downturned_stack_2.push(cards_deck[cards_deck.size() - 1]);     //  push downturned card into third column of cards array
        cards_deck.pop_back();                      //  remove last card from cards deck
    }

    for (int i = 0; i < 3; i++)
    {
        downturned_stack_3.push(cards_deck[cards_deck.size() - 1]);     //  push downturned card into fourth column of cards array
        cards_deck.pop_back();                      //  remove last card from cards deck
    }

    for (int i = 0; i < 4; i++)
    {
        downturned_stack_4.push(cards_deck[cards_deck.size() - 1]);     //  push downturned card into fifth column of cards array
        cards_deck.pop_back();                      //  remove last card from cards deck
    }

    for (int i = 0; i < 5; i++)
    {
        downturned_stack_5.push(cards_deck[cards_deck.size() - 1]);     //  push downturned card into sixth column of cards array
        cards_deck.pop_back();                      //  remove last card from cards deck
    }

    for (int i = 0; i < 6; i++)
    {
        downturned_stack_6.push(cards_deck[cards_deck.size() - 1]);     //  push downturned card into seventh column of cards array
        cards_deck.pop_back();                      //  remove last card from cards deck
    }

    for (int i = 0; i < 4; i++)
    {
        Card temp;
        temp.type = 14;                             //  this value allows pushing ace onto empty final place
        temp.color = empty;

        final_stack[i].push(temp);                  //  intialize all final cards
    }
}

void Core::clear_cards()
{
    cards_deck.clear();                             //  clear cards deck

    while (downturned_stack_1.size())               //  clear first downturned cards stack
        downturned_stack_1.pop();

    while (downturned_stack_2.size())               //  clear second downturned cards stack
        downturned_stack_2.pop();

    while (downturned_stack_3.size())               //  clear third downturned cards stack
        downturned_stack_3.pop();

    while (downturned_stack_4.size())               //  clear fourth downturned cards stack
        downturned_stack_4.pop();

    while (downturned_stack_5.size())               //  clear fifth downturned cards stack
        downturned_stack_5.pop();

    while (downturned_stack_6.size())               //  clear sixth downturned cards stack
        downturned_stack_6.pop();

    for (int i = 0; i < 4; i++)                     //  clear all final cards stacks
    {
        while (final_stack[i].size())
            final_stack[i].pop();
    }

    for (int i = 0; i < 4; i++)
    {
        Card temp;
        temp.type = 14;                             //  this value allows pushing ace onto empty final place
        temp.color = empty;

        final_stack[i].push(temp);                  //  intialize all final cards
    }

    while (cards_stack.size())                      //  clear cards stack
        cards_stack.pop();
}

void Core::check_win()
{
    if (final_stack[0].top().type == 1 && final_stack[1].top().type == 1 &&
        final_stack[2].top().type == 1 && final_stack[3].top().type == 1)   //  all final cards must be kings
    {
        game_over = true;                           //  set game over flag
        handle_win();                               //  custom reaction for game winning
    }
}

void Core::handle_command(int & x, int & y)
{
    if (game_over)                                  //  game is stopped after winning
        return;

    if (x == 15 && y == 15)                         //  cards deck selected
    {
        if (selected || stack_selected || final_selected)   //  getting next card is not possible if another card is selected
            return;

        int N = cards_deck.size();                  //  get number of remaining cards in cards deck

        if (N)                                      //  cards deck is not empty
        {
            cards_stack.push(cards_deck[N - 1]);    //  move next card from cards deck onto cards stack

            refresh_stack_card(false);

            cards_deck.pop_back();

            refresh_cards_deck();
        }
        else if (redeals_left && cards_stack.size())    //  cards deck is empty, but cards stack not and some redeals left
        {
            do
            {
                cards_deck.push_back(cards_stack.top());    //  move all cards from cards stack into cards deck
                cards_stack.pop();
            }while (cards_stack.size());

            refresh_stack_card(false);
            refresh_cards_deck();

            redeals_left--;                         //  cards deck is refreshed
        }
    }
    else if (x == 20 && y == 20)                    //  cards stack selected
    {
        if (selected || final_selected)             //  getting card from stack is not possible if another card is selected
            return;

        if (stack_selected)                         //  card on stack already selected
        {
            int N = possible.size();

            refresh_stack_card(false);              //  unselect stack card

            for (int i = 0; i < N; i++)             //  hide all highlighted possible cards
            {
                if (possible[i].x >= 100)
                    refresh_final_card(possible[i].x - 100, 0);
                else
                    refresh_card(possible[i].x, possible[i].y, 0);
            }

            stack_selected = false;                 //  unselect stack card
        }
        else                                        //  nothing is selected
        {
            if (cards_stack.size())                 //  cards stack is not empty
            {
                possible.clear();                   //  reset possbile cards

                for (int i = 0 ; i < 7; i++)        //  iterate through all cards in cards array
                {
                    for (int j = 13; j >= 0; j--)   //  iterate from bottom of column
                    {
                        if (visible[i][j])          //  card is visible
                        {
                            if (cards_stack.top().type == j + 1)    //  check if it is possible to move selected card on this card
                            {
                                bool allowed = false;   //  holds if move is possible ot not

                                if (colors[i][j] == empty)  //  cards array column is empty
                                    allowed = true;     //  move is possible
                                else if (colors[i][j] == hearts || colors[i][j] == diamonds)    //  colors of cards must be different
                                {
                                    if (cards_stack.top().color == clubs || cards_stack.top().color == spades)
                                        allowed = true;     //  move is possible
                                }
                                else                //  colors of cards must be different
                                {
                                    if (cards_stack.top().color == hearts || cards_stack.top().color == diamonds)
                                        allowed = true;     //  move is possible
                                }

                                if (allowed)        //  move is possible
                                {
                                    Coordinates card;
                                    card.x = i;
                                    card.y = j;

                                    possible.push_back(card);   //  push actual card into possible array
                                }
                            }

                            break;                  //  moving is possible only at last card in column
                        }
                    }
                }

                for (int i = 0; i < 4; i++)         //  iterate through final cards
                {
                    if (cards_stack.top().type + 1 == final_stack[i].top().type)    //  moving selected card on final card is possible
                    {
                        if (final_stack[i].top().color == empty || final_stack[i].top().color == cards_stack.top().color)
                        {
                            Coordinates card;
                            card.x = 100 + i;
                            card.y = 100 + i;

                            possible.push_back(card);
                        }
                    }
                }

                int N = possible.size();

                if (N)                              //  moving selected card is possible
                {
                    refresh_stack_card(true);       //  refresh top of cards stack to previous card in stack

                    for (int i = 0; i < N; i++)     //  iterate through all possible cards
                    {
                        if (possible[i].x >= 100)
                            refresh_final_card(possible[i].x - 100, 1);     //  highlight final card
                        else
                            refresh_card(possible[i].x, possible[i].y, 1);  //  highlight card from cards array
                    }

                    stack_selected = true;          //  stack is selected
                }
            }
        }
    }
    else if (x >= 100 && y >= 100)                  //  one of final stack cards selected
    {
        if (final_selected)                         //  final card already selected
        {                                           //  moving aces between final places is possible
            int N = possible.size();

            if (x == select.x)                      //  same selected final card selected again
            {
                refresh_final_card(x - 100, 0);

                for (int i = 0; i < N; i++)         //  hide all highlighted possible cards
                {
                    if (possible[i].x >= 100)
                        refresh_final_card(possible[i].x - 100, 0);
                    else
                        refresh_card(possible[i].x, possible[i].y, 0);
                }

                final_selected = false;             //  unselect final card
            }
            else                                    //  another final card selected
            {
                for (int i = 0; i < N; i++)         //  iterate through all possible cards
                {
                    if (possible[i].x == x)         //  selected card found in possible array
                    {
                        for (int j = 0; j < N; j++)     //  hide all highlighted possible cards
                        {
                            if (possible[j].x >= 100)
                                refresh_final_card(possible[j].x - 100, 0);
                            else
                                refresh_card(possible[j].x, possible[j].y, 0);
                        }

                        Card temp;
                        temp.type = final_stack[select.x - 100].top().type;
                        temp.color = final_stack[select.x - 100].top().color;

                        final_stack[x - 100].push(temp);    //  swap card between two selected final cards
                        final_stack[select.x - 100].pop();

                        refresh_final_card(x - 100, 0);     //  refresh cards after swap
                        refresh_final_card(select.x - 100, 0);

                        final_selected = false;     //  unselect final card

                        break;
                    }
                }
            }
        }
        else if (stack_selected)                    //  cards stack already selected
        {
            int N = possible.size();

            for (int i = 0; i < N; i++)             //  iterate through all possible cards
            {
                if (possible[i].x == x)             //  find selected card in possible array
                {
                    for (int j = 0; j < N; j++)     //  hide all highlighted possible cards
                    {
                        if (possible[j].x >= 100)
                            refresh_final_card(possible[j].x - 100, 0);
                        else
                            refresh_card(possible[j].x, possible[j].y, 0);
                    }

                    Card temp;                      //  move top stack card to one of final cards
                    temp.type = cards_stack.top().type;
                    temp.color = cards_stack.top().color;

                    final_stack[x - 100].push(temp);
                    cards_stack.pop();

                    refresh_final_card(x - 100, 0);     //  refresh cards after swap
                    refresh_stack_card(false);

                    stack_selected = false;         //  unselect stack

                    break;
                }
            }
        }
        else if (selected)                          //  card from cards array already selected
        {
            int N = possible.size();

            for (int i = 0; i < N; i++)             //  iterate through all possible cards
            {
                if (possible[i].x == x)             //  selected cards from cards array found in possible cards
                {
                    for (int j = 0; j < N; j++)     //  hide all highlighted possible cards
                    {
                        if (possible[j].x >= 100)
                            refresh_final_card(possible[j].x - 100, 0);
                        else
                            refresh_card(possible[j].x, possible[j].y, 0);
                    }

                    Card temp;                      //  move selected card from cards array to one of final cards
                    temp.type = select.y;
                    temp.color = colors[select.x][select.y];

                    hide_card(select.x, select.y);
                    visible[select.x][select.y] = false;    //  refresh cards array after swap
                    refresh_card(select.x, select.y, 0);

                    final_stack[x - 100].push(temp);

                    refresh_final_card(x - 100, 0);     //  refresh final card after swap

                    bool empty_flag = true;

                    for (int j = 0; j < 14; j++)    //  check if there is at least one card in actual column of cards array
                    {
                        if (visible[select.x][j])
                        {
                            empty_flag = false;
                            break;
                        }
                    }

                    if (empty_flag)                 //  there is no card in actual column
                    {
                        if (select.x == 1 && downturned_stack_1.size())     //  second column from cards array
                        {
                            use_downturned_card(select.x, 1);   //  load downturned card into column
                            visible[select.x][downturned_stack_1.top().type] = true;
                            colors[select.x][downturned_stack_1.top().type] = downturned_stack_1.top().color;
                            refresh_card(select.x, downturned_stack_1.top().type, 0);
                            downturned_stack_1.pop();
                        }
                        else if (select.x == 2 && downturned_stack_2.size())    //  third column from cards array
                        {
                            use_downturned_card(select.x, 2);   //  load downturned card into column
                            visible[select.x][downturned_stack_2.top().type] = true;
                            colors[select.x][downturned_stack_2.top().type] = downturned_stack_2.top().color;
                            refresh_card(select.x, downturned_stack_2.top().type, 0);
                            downturned_stack_2.pop();
                        }
                        else if (select.x == 3 && downturned_stack_3.size())    //  fourth column from cards array
                        {
                            use_downturned_card(select.x, 3);   //  load downturned card into column
                            visible[select.x][downturned_stack_3.top().type] = true;
                            colors[select.x][downturned_stack_3.top().type] = downturned_stack_3.top().color;
                            refresh_card(select.x, downturned_stack_3.top().type, 0);
                            downturned_stack_3.pop();
                        }
                        else if (select.x == 4 && downturned_stack_4.size())    //  fifth column from cards array
                        {
                            use_downturned_card(select.x, 4);   //  load downturned card into column
                            visible[select.x][downturned_stack_4.top().type] = true;
                            colors[select.x][downturned_stack_4.top().type] = downturned_stack_4.top().color;
                            refresh_card(select.x, downturned_stack_4.top().type, 0);
                            downturned_stack_4.pop();
                        }
                        else if (select.x == 5 && downturned_stack_5.size())    //  sixth column from cards array
                        {
                            use_downturned_card(select.x, 5);   //  load downturned card into column
                            visible[select.x][downturned_stack_5.top().type] = true;
                            colors[select.x][downturned_stack_5.top().type] = downturned_stack_5.top().color;
                            refresh_card(select.x, downturned_stack_5.top().type, 0);
                            downturned_stack_5.pop();
                        }
                        else if (select.x == 6 && downturned_stack_6.size())    //  sevent column from cards array
                        {
                            use_downturned_card(select.x, 6);   //  load downturned card into column
                            visible[select.x][downturned_stack_6.top().type] = true;
                            colors[select.x][downturned_stack_6.top().type] = downturned_stack_6.top().color;
                            refresh_card(select.x, downturned_stack_6.top().type, 0);
                            downturned_stack_6.pop();
                        }
                        else                        //  there is no available downturned card in actual column
                        {
                            show_card(select.x, 0);     //  show just empty place
                            visible[select.x][0] = true;
                            refresh_card(select.x, 0, 0);
                        }
                    }

                    refresh_sizes(select.x);

                    selected = false;               //  unselect card from cards array

                    break;
                }
            }
        }
        else                                        //  nothing is selected
        {
            if (final_stack[x - 100].size() > 1)
            {
                possible.clear();

                for (int i = 0; i < 7; i++)         //  iterate through all cards in cards array
                {
                    for (int j = 13; j >= 0; j--)   //  iterate from bottom of column
                    {
                        if (visible[i][j])          //  card is visible
                        {
                            if (final_stack[x - 100].top().type == j + 1)   //  possible to move one of final card to this card
                            {
                                bool allowed = false;

                                if (colors[i][j] == empty)  //  possible to move any card on empty place
                                {
                                    allowed = true;
                                }
                                else if (colors[i][j] == hearts || colors[i][j] == diamonds)    //  colors of cards must be different
                                {
                                    if (final_stack[x - 100].top().color == clubs || final_stack[x - 100].top().color == spades)
                                        allowed = true;
                                }
                                else                //  colors of cards must be different
                                {
                                    if (final_stack[x - 100].top().color == hearts || final_stack[x - 100].top().color == diamonds)
                                        allowed = true;
                                }

                                if (allowed)        //  move is possible
                                {
                                    Coordinates card;
                                    card.x = i;
                                    card.y = j;

                                    possible.push_back(card);
                                }
                            }

                            break;                  //  moving is possible only on card at bottom of column
                        }
                    }
                }

                for (int i = 0; i < 4; i++)         //  check if moving aces between final places is possible
                {
                    if (x - 100 != i)
                    {
                        if (final_stack[i].top().type == 14 && final_stack[x - 100].top().type == 13)
                        {
                            Coordinates card;
                            card.x = 100 + i;
                            card.y = 100 + i;

                            possible.push_back(card);
                        }
                    }
                }

                int N = possible.size();

                if (N)                              //  moving selected card is possible
                {
                    refresh_final_card(x - 100, 2);

                    for (int i = 0; i < N; i++)
                    {
                        if (possible[i].x >= 100)
                            refresh_final_card(possible[i].x - 100, 1);
                        else
                            refresh_card(possible[i].x, possible[i].y, 1);
                    }

                    select.x = x;                   //  load coordinates of selected final card into select variable
                    select.y = y;

                    final_selected = true;          //  final card is selected
                }
            }
        }
    }
    else                                            //  card from cards array selected
    {
        if (final_selected)                         //  one of final cards already selected
        {
            int N = possible.size();

            for (int i = 0; i < N; i++)             //  iterate through all possible cards
            {
                if (possible[i].x == x && possible[i].y == y)   //  selected card from cards array found in possible array
                {
                    for (int j = 0; j < N; j++)     //  hide all highlighted possible cards
                    {
                        if (possible[j].x >= 100)
                            refresh_final_card(possible[j].x - 100, 0);
                        else
                            refresh_card(possible[j].x, possible[j].y, 0);
                    }

                    visible[x][final_stack[select.x - 100].top().type] = true;  //  move final card into cards array
                    colors[x][final_stack[select.x - 100].top().type] = final_stack[select.x - 100].top().color;
                    show_card(x, final_stack[select.x - 100].top().type);
                    refresh_card(x, final_stack[select.x - 100].top().type, 0);

                    final_stack[select.x - 100].pop();

                    refresh_final_card(select.x - 100, 0);  //  refresh cards after swap

                    if (y == 0)                     //  handle moving king onto empty block
                    {
                        hide_card(x, 0);            //  remove empty place
                        visible[x][0] = false;
                        refresh_card(x, 0, 0);
                    }

                    refresh_sizes(x);
                    final_selected = false;         //  unselect final card

                    break;
                }
            }
        }
        else if (stack_selected)                    //  card on stack already selected
        {
            int N = possible.size();

            for (int i = 0; i < N; i++)             //  iterate through all possible cards
            {
                if (possible[i].x == x && possible[i].y == y)
                {
                    for (int j = 0; j < N; j++)     //  hide all highlighted possible cards
                    {
                        if (possible[j].x >= 100)
                            refresh_final_card(possible[j].x - 100, 0);
                        else
                            refresh_card(possible[j].x, possible[j].y, 0);
                    }

                    visible[x][cards_stack.top().type] = true;  //  move top stack card into cards array
                    colors[x][cards_stack.top().type] = cards_stack.top().color;
                    show_card(x, cards_stack.top().type);
                    refresh_card(x, cards_stack.top().type, 0);

                    cards_stack.pop();

                    refresh_stack_card(false);      //  refresh cards after swap

                    if (y == 0)                     //  handle moving king onto empty block
                    {
                        hide_card(x, 0);            //  remove empty place
                        visible[x][0] = false;
                        refresh_card(x, 0, 0);
                    }

                    refresh_sizes(x);
                    stack_selected = false;         //  unselect stack

                    break;
                }
            }
        }
        else if (selected)                          //  another card in cards array already selected
        {
            int N = possible.size();

            if (x == select.x && y == select.y)     //  already selected card selected again
            {
                refresh_card(x, y, 0);

                for (int i = 0; i < N; i++)         //  hide all highlighted possible cards
                {
                    if (possible[i].x >= 100)
                        refresh_final_card(possible[i].x - 100, 0);
                    else
                        refresh_card(possible[i].x, possible[i].y, 0);
                }

                selected = false;                   //  unselect card from cards array
            }
            else                                    //  different card from cards array selected
            {
                for (int i = 0; i < N; i++)         //  iterate through all possible cards
                {
                    if (possible[i].x == x && possible[i].y == y)   //  selected card found in possible array
                    {
                        refresh_card(select.x, select.y, 0);

                        for (int j = 0; j < N; j++)     //  hide all highlighted possible cards
                        {
                            if (possible[j].x >= 100)
                                refresh_final_card(possible[j].x - 100, 0);
                            else
                                refresh_card(possible[j].x, possible[j].y, 0);
                        }

                        int max = select.y;

                        for (int j = 13; j >= select.y; j--)    //  find most bottom card from column of already selected card
                        {
                            if (visible[select.x][j])
                            {
                                max = j;
                                break;
                            }
                        }
                                                    //  move all cards from already selected card to most bottom card to another column
                        for (int j = select.y; j <= max; j++)
                        {
                            visible[x][j] = true;
                            colors[x][j] = colors[select.x][j];
                            show_card(x, j);
                            refresh_card(x, j, 0);

                            visible[select.x][j] = false;
                            hide_card(select.x, j);
                        }

                        if (y == 0)                 //  handle moving king onto empty block
                        {
                            hide_card(x, 0);        //  remove empty place
                            visible[x][0] = false;
                            refresh_card(x, 0, 0);
                        }

                        bool empty_flag = true;

                        for (int i = 0; i < 14; i++)    //  check if there is at least one card in actual column of cards array
                        {
                            if (visible[select.x][i])
                            {
                                empty_flag = false;
                                break;
                            }
                        }

                        if (empty_flag)             //  there is no card in actual column
                        {
                            if (select.x == 1 && downturned_stack_1.size())     //  second column from cards array
                            {
                                use_downturned_card(select.x, 1);   //  load downturned card into column
                                visible[select.x][downturned_stack_1.top().type] = true;
                                colors[select.x][downturned_stack_1.top().type] = downturned_stack_1.top().color;
                                refresh_card(select.x, downturned_stack_1.top().type, 0);
                                downturned_stack_1.pop();
                            }
                            else if (select.x == 2 && downturned_stack_2.size())    //  third column from cards array
                            {
                                use_downturned_card(select.x, 2);   //  load downturned card into column
                                visible[select.x][downturned_stack_2.top().type] = true;
                                colors[select.x][downturned_stack_2.top().type] = downturned_stack_2.top().color;
                                refresh_card(select.x, downturned_stack_2.top().type, 0);
                                downturned_stack_2.pop();
                            }
                            else if (select.x == 3 && downturned_stack_3.size())    //  fourth column from cards array
                            {
                                use_downturned_card(select.x, 3);   //  load downturned card into column
                                visible[select.x][downturned_stack_3.top().type] = true;
                                colors[select.x][downturned_stack_3.top().type] = downturned_stack_3.top().color;
                                refresh_card(select.x, downturned_stack_3.top().type, 0);
                                downturned_stack_3.pop();
                            }
                            else if (select.x == 4 && downturned_stack_4.size())    //  fifth column from cards array
                            {
                                use_downturned_card(select.x, 4);   //  load downturned card into column
                                visible[select.x][downturned_stack_4.top().type] = true;
                                colors[select.x][downturned_stack_4.top().type] = downturned_stack_4.top().color;
                                refresh_card(select.x, downturned_stack_4.top().type, 0);
                                downturned_stack_4.pop();
                            }
                            else if (select.x == 5 && downturned_stack_5.size())    //  sixth column from cards array
                            {
                                use_downturned_card(select.x, 5);   //  load downturned card into column
                                visible[select.x][downturned_stack_5.top().type] = true;
                                colors[select.x][downturned_stack_5.top().type] = downturned_stack_5.top().color;
                                refresh_card(select.x, downturned_stack_5.top().type, 0);
                                downturned_stack_5.pop();
                            }
                            else if (select.x == 6 && downturned_stack_6.size())    //  seventh column from cards array
                            {
                                use_downturned_card(select.x, 6);   //  load downturned card into column
                                visible[select.x][downturned_stack_6.top().type] = true;
                                colors[select.x][downturned_stack_6.top().type] = downturned_stack_6.top().color;
                                refresh_card(select.x, downturned_stack_6.top().type, 0);
                                downturned_stack_6.pop();
                            }
                            else                    //  there is no available downturned card in actual column
                            {
                                show_card(select.x, 0);     //  show just empty place
                                visible[select.x][0] = true;
                                refresh_card(select.x, 0, 0);
                            }
                        }

                        refresh_sizes(x);
                        refresh_sizes(select.x);
                        selected = false;           //  unselect card from cards array

                        break;
                    }
                }
            }
        }
        else                                        //  nothing is selected
        {
            if (y == 0)                             //  ignore empty block click
                return;

            select.x = x;                           //  load coordinates of selected card into select variable
            select.y = y;
            possible.clear();

            for (int i = 0 ; i < 7; i++)            //  iterate through all cards in cards array
            {
                if (i != x)
                {
                    for (int j = 13; j >= 0; j--)   //  iterate from bottom of column
                    {
                        if (visible[i][j])          //  card is visible
                        {
                            if (y == j + 1)         //  check if it possible to move seleted card to actual card
                            {
                                bool allowed = false;

                                if (colors[i][j] == empty)      //  possible to move any card on empty place
                                {
                                    allowed = true;
                                }
                                else if (colors[i][j] == hearts || colors[i][j] == diamonds)    //  colors of cards must be different
                                {
                                    if (colors[x][y] == clubs || colors[x][y] == spades)
                                        allowed = true;
                                }
                                else                //  colors of cards must be different
                                {
                                    if (colors[x][y] == hearts || colors[x][y] == diamonds)
                                        allowed = true;
                                }

                                if (allowed)        //  move is possible
                                {
                                    Coordinates card;
                                    card.x = i;
                                    card.y = j;

                                    possible.push_back(card);
                                }
                            }

                            break;                  //  possible to move selected card only on bottom card of column
                        }
                    }
                }
            }

            bool last_card = true;

            for (int i = y + 1; i < 14; i++)        //  start searching from card after selected card
            {
                if (visible[x][i])                  //  only bottom card from column can be moved to final stack
                {
                    last_card = false;
                    break;
                }

            }

            if (last_card)                          //  selected card is last card
            {
                for (int i = 0; i < 4; i++)         //  check if moving aces between final places is possible
                {
                    if (final_stack[i].top().type == y + 1)
                    {
                        if (final_stack[i].top().color == empty || final_stack[i].top().color == colors[x][y])
                        {
                            Coordinates card;
                            card.x = 100 + i;
                            card.y = 100 + i;

                            possible.push_back(card);
                        }
                    }
                }
            }

            int N = possible.size();

            if (N)                                  //  moving selected card is possible
            {
                refresh_card(x, y, 2);

                for (int i = 0; i < N; i++)         //  highlight all possible card
                {
                    if (possible[i].x >= 100)
                        refresh_final_card(possible[i].x - 100, 1);
                    else
                        refresh_card(possible[i].x, possible[i].y, 1);
                }

                selected = true;                    //  card from cards array is selected
            }
        }
    }

    check_win();                                    //  check winning of game
}
