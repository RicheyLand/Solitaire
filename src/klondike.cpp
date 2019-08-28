#include "klondike.h"
#include "ui_klondike.h"

ClickableLabel::ClickableLabel(const QString & text, QWidget * parent) : QLabel(parent)
{
    setText(text);                                  //  load text into clickable label
}

void ClickableLabel::allow_mouse_tracking()
{
    this->setAttribute(Qt::WA_Hover, true);         //  allow mouse tracking of clickable label
    this->setMouseTracking(true);
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mousePressEvent(QMouseEvent * event)
{
    if (event->buttons())
        emit clicked(line, field);                  //  emit signal if mouse press event detected on clickable label
}

void ClickableLabel::enterEvent(QEvent * event)
{
    emit entered(line, field);                      //  emit signal if mouse cursor entered on clickable label
    event->ignore();
}

void ClickableLabel::leaveEvent(QEvent * event)
{
    emit leaved(line, field);                       //  emit signal if mouse cursor leaved clickable label
    event->ignore();
}

Klondike::Klondike(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Klondike)
{
    ui->setupUi(this);

    if (QApplication::desktop()->screenGeometry().width() >= 1280 && QApplication::desktop()->screenGeometry().height() >= 1024)
    {
        high_resolution = true;                     //  enable high resolution of window
        setMinimumHeight(930);                      //  increase height of window
        setMaximumHeight(930);
        help_screen_pixmap.load(":/resources/help_text.png");
        ui->help_image->setPixmap(help_screen_pixmap);  //  set help screen image
    }
    else
    {
        high_resolution = false;                    //  disable high resolution of window
        help_screen_pixmap.load(":/resources/help_text_2.png");
        ui->help_image->setPixmap(help_screen_pixmap);  //  set help screen image
    }

    for (int i = 0; i < 7; i++)                     //  initialize maximum sizes of covered cards and huge array
    {
        huge[i] = 13;
        maximum_size[i] = 55;
    }
                                                    //  initialize game by calling methods below
    init_variables();
    init_deck();
    allocate_memory();
    init_palettes();
    init_images();
    init_layouts();
    init_menu();
    init_cards();

    for (int i = 0; i < 7; i++)                     //  connect signals of cards from cards array with appropriate slots
    {
        for (int j = 0; j < 14; j++)
        {
            QObject::connect(&(cards[i][j]), SIGNAL(clicked(int,int)), this, SLOT(click_reaction(int, int)));
            QObject::connect(&(cards[i][j]), SIGNAL(entered(int,int)), this, SLOT(enter_reaction(int, int)));
            QObject::connect(&(cards[i][j]), SIGNAL(leaved(int,int)), this, SLOT(leave_reaction(int, int)));
        }
    }
                                                    //  connect signals of top cards from cards deck and cards stack with appropriate slots
    QObject::connect(next_card, SIGNAL(clicked(int,int)), this, SLOT(click_reaction(int, int)));
    QObject::connect(top_stack_card, SIGNAL(clicked(int,int)), this, SLOT(click_reaction(int, int)));
    QObject::connect(top_stack_card, SIGNAL(entered(int,int)), this, SLOT(enter_reaction(int, int)));
    QObject::connect(top_stack_card, SIGNAL(leaved(int,int)), this, SLOT(leave_reaction(int, int)));

    for (int i = 0; i < 4; i++)                     //  connect signals of final cards with appropriate slots
    {
        QObject::connect(final_card + i, SIGNAL(clicked(int,int)), this, SLOT(click_reaction(int, int)));
        QObject::connect(final_card + i, SIGNAL(entered(int,int)), this, SLOT(enter_reaction(int, int)));
        QObject::connect(final_card + i, SIGNAL(leaved(int,int)), this, SLOT(leave_reaction(int, int)));
    }
                                                    //  connect mouse click signals of all menu buttons with appropriate slots
    QObject::connect(new_game_button, SIGNAL(clicked(int,int)), this, SLOT(click_reaction(int,int)));
    QObject::connect(help_button, SIGNAL(clicked(int,int)), this, SLOT(click_reaction(int,int)));
    QObject::connect(about_button, SIGNAL(clicked(int,int)), this, SLOT(click_reaction(int,int)));
    QObject::connect(exit_button, SIGNAL(clicked(int,int)), this, SLOT(click_reaction(int,int)));
                                                    //  connect mouse cursor enter signals of all menu buttons with appropriate slots
    QObject::connect(new_game_button, SIGNAL(entered(int,int)), this, SLOT(enter_reaction(int,int)));
    QObject::connect(help_button, SIGNAL(entered(int,int)), this, SLOT(enter_reaction(int,int)));
    QObject::connect(about_button, SIGNAL(entered(int,int)), this, SLOT(enter_reaction(int,int)));
    QObject::connect(exit_button, SIGNAL(entered(int,int)), this, SLOT(enter_reaction(int,int)));
                                                    //  connect mouse cursor leave signals of all menu buttons with appropriate slots
    QObject::connect(new_game_button, SIGNAL(leaved(int,int)), this, SLOT(leave_reaction(int,int)));
    QObject::connect(help_button, SIGNAL(leaved(int,int)), this, SLOT(leave_reaction(int,int)));
    QObject::connect(about_button, SIGNAL(leaved(int,int)), this, SLOT(leave_reaction(int,int)));
    QObject::connect(exit_button, SIGNAL(leaved(int,int)), this, SLOT(leave_reaction(int,int)));

    ui->tabs->tabBar()->hide();                     //  hide tab bar of tab widget
    ui->tab->setLayout(ui->tab1_g_layout);          //  set layout for first tab of tab widget
    ui->tab_2->setLayout(ui->tab2_v_layout);        //  set layout for second tab of tab widget
    ui->tab_3->setLayout(ui->tab3_v_layout);        //  set layout for third tab of tab widget
    setLayout(main_v_layout);
}

Klondike::~Klondike()
{
    delete ui;
}

void Klondike::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if (ui->tabs->currentIndex() == 1)          //  game screen is active
            ui->tabs->setCurrentIndex(0);           //  show main menu
        else if (ui->tabs->currentIndex() == 2)     //  help screen is active
        {
            main_v_layout->setContentsMargins(4, 0, 4, 0);
            ui->tabs->setCurrentIndex(0);           //  show main menu
        }
        else                                        //  main menu is active
            ui->tabs->setCurrentIndex(1);           //  show game screen
    }

    event->ignore();
}

void Klondike::allocate_memory()
{
    cards = new ClickableLabel * [7];               //  allocate memory for all columns from cards array

    for (int i = 0; i < 7; i++)                     //  allocate memory for all cards of every column from cards array
        cards[i] = new ClickableLabel[14];

    downturned_card_1 = new QLabel;                 //  allocate memory for all downturned cards
    downturned_card_2 = new QLabel[2];
    downturned_card_3 = new QLabel[3];
    downturned_card_4 = new QLabel[4];
    downturned_card_5 = new QLabel[5];
    downturned_card_6 = new QLabel[6];

    next_card = new ClickableLabel;                 //  allocate memory for cards deck top card
    top_stack_card = new ClickableLabel;            //  allocate memory for top stack card
    final_card = new ClickableLabel[4];             //  allocate memory for final cards

    image_downturned = new QPixmap(":/resources/downturned.png");   //  load fixed images into appropriate cards
    image_empty = new QPixmap(":/resources/empty.png");
    image_empty_possible = new QPixmap(":/resources/emptyp.png");

    images = new QPixmap * [4];                     //  allocate memory for pixmaps of all columns from cards array

    for (int i = 0; i < 4; i++)                     //  allocate memory for pixmaps of all cards from every column of cards array
        images[i] = new QPixmap[13];

    images_select = new QPixmap * [4];              //  allocate memory for select pixmaps of all columns from cards array

    for (int i = 0; i < 4; i++)                     //  allocate memory for select pixmaps of all cards from every column of cards array
        images_select[i] = new QPixmap[13];

    images_possible = new QPixmap * [4];            //  allocate memory for possible pixmaps of all columns from cards array

    for (int i = 0; i < 4; i++)                     //  allocate memory for possible pixmaps of all cards from every column of cards array
        images_possible[i] = new QPixmap[13];

    separator = new QLabel("");                     //  allocate memory for cards separator
    new_game_button = new ClickableLabel;           //  allocate memory for main menu buttons
    help_button = new ClickableLabel;
    about_button = new ClickableLabel;
    exit_button = new ClickableLabel;
}

void Klondike::init_palettes()
{
    background_palette = new QPalette;              //  allocate memory for all palettes

    background_palette->setColor(QPalette::Background, QColor(172, 172, 172));  //  set colors of all palettes
    ui->tabs->setPalette(* background_palette);     //  set screen background color
    setPalette(* background_palette);               //  set dialog background color
}

void Klondike::init_images()
{                                                   //  initialize all standard images from project resources
    images[0][0].load(":/resources/Kh.png");
    images[0][1].load(":/resources/Qh.png");
    images[0][2].load(":/resources/Jh.png");
    images[0][3].load(":/resources/10h.png");
    images[0][4].load(":/resources/9h.png");
    images[0][5].load(":/resources/8h.png");
    images[0][6].load(":/resources/7h.png");
    images[0][7].load(":/resources/6h.png");
    images[0][8].load(":/resources/5h.png");
    images[0][9].load(":/resources/4h.png");
    images[0][10].load(":/resources/3h.png");
    images[0][11].load(":/resources/2h.png");
    images[0][12].load(":/resources/Ah.png");

    images[1][0].load(":/resources/Kd.png");
    images[1][1].load(":/resources/Qd.png");
    images[1][2].load(":/resources/Jd.png");
    images[1][3].load(":/resources/10d.png");
    images[1][4].load(":/resources/9d.png");
    images[1][5].load(":/resources/8d.png");
    images[1][6].load(":/resources/7d.png");
    images[1][7].load(":/resources/6d.png");
    images[1][8].load(":/resources/5d.png");
    images[1][9].load(":/resources/4d.png");
    images[1][10].load(":/resources/3d.png");
    images[1][11].load(":/resources/2d.png");
    images[1][12].load(":/resources/Ad.png");

    images[2][0].load(":/resources/Kc.png");
    images[2][1].load(":/resources/Qc.png");
    images[2][2].load(":/resources/Jc.png");
    images[2][3].load(":/resources/10c.png");
    images[2][4].load(":/resources/9c.png");
    images[2][5].load(":/resources/8c.png");
    images[2][6].load(":/resources/7c.png");
    images[2][7].load(":/resources/6c.png");
    images[2][8].load(":/resources/5c.png");
    images[2][9].load(":/resources/4c.png");
    images[2][10].load(":/resources/3c.png");
    images[2][11].load(":/resources/2c.png");
    images[2][12].load(":/resources/Ac.png");

    images[3][0].load(":/resources/Ks.png");
    images[3][1].load(":/resources/Qs.png");
    images[3][2].load(":/resources/Js.png");
    images[3][3].load(":/resources/10s.png");
    images[3][4].load(":/resources/9s.png");
    images[3][5].load(":/resources/8s.png");
    images[3][6].load(":/resources/7s.png");
    images[3][7].load(":/resources/6s.png");
    images[3][8].load(":/resources/5s.png");
    images[3][9].load(":/resources/4s.png");
    images[3][10].load(":/resources/3s.png");
    images[3][11].load(":/resources/2s.png");
    images[3][12].load(":/resources/As.png");
                                                //  initialize all select images from project resources
    images_select[0][0].load(":/resources/Khs.png");
    images_select[0][1].load(":/resources/Qhs.png");
    images_select[0][2].load(":/resources/Jhs.png");
    images_select[0][3].load(":/resources/10hs.png");
    images_select[0][4].load(":/resources/9hs.png");
    images_select[0][5].load(":/resources/8hs.png");
    images_select[0][6].load(":/resources/7hs.png");
    images_select[0][7].load(":/resources/6hs.png");
    images_select[0][8].load(":/resources/5hs.png");
    images_select[0][9].load(":/resources/4hs.png");
    images_select[0][10].load(":/resources/3hs.png");
    images_select[0][11].load(":/resources/2hs.png");
    images_select[0][12].load(":/resources/Ahs.png");

    images_select[1][0].load(":/resources/Kds.png");
    images_select[1][1].load(":/resources/Qds.png");
    images_select[1][2].load(":/resources/Jds.png");
    images_select[1][3].load(":/resources/10ds.png");
    images_select[1][4].load(":/resources/9ds.png");
    images_select[1][5].load(":/resources/8ds.png");
    images_select[1][6].load(":/resources/7ds.png");
    images_select[1][7].load(":/resources/6ds.png");
    images_select[1][8].load(":/resources/5ds.png");
    images_select[1][9].load(":/resources/4ds.png");
    images_select[1][10].load(":/resources/3ds.png");
    images_select[1][11].load(":/resources/2ds.png");
    images_select[1][12].load(":/resources/Ads.png");

    images_select[2][0].load(":/resources/Kcs.png");
    images_select[2][1].load(":/resources/Qcs.png");
    images_select[2][2].load(":/resources/Jcs.png");
    images_select[2][3].load(":/resources/10cs.png");
    images_select[2][4].load(":/resources/9cs.png");
    images_select[2][5].load(":/resources/8cs.png");
    images_select[2][6].load(":/resources/7cs.png");
    images_select[2][7].load(":/resources/6cs.png");
    images_select[2][8].load(":/resources/5cs.png");
    images_select[2][9].load(":/resources/4cs.png");
    images_select[2][10].load(":/resources/3cs.png");
    images_select[2][11].load(":/resources/2cs.png");
    images_select[2][12].load(":/resources/Acs.png");

    images_select[3][0].load(":/resources/Kss.png");
    images_select[3][1].load(":/resources/Qss.png");
    images_select[3][2].load(":/resources/Jss.png");
    images_select[3][3].load(":/resources/10ss.png");
    images_select[3][4].load(":/resources/9ss.png");
    images_select[3][5].load(":/resources/8ss.png");
    images_select[3][6].load(":/resources/7ss.png");
    images_select[3][7].load(":/resources/6ss.png");
    images_select[3][8].load(":/resources/5ss.png");
    images_select[3][9].load(":/resources/4ss.png");
    images_select[3][10].load(":/resources/3ss.png");
    images_select[3][11].load(":/resources/2ss.png");
    images_select[3][12].load(":/resources/Ass.png");
                                                //  initialize all possible images from project resources
    images_possible[0][0].load(":/resources/Khp.png");
    images_possible[0][1].load(":/resources/Qhp.png");
    images_possible[0][2].load(":/resources/Jhp.png");
    images_possible[0][3].load(":/resources/10hp.png");
    images_possible[0][4].load(":/resources/9hp.png");
    images_possible[0][5].load(":/resources/8hp.png");
    images_possible[0][6].load(":/resources/7hp.png");
    images_possible[0][7].load(":/resources/6hp.png");
    images_possible[0][8].load(":/resources/5hp.png");
    images_possible[0][9].load(":/resources/4hp.png");
    images_possible[0][10].load(":/resources/3hp.png");
    images_possible[0][11].load(":/resources/2hp.png");
    images_possible[0][12].load(":/resources/Ahp.png");

    images_possible[1][0].load(":/resources/Kdp.png");
    images_possible[1][1].load(":/resources/Qdp.png");
    images_possible[1][2].load(":/resources/Jdp.png");
    images_possible[1][3].load(":/resources/10dp.png");
    images_possible[1][4].load(":/resources/9dp.png");
    images_possible[1][5].load(":/resources/8dp.png");
    images_possible[1][6].load(":/resources/7dp.png");
    images_possible[1][7].load(":/resources/6dp.png");
    images_possible[1][8].load(":/resources/5dp.png");
    images_possible[1][9].load(":/resources/4dp.png");
    images_possible[1][10].load(":/resources/3dp.png");
    images_possible[1][11].load(":/resources/2dp.png");
    images_possible[1][12].load(":/resources/Adp.png");

    images_possible[2][0].load(":/resources/Kcp.png");
    images_possible[2][1].load(":/resources/Qcp.png");
    images_possible[2][2].load(":/resources/Jcp.png");
    images_possible[2][3].load(":/resources/10cp.png");
    images_possible[2][4].load(":/resources/9cp.png");
    images_possible[2][5].load(":/resources/8cp.png");
    images_possible[2][6].load(":/resources/7cp.png");
    images_possible[2][7].load(":/resources/6cp.png");
    images_possible[2][8].load(":/resources/5cp.png");
    images_possible[2][9].load(":/resources/4cp.png");
    images_possible[2][10].load(":/resources/3cp.png");
    images_possible[2][11].load(":/resources/2cp.png");
    images_possible[2][12].load(":/resources/Acp.png");

    images_possible[3][0].load(":/resources/Ksp.png");
    images_possible[3][1].load(":/resources/Qsp.png");
    images_possible[3][2].load(":/resources/Jsp.png");
    images_possible[3][3].load(":/resources/10sp.png");
    images_possible[3][4].load(":/resources/9sp.png");
    images_possible[3][5].load(":/resources/8sp.png");
    images_possible[3][6].load(":/resources/7sp.png");
    images_possible[3][7].load(":/resources/6sp.png");
    images_possible[3][8].load(":/resources/5sp.png");
    images_possible[3][9].load(":/resources/4sp.png");
    images_possible[3][10].load(":/resources/3sp.png");
    images_possible[3][11].load(":/resources/2sp.png");
    images_possible[3][12].load(":/resources/Asp.png");
}

void Klondike::init_layouts()
{
    main_v_layout = new QVBoxLayout;                //  allocate memory for all layouts
    up_h_layout = new QHBoxLayout;
    down_h_layout = new QHBoxLayout;
    down_v_layout = new QVBoxLayout[7];

    main_v_layout->setSpacing(0);                   //  initialize main vertical layout
    main_v_layout->setMargin(0);
    main_v_layout->setContentsMargins(4, 0, 4, 0);
    main_v_layout->addWidget(ui->tabs);

    for (int i = 0; i < 7; i++)                     //  initialize seven vertical layouts for columns of card from cards array
    {
        down_v_layout[i].setSpacing(0);
        down_h_layout->addLayout(down_v_layout + i);
    }

    ui->tab2_v_layout->addLayout(up_h_layout);      //  add upper and lower horizontal layout into layout of second tab from tabs widget
    ui->tab2_v_layout->addLayout(down_h_layout);
}

void Klondike::init_menu()
{
    new_game_pixmap.load(":/resources/restart_off.png");    //  initialize main menu images
    help_pixmap.load(":/resources/help_off.png");
    about_pixmap.load(":/resources/about_off.png");
    exit_pixmap.load(":/resources/exit_off.png");

    new_game_button->allow_mouse_tracking();        //  initialize new game button from main menu
    new_game_button->setAutoFillBackground(true);
    new_game_button->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    new_game_button->setFrameShape(QFrame::NoFrame);
    new_game_button->setLineWidth(0);
    new_game_button->line = 301;
    new_game_button->field = 301;
    new_game_button->setMinimumWidth(310);
    new_game_button->setMaximumWidth(310);
    new_game_button->setMinimumHeight(310);
    new_game_button->setMaximumHeight(310);
    new_game_button->setPixmap(new_game_pixmap);
    ui->tab1_g_layout->addWidget(new_game_button, 0, 0);
    ui->tab1_g_layout->setAlignment(new_game_button, Qt::AlignHCenter | Qt::AlignVCenter);

    help_button->allow_mouse_tracking();            //  initialize help button from main menu
    help_button->setAutoFillBackground(true);
    help_button->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    help_button->setFrameShape(QFrame::NoFrame);
    help_button->setLineWidth(0);
    help_button->line = 302;
    help_button->field = 302;
    help_button->setMinimumWidth(310);
    help_button->setMaximumWidth(310);
    help_button->setMinimumHeight(310);
    help_button->setMaximumHeight(310);
    help_button->setPixmap(help_pixmap);
    ui->tab1_g_layout->addWidget(help_button, 0, 1);
    ui->tab1_g_layout->setAlignment(help_button, Qt::AlignHCenter | Qt::AlignVCenter);

    about_button->allow_mouse_tracking();            //  initialize about button from main menu
    about_button->setAutoFillBackground(true);
    about_button->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    about_button->setFrameShape(QFrame::NoFrame);
    about_button->setLineWidth(0);
    about_button->line = 303;
    about_button->field = 303;
    about_button->setMinimumWidth(310);
    about_button->setMaximumWidth(310);
    about_button->setMinimumHeight(310);
    about_button->setMaximumHeight(310);
    about_button->setPixmap(about_pixmap);
    ui->tab1_g_layout->addWidget(about_button, 1, 0);
    ui->tab1_g_layout->setAlignment(about_button, Qt::AlignHCenter | Qt::AlignVCenter);

    exit_button->allow_mouse_tracking();            //  initialize exit button from main menu
    exit_button->setAutoFillBackground(true);
    exit_button->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    exit_button->setFrameShape(QFrame::NoFrame);
    exit_button->setLineWidth(0);
    exit_button->line = 304;
    exit_button->field = 304;
    exit_button->setMinimumWidth(310);
    exit_button->setMaximumWidth(310);
    exit_button->setMinimumHeight(310);
    exit_button->setMaximumHeight(310);
    exit_button->setPixmap(exit_pixmap);
    ui->tab1_g_layout->addWidget(exit_button, 1, 1);
    ui->tab1_g_layout->setAlignment(exit_button, Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tab1_g_layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);  //  set vertical alignment for buttons layout in main menu
}

void Klondike::init_cards()
{
    downturned_card_1->setAutoFillBackground(true);     //  initialize downturned card from second column of cards array
    downturned_card_1->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    downturned_card_1->setMinimumWidth(card_width);
    downturned_card_1->setMaximumWidth(card_width);
    downturned_card_1->setMinimumHeight(maximum_size[1]);
    downturned_card_1->setMaximumHeight(maximum_size[1]);
    downturned_card_1->setPixmap(* image_downturned);
    down_v_layout[1].addWidget(downturned_card_1);
    down_v_layout[1].setAlignment(downturned_card_1, Qt::AlignHCenter);

    for (int i = 0; i < 2; i++)                     //  initialize all downturned cards from third column of cards array
    {
        downturned_card_2[i].setAutoFillBackground(true);
        downturned_card_2[i].setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        downturned_card_2[i].setMinimumWidth(card_width);
        downturned_card_2[i].setMaximumWidth(card_width);
        downturned_card_2[i].setMinimumHeight(maximum_size[2]);
        downturned_card_2[i].setMaximumHeight(maximum_size[2]);
        downturned_card_2[i].setPixmap(* image_downturned);
        down_v_layout[2].addWidget(downturned_card_2 + i);
        down_v_layout[2].setAlignment(downturned_card_2 + i, Qt::AlignHCenter);
    }

    for (int i = 0; i < 3; i++)                     //  initialize all downturned cards from fourth column of cards array
    {
        downturned_card_3[i].setAutoFillBackground(true);
        downturned_card_3[i].setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        downturned_card_3[i].setMinimumWidth(card_width);
        downturned_card_3[i].setMaximumWidth(card_width);
        downturned_card_3[i].setMinimumHeight(maximum_size[3]);
        downturned_card_3[i].setMaximumHeight(maximum_size[3]);
        downturned_card_3[i].setPixmap(* image_downturned);
        down_v_layout[3].addWidget(downturned_card_3 + i);
        down_v_layout[3].setAlignment(downturned_card_3 + i, Qt::AlignHCenter);
    }

    for (int i = 0; i < 4; i++)                     //  initialize all downturned cards from fifth column of cards array
    {
        downturned_card_4[i].setAutoFillBackground(true);
        downturned_card_4[i].setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        downturned_card_4[i].setMinimumWidth(card_width);
        downturned_card_4[i].setMaximumWidth(card_width);
        downturned_card_4[i].setMinimumHeight(maximum_size[4]);
        downturned_card_4[i].setMaximumHeight(maximum_size[4]);
        downturned_card_4[i].setPixmap(* image_downturned);
        down_v_layout[4].addWidget(downturned_card_4 + i);
        down_v_layout[4].setAlignment(downturned_card_4 + i, Qt::AlignHCenter);
    }

    for (int i = 0; i < 5; i++)                     //  initialize all downturned cards from sixth column of cards array
    {
        downturned_card_5[i].setAutoFillBackground(true);
        downturned_card_5[i].setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        downturned_card_5[i].setMinimumWidth(card_width);
        downturned_card_5[i].setMaximumWidth(card_width);
        downturned_card_5[i].setMinimumHeight(maximum_size[5]);
        downturned_card_5[i].setMaximumHeight(maximum_size[5]);
        downturned_card_5[i].setPixmap(* image_downturned);
        down_v_layout[5].addWidget(downturned_card_5 + i);
        down_v_layout[5].setAlignment(downturned_card_5 + i, Qt::AlignHCenter);
    }

    for (int i = 0; i < 6; i++)                     //  initialize all downturned cards from seventh column of cards array
    {
        downturned_card_6[i].setAutoFillBackground(true);
        downturned_card_6[i].setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        downturned_card_6[i].setMinimumWidth(card_width);
        downturned_card_6[i].setMaximumWidth(card_width);
        downturned_card_6[i].setMinimumHeight(maximum_size[6]);
        downturned_card_6[i].setMaximumHeight(maximum_size[6]);
        downturned_card_6[i].setPixmap(* image_downturned);
        down_v_layout[6].addWidget(downturned_card_6 + i);
        down_v_layout[6].setAlignment(downturned_card_6 + i, Qt::AlignHCenter);
    }

    for (int i = 0; i < 7; i++)                     //  initialize all cards from cards array
    {
        for (int j = 0; j < 14; j++)
        {
            cards[i][j].setAutoFillBackground(true);
            cards[i][j].hide();
            visible[i][j] = false;
            cards[i][j].line = i;
            cards[i][j].field = j;
            cards[i][j].setAlignment(Qt::AlignHCenter | Qt::AlignTop);
            cards[i][j].setMinimumWidth(card_width);
            cards[i][j].setMaximumWidth(card_width);
            cards[i][j].setMinimumHeight(maximum_size[i]);
            cards[i][j].setMaximumHeight(maximum_size[i]);
            down_v_layout[i].addWidget(&(cards[i][j]));
            down_v_layout[i].setAlignment(&(cards[i][j]), Qt::AlignHCenter);
        }

        colors[i][0] = empty;                       //  first item from every column of cards array represents empty place for putting new cards

        down_v_layout[i].setAlignment(Qt::AlignTop);

        unsigned long N = cards_deck.size();

        cards[i][cards_deck[N - 1].type].show();
        visible[i][cards_deck[N - 1].type] = true;
        colors[i][cards_deck[N - 1].type] = cards_deck[N - 1].color;
        refresh_card(i, cards_deck[N - 1].type, 0);
        cards_deck.pop_back();

        refresh_sizes(i);                           //  refresh sizes and visibility of cards in actual column
    }

    next_card->setAutoFillBackground(true);         //  initialize top card of cards deck
    next_card->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    next_card->line = 15;
    next_card->field = 15;
    next_card->setMinimumWidth(card_width);
    next_card->setMaximumWidth(card_width);
    next_card->setMinimumHeight(card_height);
    next_card->setMaximumHeight(card_height);
    next_card->setText("Deck");
    next_card->setPixmap(* image_downturned);
    next_card->setToolTip("Stock left: 24\nRedeals left: 2");
    up_h_layout->addWidget(next_card);
    up_h_layout->setAlignment(next_card, Qt::AlignHCenter);

    top_stack_card->setAutoFillBackground(true);    //  initialize top stack card
    top_stack_card->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    top_stack_card->line = 20;
    top_stack_card->field = 20;
    top_stack_card->setMinimumWidth(card_width);
    top_stack_card->setMaximumWidth(card_width);
    top_stack_card->setMinimumHeight(card_height);
    top_stack_card->setMaximumHeight(card_height);
    top_stack_card->setPixmap(* image_empty);
    up_h_layout->addWidget(top_stack_card);
    up_h_layout->setAlignment(top_stack_card, Qt::AlignHCenter);

    separator->setAutoFillBackground(true);         //  initialize separator label
    separator->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    separator->setFrameShape(QFrame::NoFrame);
    separator->setLineWidth(0);
    separator->setMinimumWidth(card_width);
    separator->setMaximumWidth(card_width);
    separator->setMinimumHeight(card_height);
    separator->setMaximumHeight(card_height);

    up_h_layout->addWidget(separator);
    up_h_layout->setAlignment(separator, Qt::AlignHCenter);   //  set vertical alignment for buttons layout in game screen

    for (int i = 0; i < 4; i++)                     //  initialize all final cards
    {
        final_card[i].setAutoFillBackground(true);
        final_card[i].setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        final_card[i].line = 100 + i;
        final_card[i].field = 100 + i;
        final_card[i].setMinimumWidth(card_width);
        final_card[i].setMaximumWidth(card_width);
        final_card[i].setMinimumHeight(card_height);
        final_card[i].setMaximumHeight(card_height);
        final_card[i].setPixmap(* image_empty);
        up_h_layout->addWidget(&(final_card[i]));
        up_h_layout->setAlignment(&(final_card[i]), Qt::AlignHCenter);
    }
}

void Klondike::refresh_default_maximum_size(int & x)
{
    int N = 0;

    for (int i = 1; i < 14; i++)                    //  get number of visible cards in desired column
    {
        if (visible[x][i])
            N++;
    }

    if (x == 1)                                     //  increase number of visible cards by number of downturned cards in actual column
        N += downturned_stack_1.size();
    else if (x == 2)
        N += downturned_stack_2.size();
    else if (x == 3)
        N += downturned_stack_3.size();
    else if (x == 4)
        N += downturned_stack_4.size();
    else if (x == 5)
        N += downturned_stack_5.size();
    else if (x == 6)
        N += downturned_stack_6.size();

    int new_maximum = 0;                            //  new maximum height for covered card will be calculated

    switch (N)                                      //  set new maximum height of covered cards by number of cards in column
    {
        case 0:
        case 1:
            new_maximum = 55;
            break;
        case 2:
            new_maximum = 52;
            break;
        case 3:
            new_maximum = 49;
            break;
        case 4:
            new_maximum = 46;
            break;
        case 5:
            new_maximum = 43;
            break;
        case 6:
            new_maximum = 40;
            break;
        case 7:
            new_maximum = 38;
            break;
        case 8:
            new_maximum = 36;
            break;
        case 9:
            new_maximum = 34;
            break;
        case 10:
            new_maximum = 32;
            break;
        case 11:
            new_maximum = 30;
            break;
        case 12:
            new_maximum = 29;
            break;
        case 13:
            new_maximum = 28;
            break;
        case 14:
            new_maximum = 27;
            break;
        case 15:
            new_maximum = 26;
            break;
        case 16:
            new_maximum = 25;
            break;
        case 18:
            new_maximum = 24;
            break;
        default:
            new_maximum = 23;
            break;
    }

    if (high_resolution && new_maximum < 32)        //  covered cards can have bigger size if high resolution is active
        new_maximum = 32;

    if (maximum_size[x] != new_maximum)             //  old and new maximum heights of covered cards are different
    {
        for (int i = 0; i < 14; i++)                //  refresh maximum height of all covered cards from actual column
        {
            if (cards[x][i].maximumHeight() == maximum_size[x])
            {
                cards[x][i].setMinimumHeight(new_maximum);
                cards[x][i].setMaximumHeight(new_maximum);
            }
        }

        if (x == 1)                                 //  refresh maximum height of downturned card from second column of cards array
        {
            downturned_card_1->setMinimumHeight(new_maximum);
            downturned_card_1->setMaximumHeight(new_maximum);
        }

        if (x == 2)                                 //  refresh maximum height of downturned cards from third column of cards array
        {
            for (int i = 0; i < 2; i++)
            {
                downturned_card_2[i].setMinimumHeight(new_maximum);
                downturned_card_2[i].setMaximumHeight(new_maximum);
            }
        }

        if (x == 3)                                 //  refresh maximum height of downturned cards from fourth column of cards array
        {
            for (int i = 0; i < 3; i++)
            {
                downturned_card_3[i].setMinimumHeight(new_maximum);
                downturned_card_3[i].setMaximumHeight(new_maximum);
            }
        }

        if (x == 4)                                 //  refresh maximum height of downturned cards from fifth column of cards array
        {
            for (int i = 0; i < 4; i++)
            {
                downturned_card_4[i].setMinimumHeight(new_maximum);
                downturned_card_4[i].setMaximumHeight(new_maximum);
            }
        }

        if (x == 5)                                 //  refresh maximum height of downturned cards from sixth column of cards array
        {
            for (int i = 0; i < 5; i++)
            {
                downturned_card_5[i].setMinimumHeight(new_maximum);
                downturned_card_5[i].setMaximumHeight(new_maximum);
            }
        }

        if (x == 6)                                 //  refresh maximum height of downturned cards from seventh column of cards array
        {
            for (int i = 0; i < 6; i++)
            {
                downturned_card_6[i].setMinimumHeight(new_maximum);
                downturned_card_6[i].setMaximumHeight(new_maximum);
            }
        }

        maximum_size[x] = new_maximum;              //  update value on actual index in maximum sizes array
    }
}

void Klondike::refresh_labels()
{
    string value = "Stock left: ";                  //  tooltip content will be saved in string
    value += to_string(cards_deck.size());          //  get number of cards in cards deck
    value += "\nRedeals left: ";
    value += to_string(redeals_left);               //  get number of remaining redeals of cards deck

    QString temp = next_card->toolTip();
    string actual = temp.toUtf8().constData();
    temp = top_stack_card->toolTip();
    actual = actual + "\n" + temp.toUtf8().constData();

    if (actual != value)                            //  update tooltip only if string is different than old tooltip string
        next_card->setToolTip(value.c_str());
}

void Klondike::new_game()
{
    for (int i = 0; i < 7; i++)                     //  initialize huge values and maximum sizes of covered cards
    {
        huge[i] = 13;
        maximum_size[i] = card_small_height;
    }

    clear_cards();                                  //  initialize new game by calling these methods
    init_variables();
    init_deck();

    downturned_card_1->show();                      //  make all downturned cards visible

    for (int i = 0; i < 2; i++)
        downturned_card_2[i].show();

    for (int i = 0; i < 3; i++)
        downturned_card_3[i].show();

    for (int i = 0; i < 4; i++)
        downturned_card_4[i].show();

    for (int i = 0; i < 5; i++)
        downturned_card_5[i].show();

    for (int i = 0; i < 6; i++)
        downturned_card_6[i].show();

    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 14; j++)
        {
            cards[i][j].hide();                     //  hide all cards from cards array
            visible[i][j] = false;
            cards[i][j].setMinimumHeight(maximum_size[i]);
            cards[i][j].setMaximumHeight(maximum_size[i]);
        }

        colors[i][0] = empty;

        unsigned long N = cards_deck.size();

        cards[i][cards_deck[N - 1].type].show();    //  show one card from every column of cards array
        visible[i][cards_deck[N - 1].type] = true;
        colors[i][cards_deck[N - 1].type] = cards_deck[N - 1].color;
        refresh_card(i, cards_deck[N - 1].type, 0);
        cards_deck.pop_back();                      //  take card from cards deck

        refresh_sizes(i);                           //  refresh cards in current column
    }

    next_card->setPixmap(* image_downturned);       //  refresh image of top stack card, cards deck top card and all final cards
    top_stack_card->setPixmap(* image_empty);

    for (int i = 0; i < 4; i++)
        final_card[i].setPixmap(* image_empty);

    refresh_labels();
    ui->tabs->setCurrentIndex(1);                   //  activate game window in actual screen
}

void Klondike::click_reaction(int x, int y)
{
    if (x == 301)                                   //  new game button
    {
        new_game();                                 //  start new game without saving actual game progress
        return;
    }
    else if (x == 302)                              //  help button
    {
        main_v_layout->setContentsMargins(0, 0, 0, 0);
        ui->tabs->setCurrentIndex(2);               //  show help screen
        return;
    }
    else if (x == 303)                              //  about button
    {
        setWindowTitle("Version 1.0");              //  show version of application as window title
        return;
    }
    else if (x == 304)                              //  exit button
    {
        QApplication::quit();
        return;
    }

    handle_command(x, y);                           //  handle click on card by method from core
    refresh_labels();                               //  refresh content of tooltips
}

void Klondike::enter_reaction(int x, int)
{
    if (x == 301)                                   //  new game button
    {
        new_game_pixmap.load(":/resources/restart_on.png");
        new_game_button->setPixmap(new_game_pixmap);
    }
    else if (x == 302)                              //  help button
    {
        help_pixmap.load(":/resources/help_on.png");
        help_button->setPixmap(help_pixmap);
    }
    else if (x == 303)                              //  about button
    {
        about_pixmap.load(":/resources/about_on.png");
        about_button->setPixmap(about_pixmap);
    }
    else if (x == 304)                              //  exit button
    {
        exit_pixmap.load(":/resources/exit_on.png");
        exit_button->setPixmap(exit_pixmap);
    }
}

void Klondike::leave_reaction(int x, int)
{
    if (x == 301)                                   //  new game button
    {
        new_game_pixmap.load(":/resources/restart_off.png");
        new_game_button->setPixmap(new_game_pixmap);
    }
    else if (x == 302)                              //  help button
    {
        help_pixmap.load(":/resources/help_off.png");
        help_button->setPixmap(help_pixmap);
    }
    else if (x == 303)                              //  about button
    {
        about_pixmap.load(":/resources/about_off.png");
        about_button->setPixmap(about_pixmap);

        if (windowTitle() != "Solitaire")               //  reset window title
            setWindowTitle("Solitaire");
    }
    else if (x == 304)                              //  exit button
    {
        exit_pixmap.load(":/resources/exit_off.png");
        exit_button->setPixmap(exit_pixmap);
    }
}
