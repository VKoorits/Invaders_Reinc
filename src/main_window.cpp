#include "main_window.hpp"

#include "game_widget.hpp"
#include "gameclasses/constants.hpp"

#include <QFile>
#include <algorithm>

#include <pugixml.hpp>
#include <json/json.h>
#include <json/writer.h>
#include <fstream>
#include <iostream>

// WINDOW
MainWindow::MainWindow()
{
  // size
  m_size = QSize(0,0);

  // QStyle
  QFile styleFile("data/styles/default.qss");
  if (styleFile.open(QFile::ReadOnly))
  {
    m_style = QLatin1String(styleFile.readAll());
    std::cout << "style file opened" << std::endl;
  }
  else std::cout << "style file not opened" << std::endl;

  // Stacked widget
  m_widgetStacked = new QStackedWidget(this);

  // window settings
  this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
  this->setWindowIcon(QIcon(WINDOW_ICON_FOLDER));
  this->setWindowState(Qt::WindowState::WindowActive);
  this->setStyleSheet(m_style);
  this->setCentralWidget(m_widgetStacked);

  // FOR MAIN MENU
  // buttons
  m_pbMenuNewGame = new QPushButton();
  connect(m_pbMenuNewGame, SIGNAL(clicked(bool)), this, SLOT(NewGame()));
  m_pbMenuContinueGame = new QPushButton();
  connect(m_pbMenuContinueGame, SIGNAL(clicked(bool)), this, SLOT(ContinueGame()));
  m_pbMenuRecord = new QPushButton();
  connect(m_pbMenuRecord, SIGNAL(clicked(bool)), this, SLOT(CheckoutToRecords()) );
  m_pbToSet = new QPushButton();
  connect(m_pbToSet, SIGNAL(clicked(bool)), this, SLOT(CheckoutToSettings()));
  m_pbExit = new QPushButton();
  connect(m_pbExit, &QAbstractButton::clicked, [this]()
    {
      if (SOUND_MENU_ON) m_soundButtonClick->play();
      ShowDialog(DIALOG_ON_SUBMIT_CLOSE, DialogTypes::OnSubmitClose);
    });
  m_pbMenuNewGame->setIcon(QIcon("data/images/icons/begin.ico"));
  m_pbExit->setObjectName("ExitButton");
  m_pbExit->setIcon(QIcon("data/images/icons/end.ico"));

  // fillers header and footer
  QWidget *topFiller = new QWidget;
  topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  QWidget *bottomFiller = new QWidget;
  bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // layout
  m_layoutMenu = new QGridLayout;
  m_layoutMenu->addWidget(topFiller, 0, 0, 1, 3);
  m_layoutMenu->addWidget(m_pbMenuNewGame, 1, 1, 1, 1);
  m_layoutMenu->addWidget(m_pbMenuContinueGame, 2, 1, 1, 1);
  m_layoutMenu->addWidget(m_pbToSet, 3, 1, 1, 1);
  m_layoutMenu->addWidget(m_pbMenuRecord, 4, 1, 1, 1);
  m_layoutMenu->addWidget(m_pbExit, 6, 1, 1, 1);
  m_layoutMenu->addWidget(bottomFiller, 7, 0, 1, 3);

  // widget
  m_widgetMenu = new QWidget(m_widgetStacked);
  m_widgetMenu->setLayout(m_layoutMenu);

  // FOR SETTINGS
  // buttons
  m_pbToMenu = new QPushButton();
  connect(m_pbToMenu, SIGNAL(clicked(bool)), this, SLOT(CheckoutToMenu()));
  m_pbToMenu->setFixedWidth(174);
  m_pbSaveSettings = new QPushButton();
  connect(m_pbSaveSettings, SIGNAL(clicked(bool)), this, SLOT(SaveSettings()));
  m_pbLoadSettings = new QPushButton();
  connect(m_pbLoadSettings, SIGNAL(clicked(bool)), this, SLOT(LoadSettings()));
  m_pbSetDefault = new QPushButton();
  connect(m_pbSetDefault, &QAbstractButton::clicked, [this]()
    {
      if (SOUND_MENU_ON) m_soundButtonClick->play();
      m_settingsChanged = true;
      SetDefaultSettings();
    });

  // QLabels
    // Control
  m_lControlComment = new QLabel();
  m_lControlGunMoveLeft = new QLabel();
  m_lControlGunMoveRight = new QLabel();
  m_lControlGunShoot = new QLabel();
  m_lControlGamePause = new QLabel();
    // Game parameters
  m_lGameParamComment = new QLabel();
  m_lGPAliensCount = new QLabel();
  m_lGPObstacleCount = new QLabel();
  m_lGPObstacleRedraw = new QLabel();
  m_lGPGunStartLives = new QLabel();
  m_lGPGunAddLive = new QLabel();
    // Main
  m_lWindowComment = new QLabel();
  m_lWindowSize = new QLabel();
  m_lWindowState = new QLabel();
  m_lLanguage = new QLabel();
    // Sound
  m_lSoundComment = new QLabel();
  m_lSMainOn = new QLabel();
  m_lSMainVolume = new QLabel();
  m_lSGameOn = new QLabel();
  m_lSGameVolume = new QLabel();

  // Work with Values and Keys
    // Control
  m_kseGunMoveLeft = new QKeySequenceEdit;
  connect(m_kseGunMoveLeft, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(ChangeShortcutGunMoveLeft(QKeySequence)));
  m_kseGunMoveRight = new QKeySequenceEdit;
  connect(m_kseGunMoveRight, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(ChangeShortcutGunMoveRight(QKeySequence)));
  m_kseGunShoot = new QKeySequenceEdit;
  connect(m_kseGunShoot, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(ChangeShortcutGunShoot(QKeySequence)));
  m_kseGamePause = new QKeySequenceEdit;
  connect(m_kseGamePause, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(ChangeShortcutGamePause(QKeySequence)));
    // Game parameters
  m_slGPAliensCount = new QSlider(Qt::Horizontal);
  m_slGPAliensCount->setRange(20, 70);
  m_slGPAliensCount->setPageStep(5);
  m_slGPAliensCount->setSingleStep(5);
  connect(m_slGPAliensCount, SIGNAL(valueChanged(int)), this, SLOT(ChangeAliensCount(int)));

  m_slGPObstacleCount = new QSlider(Qt::Horizontal);
  m_slGPObstacleCount->setRange(0, 6);
  m_slGPObstacleCount->setPageStep(1);
  connect(m_slGPObstacleCount, SIGNAL(valueChanged(int)), this, SLOT(ChangeObstacleCount(int)));

  m_slGPGunStartLives = new QSlider(Qt::Horizontal);
  m_slGPGunStartLives->setRange(1, 5);
  m_slGPGunStartLives->setPageStep(1);
  connect(m_slGPGunStartLives, SIGNAL(valueChanged(int)), this, SLOT(ChangeGunStartLives(int)));

  m_chbGPObstacleRedraw = new QCheckBox;
  connect(m_chbGPObstacleRedraw, SIGNAL(clicked(bool)), this, SLOT(ChangeObstacleRedrawState(bool)));

  m_chbGPGunAddLive = new QCheckBox;
  connect(m_chbGPGunAddLive, SIGNAL(clicked(bool)), this, SLOT(ChangeGunAddLiveState(bool)));
    // Main
  m_cbWindowState = new QComboBox();
  m_cbWindowState->addItem("Full screen", GameWindowStateTypes::FullScreen);
  m_cbWindowState->addItem("Minimized window", GameWindowStateTypes::MinimizedWindow);
  m_cbWindowState->addItem("Maximized window", GameWindowStateTypes::MaximizedWindow);
  connect(m_cbWindowState, SIGNAL(activated(int)), this, SLOT(ChangeWindowState(int)));

  m_cbWindowSize = new QComboBox();
  m_cbWindowSize->addItem("800x600", GameResolutionTypes::Size800x600);
  m_cbWindowSize->addItem("1024x768", GameResolutionTypes::Size1024x768);
  m_cbWindowSize->addItem("1280x720", GameResolutionTypes::Size1280x720);
  m_cbWindowSize->addItem("1280x1024", GameResolutionTypes::Size1280x1024);
  m_cbWindowSize->addItem("1360x768", GameResolutionTypes::Size1360x768);
  m_cbWindowSize->addItem("1366x768", GameResolutionTypes::Size1366x768);
  m_cbWindowSize->addItem("1400x1050", GameResolutionTypes::Size1400x1050);
  m_cbWindowSize->addItem("1440x900", GameResolutionTypes::Size1440x900);
  m_cbWindowSize->addItem("1600x900", GameResolutionTypes::Size1600x900);
  m_cbWindowSize->addItem("1680x1050", GameResolutionTypes::Size1680x1050);
  m_cbWindowSize->addItem("1920x1080", GameResolutionTypes::Size1920x1080);
  connect(m_cbWindowSize, SIGNAL(activated(int)), this, SLOT(ChangeResolution(int)));

  m_cbLanguage = new QComboBox();
  m_cbLanguage->addItem("English", GameLanguages::English);
  m_cbLanguage->addItem("Russian", GameLanguages::Russian);
  connect(m_cbLanguage, SIGNAL(activated(int)), this, SLOT(ChangeLanguage(int)));
    // Sound
  m_slSMenuVolume = new QSlider(Qt::Horizontal);
  m_slSMenuVolume->setRange(0, 100);
  m_slSMenuVolume->setPageStep(1);
  m_slSMenuVolume->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  connect(m_slSMenuVolume, SIGNAL(valueChanged(int)), this, SLOT(ChangeSoundMenuVolume(int)));

  m_slSGameVolume = new QSlider(Qt::Horizontal);
  m_slSGameVolume->setRange(0, 100);
  m_slSGameVolume->setPageStep(1);
  m_slSGameVolume->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  connect(m_slSGameVolume, SIGNAL(valueChanged(int)), this, SLOT(ChangeSoundGameVolume(int)));

  m_chbSMenuOn = new QCheckBox;
  connect(m_chbSMenuOn, SIGNAL(clicked(bool)), this, SLOT(ChangeSoundMenuOn(bool)));

  m_chbSGameOn = new QCheckBox;
  connect(m_chbSGameOn, SIGNAL(clicked(bool)), this, SLOT(ChangeSoundGameOn(bool)));

  // QHBoxLayouts
    // control
  QHBoxLayout * hbControlGunMoveLeft = new QHBoxLayout;
  hbControlGunMoveLeft->addWidget(m_lControlGunMoveLeft);
  hbControlGunMoveLeft->addWidget(m_kseGunMoveLeft);
  QHBoxLayout * hbControlGunMoveRight = new QHBoxLayout;
  hbControlGunMoveRight->addWidget(m_lControlGunMoveRight);
  hbControlGunMoveRight->addWidget(m_kseGunMoveRight);
  QHBoxLayout * hbControlGunShoot = new QHBoxLayout;
  hbControlGunShoot->addWidget(m_lControlGunShoot);
  hbControlGunShoot->addWidget(m_kseGunShoot);
  QHBoxLayout * hbControlPause = new QHBoxLayout;
  hbControlPause->addWidget(m_lControlGamePause);
  hbControlPause->addWidget(m_kseGamePause);
    // game param
  QHBoxLayout * hbGPAlienCount = new QHBoxLayout;
  hbGPAlienCount->addWidget(m_lGPAliensCount);
  hbGPAlienCount->addWidget(m_slGPAliensCount);
  QHBoxLayout * hbGPObstacleCount = new QHBoxLayout;
  hbGPObstacleCount->addWidget(m_lGPObstacleCount);
  hbGPObstacleCount->addWidget(m_slGPObstacleCount);
  QHBoxLayout * hbGPObstacleRedraw = new QHBoxLayout;
  hbGPObstacleRedraw->addWidget(m_lGPObstacleRedraw);
  hbGPObstacleRedraw->addWidget(m_chbGPObstacleRedraw);
  QHBoxLayout * hbGPGunStartLives = new QHBoxLayout;
  hbGPGunStartLives->addWidget(m_lGPGunStartLives);
  hbGPGunStartLives->addWidget(m_slGPGunStartLives);
  QHBoxLayout * hbGPGunAddLive = new QHBoxLayout;
  hbGPGunAddLive->addWidget(m_lGPGunAddLive);
  hbGPGunAddLive->addWidget(m_chbGPGunAddLive);
    // window
  QHBoxLayout * hbWindowSize = new QHBoxLayout;
  hbWindowSize->addWidget(m_lWindowSize);
  hbWindowSize->addWidget(m_cbWindowSize);
  QHBoxLayout * hbWindowState = new QHBoxLayout;
  hbWindowState->addWidget(m_lWindowState);
  hbWindowState->addWidget(m_cbWindowState);
  QHBoxLayout * hbLanguage = new QHBoxLayout;
  hbLanguage->addWidget(m_lLanguage);
  hbLanguage->addWidget(m_cbLanguage);
    // Sound
  QHBoxLayout * hbSoundMain = new QHBoxLayout;
  hbSoundMain->addWidget(m_lSMainOn);
  hbSoundMain->addWidget(m_chbSMenuOn);
  QHBoxLayout * hbSoundMainVolume = new QHBoxLayout;
  hbSoundMainVolume->addWidget(m_lSMainVolume);
  hbSoundMainVolume->addWidget(m_slSMenuVolume);
  QHBoxLayout * hbSoundGame = new QHBoxLayout;
  hbSoundGame->addWidget(m_lSGameOn);
  hbSoundGame->addWidget(m_chbSGameOn);
  QHBoxLayout * hbSoundGameVolume = new QHBoxLayout;
  hbSoundGameVolume->addWidget(m_lSGameVolume);
  hbSoundGameVolume->addWidget(m_slSGameVolume);

  // layout
  m_layoutSettings = new QGridLayout;
  m_layoutSettings->addWidget(m_pbToMenu, 0, 0);
    // control
  m_layoutSettings->addWidget(m_lControlComment, 2, 0, 1, 2);
  m_layoutSettings->addLayout(hbControlGunMoveLeft, 3, 0, 1, 2);
  m_layoutSettings->addLayout(hbControlGunMoveRight, 4, 0, 1, 2);
  m_layoutSettings->addLayout(hbControlGunShoot, 5, 0, 1, 2);
  m_layoutSettings->addLayout(hbControlPause, 6, 0, 1, 2);
    // game param
  m_layoutSettings->addWidget(m_lGameParamComment, 2, 2, 1, 2);
  m_layoutSettings->addLayout(hbGPAlienCount, 3, 2, 1, 2);
  m_layoutSettings->addLayout(hbGPObstacleCount, 4, 2, 1, 2);
  m_layoutSettings->addLayout(hbGPObstacleRedraw, 5, 2, 1, 2);
  m_layoutSettings->addLayout(hbGPGunStartLives, 6, 2, 1, 2);
  m_layoutSettings->addLayout(hbGPGunAddLive, 7, 2, 1, 2);
    // window
  m_layoutSettings->addWidget(m_lWindowComment, 8, 2, 1, 2);
  m_layoutSettings->addLayout(hbWindowSize, 9, 2, 1, 2);
  m_layoutSettings->addLayout(hbWindowState, 10, 2, 1, 2);
  m_layoutSettings->addLayout(hbLanguage, 11, 2, 1, 2);
    // sound
  m_layoutSettings->addWidget(m_lSoundComment, 7, 0, 1, 2);
  m_layoutSettings->addLayout(hbSoundMain, 8, 0, 1, 2);
  m_layoutSettings->addLayout(hbSoundMainVolume, 9, 0, 1, 2);
  m_layoutSettings->addLayout(hbSoundGame, 10, 0, 1, 2);
  m_layoutSettings->addLayout(hbSoundGameVolume, 11, 0, 1, 2);
    // buttons
  m_layoutSettings->addWidget(m_pbSaveSettings, 12, 0);
  m_layoutSettings->addWidget(m_pbLoadSettings, 12, 1);
  m_layoutSettings->addWidget(m_pbSetDefault, 12, 2);
  m_layoutSettings->addWidget(bottomFiller, 13, 0, 1, 4);

  // widget
  m_widgetSettings = new QWidget(m_widgetStacked);
  m_widgetSettings->setLayout(m_layoutSettings);
  m_widgetSettings->hide();

  // GAME
  m_windowGame = new GameWindow(m_widgetStacked, this);

  // RECORDS
    // buttons
  m_pbToMenuFromRecords = new QPushButton();
  connect(m_pbToMenuFromRecords, SIGNAL(clicked(bool)), this, SLOT(CheckoutToMenu()));
    // QLabels
  m_table = new QLabel();
    // QVBoxLayout
  m_recordsVLayout = new QVBoxLayout;
  m_recordsVLayout->addWidget(m_pbToMenuFromRecords);
  m_recordsVLayout->addWidget(m_table);
    // widget
  m_widgetRecords = new QWidget;
  m_widgetRecords->setLayout(m_recordsVLayout);
    // default records init
  ReadJsonRecords(recordsArray);
  CreateRecordTable();

  // fill stackedWidget
  m_widgetStacked->addWidget(m_widgetMenu);
  m_widgetStacked->addWidget(m_widgetSettings);
  m_widgetStacked->addWidget(m_windowGame);
  m_widgetStacked->addWidget(m_widgetRecords);
  m_widgetStacked->setCurrentIndex(0);

  // QShortcuts
  m_shortcutGamePause = new QShortcut(m_widgetStacked);
  connect(m_shortcutGamePause, SIGNAL(activated()), this, SLOT(ShortcutPause()));

  // QSoundEffect
  InitSound();

  // default button settings (game not started)
  ShowMenuItems();

  // load all user settings from file
  // load default and then load from file
  SetDefaultSettings();

  if (!ReadXml()) ReadJson();
  m_settingsChanged = false;
}

MainWindow::~MainWindow()
{
  delete m_soundButtonClick;
}

void MainWindow::InitSound()
{
  m_soundButtonClick = new QSoundEffect;
  m_soundButtonClick->setSource(QUrl::fromLocalFile("data/audio/menu_click.wav"));

  SetVolume();
}

void MainWindow::SetVolume()
{
   m_soundButtonClick->setVolume(SOUND_MENU_VOLUME);
}

void MainWindow::MoveWindowToCenter()
{
  QRect frect = frameGeometry();
  frect.moveCenter(QDesktopWidget().availableGeometry().center());
  move(frect.topLeft());
}

// Предлагает сохранить игру -> вызывает SaveGame в случае подтверждения пользователем
void MainWindow::ShowDialog(QString const & msg, DialogTypes type)
{
  switch (type)
  {
    case DialogTypes::OnSubmitSettingsLeave:                                       // CHANGE SETTINGS BEFORE LEAVE FROM SETTINGS WIDGET
    {
      this->setDisabled(true);
      QDialog * about = new QDialog;
      about->setStyleSheet(m_style);
      about->resize(250,100);
      about->setWindowFlags(Qt::ToolTip);

      QGridLayout * glLayout = new QGridLayout(about);

      QLabel * lMsg = new QLabel();
      lMsg->setText(msg + DIALOG_TEXT_SETTINGS_SAVE);
      QPushButton * pbDialogYes = new QPushButton();
      pbDialogYes->setText(DIALOG_BUTTON_YES);
      pbDialogYes->resize(150,33);
      connect(pbDialogYes, &QAbstractButton::clicked, [this, about]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          this->setDisabled(false);
          this->m_settingsChanged = false;
          this->m_widgetStacked->setCurrentIndex(0);
          about->close();
          this->SaveSettings();
        });
      QPushButton * pbDialogNo = new QPushButton();
      pbDialogNo->setText(DIALOG_BUTTON_NO);
      pbDialogNo->resize(150,33);
      connect(pbDialogNo, &QAbstractButton::clicked, [this, about]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          this->setDisabled(false);
          this->m_settingsChanged = false;
          this->m_widgetStacked->setCurrentIndex(0);
          about->close();
        });
      QPushButton * pbDialogAbort = new QPushButton();
      pbDialogAbort->setText(DIALOG_BUTTON_ABORT);
      pbDialogAbort->resize(150,33);
      connect(pbDialogAbort, &QAbstractButton::clicked, [this, about]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          this->setDisabled(false);
          about->close();
        });

      glLayout->addWidget(lMsg, 0, 0, 1, 3);
      glLayout->addWidget(pbDialogYes, 1, 0, 1, 1);
      glLayout->addWidget(pbDialogNo, 1, 1, 1, 1);
      glLayout->addWidget(pbDialogAbort, 1, 2, 1, 1);

      about->show();
      break;
    }
    case DialogTypes::OnSubmitClose:                                       // Submit close
    {
      this->setDisabled(true);
      QDialog * about = new QDialog;
      about->setStyleSheet(m_style);
      about->resize(250,100);
      about->setWindowFlags(Qt::ToolTip);

      QGridLayout * glLayout = new QGridLayout(about);

      QLabel * lMsg = new QLabel();
      lMsg->setText(msg + DIALOG_TEXT_CLOSE);
      QPushButton * pbDialogYes = new QPushButton();
      pbDialogYes->setText(DIALOG_BUTTON_YES);
      pbDialogYes->resize(150,50);
      connect(pbDialogYes, &QAbstractButton::clicked, [this, about, msg]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          this->setDisabled(false);
          GAME_STARTED = false;
          ShowMenuItems();
          // delete m_space and set it to nullptr
          m_windowGame->DeleteSpace();
          about->close();
          if (msg == DIALOG_ON_SUBMIT_CLOSE) this->close();   // if exit
        });
      QPushButton * pbDialogNo = new QPushButton();
      pbDialogNo->setText(DIALOG_BUTTON_NO);
      pbDialogNo->resize(150,50);
      connect(pbDialogNo, &QAbstractButton::clicked, [this, about, msg]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          this->setDisabled(false);
          about->close();
        });

      glLayout->addWidget(lMsg, 0, 0, 1, 3);
      glLayout->addWidget(pbDialogYes, 1, 0, 1, 1);
      glLayout->addWidget(pbDialogNo, 1, 1, 1, 1);

      about->show();
      break;
    }
    case DialogTypes::OnSettingsLoaded:                                         // SUCCESFULL LOADED SETTINGS
    {
      this->setDisabled(true);
      QDialog * about = new QDialog;
      about->setStyleSheet(m_style);
      about->resize(250,100);
      about->setWindowFlags(Qt::ToolTip);

      QPushButton * pbDialog = new QPushButton(about);
      pbDialog->setText(msg + ADDITION_DIALOG_TEXT);
      pbDialog->resize(250,100);
      connect(pbDialog, &QAbstractButton::clicked, [this, about]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          this->setDisabled(false);
          about->close();
        });

      about->show();
      break;
    }
    case DialogTypes::OnSettingsLoadError:                                      // FAILED LOADING SETTINGS
    {
      this->setDisabled(true);
      QDialog * about = new QDialog;
      about->setStyleSheet(m_style);
      about->resize(250,100);
      about->setWindowFlags(Qt::ToolTip);

      QPushButton * pbDialog = new QPushButton(about);
      pbDialog->setText(msg + ADDITION_DIALOG_TEXT);
      pbDialog->resize(250,100);
      connect(pbDialog, &QAbstractButton::clicked, [this, about]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          this->setDisabled(false);
          about->close();
        });

      about->show();
      break;
    }
    case DialogTypes::OnSaveRecord:                                      // Save Record dialog
    {
      this->setDisabled(true);
      QDialog * about = new QDialog;
      about->setStyleSheet(m_style);
      about->resize(250,100);
      about->setWindowFlags(Qt::ToolTip);

      QGridLayout * glLayout = new QGridLayout(about);

      QLabel * lMsg = new QLabel();
      lMsg->setText(msg);

      QLabel * nameLabel = new QLabel();
      nameLabel->setText(DIALOG_ON_SAVE_RECORD_INPUT_NAME);
      nameLabel->resize(25,25);
      QLineEdit * nameLine = new QLineEdit;
      nameLine->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

      QPushButton * pbDialogSave = new QPushButton();
      pbDialogSave->setText(DIALOG_BUTTON_SAVE);
      pbDialogSave->resize(100,50);
      connect(pbDialogSave, &QAbstractButton::clicked, [this, about, nameLine, lMsg, msg]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          if (nameLine->text() != "")
          {
            this->setDisabled(false);
            this->SaveRecord(nameLine->text());
            about->close();
          }
          else
          {
            lMsg->setText(msg + DIALOG_ON_SAVE_RECORD_ERROR);
          }
        });

      QPushButton * pbDialogAbort = new QPushButton();
      pbDialogAbort->setText(DIALOG_BUTTON_ABORT);
      pbDialogAbort->resize(100,50);
      connect(pbDialogAbort, &QAbstractButton::clicked, [this, about]()
        {
          if (SOUND_MENU_ON) m_soundButtonClick->play();
          this->setDisabled(false);
          about->close();
        });

      glLayout->addWidget(lMsg, 0, 0, 1, 3);
      glLayout->addWidget(nameLabel, 1, 0, 1, 1);
      glLayout->addWidget(nameLine, 1, 1, 1, 1);
      glLayout->addWidget(pbDialogSave, 2, 0, 1, 1);
      glLayout->addWidget(pbDialogAbort, 2, 1, 1, 1);

      about->show();
      break;
    }
  }
}

void MainWindow::Resize(size_t w, size_t h)
{
  m_size.setWidth(w);
  m_size.setHeight(h);

  this->setMaximumSize(m_size-QSize(1,1));
  this->setMinimumSize(m_size);
  this->resize(m_size);
  m_widgetStacked->resize(m_size);
  m_widgetMenu->resize(m_size);
  m_widgetSettings->resize(m_size);
  m_windowGame->resize(m_size);
  MoveWindowToCenter();

  ResizeQGridLayouts();
  ChangeConstants(w, h);
}

void MainWindow::SetTextsForCurLang()
{
  // WINDOW
  this->setWindowTitle(QMainWindow::tr("Invaders Reincarnation"));
  // MENU
    // button
  if (GAME_STARTED)
  {
    m_pbMenuNewGame->setText(QPushButton::tr("End game"));
    m_pbMenuNewGame->setToolTip(QPushButton::tr("End current game with save dialog"));
  }
  else
  {
    m_pbMenuNewGame->setText(QPushButton::tr("New game"));
    m_pbMenuNewGame->setToolTip(QPushButton::tr("Start new game"));
  }
  m_pbMenuContinueGame->setText(QPushButton::tr("Continue game"));
  m_pbMenuContinueGame->setToolTip(QPushButton::tr("Continue current game"));
  m_pbToSet->setText(QPushButton::tr("Settings"));
  m_pbToSet->setToolTip(QPushButton::tr("Move to the settings"));
  m_pbMenuRecord->setText(QPushButton::tr("Game scores"));
  m_pbMenuRecord->setToolTip(QPushButton::tr("Move to the game scores table"));
  m_pbExit->setText(QPushButton::tr("Exit"));
  m_pbExit->setToolTip(QPushButton::tr("Close program"));
  // SETTINGS
    // button
  m_pbToMenu->setText(QPushButton::tr("Menu"));
  m_pbToMenu->setToolTip(QPushButton::tr("Back to the main menu"));
  m_pbSaveSettings->setText(QPushButton::tr("Save settings"));
  m_pbSaveSettings->setToolTip(QPushButton::tr("Save settings to data/settings"));
  m_pbLoadSettings->setText(QPushButton::tr("Load settings"));
  m_pbLoadSettings->setToolTip(QPushButton::tr("Load settings from data/settings"));
  m_pbSetDefault->setText(QPushButton::tr("Set default"));
  m_pbSetDefault->setToolTip(QPushButton::tr("Set default settings"));
    // label
  m_lControlComment->setText(QLabel::tr("__________________Control buttons:_________________"));
  m_lControlGunMoveLeft->setText(QLabel::tr("Gun move left button---"));
  m_lControlGunMoveRight->setText(QLabel::tr("Gun move right button-"));
  m_lControlGunShoot->setText(QLabel::tr("Button to gun shoot-----"));
  m_lControlGamePause->setText(QLabel::tr("Game pause---------------"));
  m_lGameParamComment->setText(QLabel::tr("_______________________Game:_____________________"));
  m_lGPAliensCount->setText(QLabel::tr("Count of aliens = ") + QString::number(m_slGPAliensCount->value()));
  m_lGPObstacleCount->setText(QLabel::tr("Count of obstacles = ") + QString(" ") + QString::number(m_slGPObstacleCount->value()));
  m_lGPObstacleRedraw->setText(QLabel::tr("Redraw obstacles every level"));
  m_lGPGunStartLives->setText(QLabel::tr("Start gun lives count = ") + QString::number(m_slGPGunStartLives->value()));
  m_lGPGunAddLive->setText(QLabel::tr("Add one life to gun at every level"));
  m_lWindowComment->setText(QLabel::tr("______________________Main:_______________________"));
  m_lWindowSize->setText(QLabel::tr("Window size"));
  m_lWindowState->setText(QLabel::tr("Window state"));
  m_lLanguage->setText(QLabel::tr("Language:"));
  m_lSoundComment->setText(QLabel::tr("_____________________Sound:_______________________"));
  m_lSMainOn->setText(QLabel::tr("Menu page sound state on?"));
  m_lSMainVolume->setText(QLabel::tr("Menu page volume = ") + QString::number(m_slSMenuVolume->value()));
  m_lSGameOn->setText(QLabel::tr("Game page sound state on?"));
  m_lSGameVolume->setText(QLabel::tr("Game page volume = ") + QString::number(m_slSGameVolume->value()));
    // combo box
      // window state
  m_cbWindowState->setItemText(GameWindowStateTypes::FullScreen, QComboBox::tr("Full screen"));
  m_cbWindowState->setItemText(GameWindowStateTypes::MinimizedWindow, QComboBox::tr("Minimized window"));
  m_cbWindowState->setItemText(GameWindowStateTypes::MaximizedWindow, QComboBox::tr("Maximized window"));
      // language
  m_cbLanguage->setItemText(GameLanguages::English, QComboBox::tr("English"));
  m_cbLanguage->setItemText(GameLanguages::Russian, QComboBox::tr("Russian"));
  // RECORDS
    // button
  m_pbToMenuFromRecords->setText(m_pbToMenu->text());
  m_pbToMenuFromRecords->setToolTip(m_pbToMenu->toolTip());
    // DIALOGS
      // main
  DIALOG_ON_SUBMIT_CLOSE          = QObject::tr("Are you really want to close program?");
  DIALOG_ON_SUBMIT_BREAK          = QObject::tr("Are you really want to break current game?");
  DIALOG_ON_SUBMIT_SETTINGS_LEAVE = QObject::tr("Do you want to save current settings of the game before back to the main menu?");
  DIALOG_ON_SETTINGS_LOADED       = QObject::tr("Succesfull load settings.");
  DIALOG_ON_SETTINGS_LOAD_ERROR   = QObject::tr("Error then try to load settings.");
  ADDITION_DIALOG_TEXT            = QObject::tr("\n\n Click to back to settings.");
  DIALOG_TEXT_SETTINGS_SAVE       = QObject::tr("\n Click:\n Yes - to save settings and go to the main menu;\n No - to go to the main menu without saving;\n Abort - to abort action. ");
  DIALOG_TEXT_CLOSE               = QObject::tr("\n Click:\n Yes - to accept;\n No - to abort action. ");
  DIALOG_BUTTON_YES               = QObject::tr("Yes");
  DIALOG_BUTTON_NO                = QObject::tr("No");
  DIALOG_BUTTON_ABORT             = QObject::tr("Abort");
  DIALOG_BUTTON_SAVE              = QObject::tr("Save");
      // records
  DIALOG_ON_SAVE_RECORD           = QObject::tr("Input you name and click: \n 'Save' - to accept you record \n 'Abort' - to continue without saving you rate");
  DIALOG_ON_SAVE_RECORD_ERROR     = QObject::tr("\n Name are important to save record!");
  DIALOG_ON_SAVE_RECORD_INPUT_NAME= QObject::tr("Name:");
  RECORD_MESSAGE                  = QObject::tr("RECORDS:");
  RECORD_NO_MESSAGE               = QObject::tr("NO RECORDS! BE FIRST ;)");
      // game
  GAME_RESULT                     = QObject::tr("You rezult: ");
  if (GAME_RESULT == "You rezult: ") GAME_RESULT = QString::fromStdString("Score: ");
  GAME_OVER                       = QObject::tr("Game over. Gun shooted.");
  GAME_OVER_ADDITIONAL            = QObject::tr("Press Enter to go to the game scores!");
  GAME_CURRENT_RESULT             = QObject::tr("You current rezult: ");
  if (GAME_CURRENT_RESULT == "You current rezult: ") GAME_CURRENT_RESULT = QString::fromStdString("You current score: ");
  GAME_PRE_NEXT_LEVEL             = QObject::tr("Aliens destroyed! You win! Level ");
  GAME_NEXT_ADDITIONAL            = QObject::tr("Press Enter to go to the next level!");
  GAME_USE_CHEATS_MSG             = QObject::tr("You are used cheat code on this level");
  GAME_NEXT_ALIEN_MODIF           = QObject::tr("Next Level Aliens Modifications:");
  GAME_NEXT_ALIEN_MODIF_FAST_MOVE = QObject::tr("Little faster move");
  GAME_NEXT_ALIEN_MODIF_FAST_SHOOT= QObject::tr("Little faster shoot");
  GAME_NEXT_ALIEN_MODIF_SPEC_LIFE = QObject::tr("One more life to aliens");
    // function to rewrite records to current language
  CreateRecordTable();
}

void MainWindow::ResizeQGridLayouts()
{
    // menu layout
  m_layoutMenu->setMargin(20);
  m_layoutMenu->setColumnStretch(0, m_size.width()/100*30);
  m_layoutMenu->setColumnMinimumWidth(1, 225);
  m_layoutMenu->setColumnStretch(2, m_size.width()/100*30);
  m_layoutMenu->setRowStretch(0, m_size.height()/100*10);
  m_layoutMenu->setRowMinimumHeight(5, 36);
  m_layoutMenu->setRowStretch(7, m_size.height()/100*15);
    // settings layout
  m_layoutSettings->setMargin(20);
  m_layoutSettings->setColumnMinimumWidth(0, 140);
  m_layoutSettings->setColumnMinimumWidth(1, 140);
  m_layoutSettings->setColumnMinimumWidth(2, 140);
  m_layoutSettings->setColumnMinimumWidth(3, 140);
  m_layoutSettings->setColumnStretch(4, m_size.width()/100*30);
  m_layoutSettings->setRowMinimumHeight(1, 15);
  m_layoutSettings->setRowMinimumHeight(7, 15);
  m_layoutSettings->setRowMinimumHeight(12, 45);
  m_layoutSettings->setRowStretch(13, m_size.height()/100*15);
    // records layout
  m_recordsVLayout->setMargin(20);
  m_pbToMenuFromRecords->setFixedWidth(m_pbToMenu->width());
}

void MainWindow::SetSize(int state)
{
  size_t w = 800, h = 600;
  switch (state)
  {
    case Size1024x768: { w = 1024; h = 768; break; }
    case Size1280x720: { w = 1280; h = 720; break; }
    case Size1280x1024: { w = 1280; h = 1024; break; }
    case Size1360x768: { w = 1360; h = 768; break; }
    case Size1366x768: { w = 1366; h = 768; break; }
    case Size1400x1050: { w = 1400; h = 1050; break; }
    case Size1440x900: { w = 1440; h = 900; break; }
    case Size1600x900: { w = 1600; h = 900; break; }
    case Size1680x1050: { w = 1680; h = 1050; break; }
    case Size1920x1080: { w = 1920; h = 1080; break; }
  }
  m_size.setWidth(w);
  m_size.setHeight(h);
}


// SHORTCUTS
// shortcuts  slots
void MainWindow::ShortcutPause()
{
  if (m_widgetStacked->currentIndex() != 0)
  {
    CheckoutToMenu();
    ShowMenuItems();
    // delete m_space, set it to nullptr
    if (!GAME_STARTED) m_windowGame->DeleteSpace();
  }
  else
  {
    if (GAME_STARTED)
    {
      m_widgetStacked->setCurrentIndex(2);
    }
  }
}


// MENU
// check current game state and show needed buttons in main menu
void MainWindow::ShowMenuItems()
{
  if (GAME_STARTED)
  {
    // Menu
    m_pbMenuNewGame->setText(QPushButton::tr("End game"));
    m_pbMenuNewGame->setToolTip(QPushButton::tr("End current game with save dialog"));
    m_pbMenuContinueGame->show();

    // Settings block
      // button
    m_pbLoadSettings->hide();
    m_pbSetDefault->hide();
      // boxes
    m_cbWindowState->setDisabled(true);
    m_cbWindowState->setObjectName("NotActiveBox");
    m_cbWindowState->setStyleSheet(m_style);
    m_cbWindowSize->setDisabled(true);
    m_cbWindowSize->setObjectName("NotActiveBox");
    m_cbWindowSize->setStyleSheet(m_style);
  }
  else
  {
    // Menu
    m_pbMenuNewGame->setText(QPushButton::tr("New game"));
    m_pbMenuNewGame->setToolTip(QPushButton::tr("Start new game"));
    m_pbMenuContinueGame->hide();

    // Settings unlock
      // button
    m_pbLoadSettings->show();
    m_pbSetDefault->show();
      // boxes
    m_cbWindowState->setDisabled(false);
    m_cbWindowState->setObjectName("");
    m_cbWindowState->setStyleSheet(m_style);
    ChangeWindowState(m_cbWindowState->currentIndex());
    m_settingsChanged = false;
  }
}

// menu button slots
void MainWindow::NewGame()
{
  if (SOUND_MENU_ON) m_soundButtonClick->play();

  if (GAME_STARTED)  // END GAME
  {
    ShowDialog(DIALOG_ON_SUBMIT_BREAK , DialogTypes::OnSubmitClose);
  }
  else               // NEW GAME
  {
    // flag set change menu
    GAME_STARTED = true;
    ShowMenuItems();
    m_windowGame->NewGame();
    m_widgetStacked->setCurrentIndex(2);
  }
}

void MainWindow::ContinueGame()
{
  if (SOUND_MENU_ON) m_soundButtonClick->play();

  m_widgetStacked->setCurrentIndex(2);
}

void MainWindow::CheckoutToSettings()
{
  if (SOUND_MENU_ON) m_soundButtonClick->play();

  m_widgetStacked->setCurrentIndex(1);
}


// SETTINGS
// settings button slots
void MainWindow::CheckoutToMenu()
{
  if (SOUND_MENU_ON) m_soundButtonClick->play();

  ShowMenuItems();

  if (m_settingsChanged) ShowDialog(DIALOG_ON_SUBMIT_SETTINGS_LEAVE, DialogTypes::OnSubmitSettingsLeave);
  else m_widgetStacked->setCurrentIndex(0);
}

void MainWindow::CheckoutToRecords()
{
  m_widgetStacked->setCurrentIndex(3);
}

// save/load settings
void MainWindow::WriteJson()
{
  Json::Value settings;

  auto & root = settings["settings"];

  auto & control = root["controlButtons"];
  control["gunMoveLeft"]["key"] = m_kseGunMoveLeft->keySequence().toString().toStdString();
  control["gunMoveRight"]["key"] = m_kseGunMoveRight->keySequence().toString().toStdString();
  control["gunShoot"]["key"] = m_kseGunShoot->keySequence().toString().toStdString();
  control["gamePause"]["key"] = m_kseGamePause->keySequence().toString().toStdString();

  auto & game = root["game"];
  game["Aliens"]["count"] = m_slGPAliensCount->value();
  game["Obstacles"]["count"] = m_slGPObstacleCount->value();
  game["Obstacles"]["flagRedraw"] = m_chbGPObstacleRedraw->isChecked();
  game["Gun"]["lives"] = m_slGPGunStartLives->value();
  game["Gun"]["flagAddLife"] = m_chbGPGunAddLive->isChecked();

  auto & main = root["main"];
  main["windowSize"]["index"] = m_cbWindowSize->currentIndex();
  main["windowState"]["index"] = m_cbWindowState->currentIndex();
  main["language"]["index"] = m_cbLanguage->currentIndex();

  auto & sound = root["sound"];
  sound["MenuOn"]["flag"] = m_chbSMenuOn->isChecked();
  sound["MenuVolume"]["volume"] = m_slSMenuVolume->value();
  sound["GameOn"]["flag"] = m_chbSGameOn->isChecked();
  sound["GameVolume"]["volume"] = m_slSGameVolume->value();

  std::ofstream settingsFile;
  settingsFile.open("data/settings.json");
  if (settingsFile.is_open())
  {
    Json::StyledWriter styledWriter;
    settingsFile << styledWriter.write(settings);
    settingsFile.close();
  }
}

bool MainWindow::ReadJson()
{
  Json::Value settings;
  std::ifstream file("data/settings.json");
  if (file.is_open())
  {
    file >> settings;
    file.close();
  }
  else return false; // not loaded

  auto & root = settings["settings"];

  Json::Value & control = root["controlButtons"];
  if (!control.empty())
  {
    if (!control["gunMoveLeft"]["key"].empty())
    {
      m_kseGunMoveLeft->setKeySequence(QKeySequence(
        QString(control["gunMoveLeft"]["key"].asCString())
      ));
    }
    if (!control["gunMoveRight"]["key"].empty())
    {
      m_kseGunMoveRight->setKeySequence(QKeySequence(
        QString(control["gunMoveRight"]["key"].asCString())
      ));
    }
    if (!control["gunShoot"]["key"].empty())
    {
      m_kseGunShoot->setKeySequence(QKeySequence(
        QString(control["gunShoot"]["key"].asCString())
      ));
    }
    if (!control["gamePause"]["key"].empty())
    {
      m_kseGamePause->setKeySequence(QKeySequence(
        QString(control["gamePause"]["key"].asCString())
      ));
    }
  }

  auto & game = root["game"];
  if (!game.empty())
  {
    if (!game["Aliens"]["count"].empty())
    {
      m_slGPAliensCount->setValue(game["Aliens"]["count"].asInt());
    }
    if (!game["Obstacles"]["count"].empty())
    {
      m_slGPObstacleCount->setValue(game["Obstacles"]["count"].asInt());
    }
    if (!game["Obstacles"]["flagRedraw"].empty())
    {
      m_chbGPObstacleRedraw->setChecked(game["Obstacles"]["flagRedraw"].asBool());
    }
    if (!game["Gun"]["lives"].empty())
    {
      m_slGPGunStartLives->setValue(game["Gun"]["lives"].asInt());
    }
    if (!game["Gun"]["flagAddLife"].empty())
    {
      m_chbGPGunAddLive->setChecked(game["Gun"]["flagAddLife"].asBool());
    }

    ChangeObstacleRedrawState(m_chbGPObstacleRedraw->isChecked());
    ChangeGunAddLiveState(m_chbGPGunAddLive->isChecked());
  }

  auto & main = root["main"];
  if (!main.empty())
  {
    if (!main["windowSize"]["index"].empty())
    {
      m_cbWindowSize->setCurrentIndex(main["windowSize"]["index"].asInt());
    }
    if (!main["windowState"]["index"].empty())
    {
      m_cbWindowState->setCurrentIndex(main["windowState"]["index"].asInt());
    }
    if (!main["language"]["index"].empty())
    {
      m_cbLanguage->setCurrentIndex(main["language"]["index"].asInt());
    }

    SetSize(m_cbWindowSize->currentIndex());
    ResizeQGridLayouts();
    ChangeWindowState(m_cbWindowState->currentIndex());
    ChangeLanguage(m_cbLanguage->currentIndex());
  }

  auto & sound = root["sound"];
  if (!sound.empty())
  {
    if (!sound["MenuOn"]["flag"].empty())
    {
      m_chbSMenuOn->setChecked(sound["MenuOn"]["flag"].asBool());
    }
    if (!sound["MenuVolume"]["volume"].empty())
    {
      m_slSMenuVolume->setValue(sound["MenuVolume"]["volume"].asInt());
    }
    if (!sound["GameOn"]["flag"].empty())
    {
      m_chbSGameOn->setChecked(sound["GameOn"]["flag"].asBool());
    }
    if (!sound["GameVolume"]["volume"].empty())
    {
      m_slSGameVolume->setValue(sound["GameVolume"]["volume"].asInt());
    }

    ChangeSoundMenuOn(m_chbSMenuOn->isChecked());
    ChangeSoundGameOn(m_chbSGameOn->isChecked());
  }

  return true; //success load
}

void MainWindow::WriteXml()
{
  pugi::xml_document doc;
  auto declarationNode = doc.append_child(pugi::node_declaration);
  declarationNode.append_attribute("version") = "1.0";
  declarationNode.append_attribute("encoding") = "UTF-8";

  auto root = doc.append_child("settings");

  pugi::xml_node controlButtons = root.append_child("controlButtons");
  controlButtons.append_child("gunMoveLeft").append_attribute("key").set_value(m_kseGunMoveLeft->keySequence().toString().toStdString().c_str());
  controlButtons.append_child("gunMoveRight").append_attribute("key").set_value(m_kseGunMoveRight->keySequence().toString().toStdString().c_str());
  controlButtons.append_child("gunShoot").append_attribute("key").set_value(m_kseGunShoot->keySequence().toString().toStdString().c_str());
  controlButtons.append_child("gamePause").append_attribute("key").set_value(m_kseGamePause->keySequence().toString().toStdString().c_str());

  pugi::xml_node game = root.append_child("game");
  game.append_child("Aliens").append_attribute("count").set_value(m_slGPAliensCount->value());
  game.append_child("Obstacles").append_attribute("count").set_value(m_slGPObstacleCount->value());
  game.child("Obstacles").append_attribute("flagRedraw").set_value(m_chbGPObstacleRedraw->isChecked());
  game.append_child("Gun").append_attribute("lives").set_value(m_slGPGunStartLives->value());
  game.child("Gun").append_attribute("flagAddLife").set_value(m_chbGPGunAddLive->isChecked());

  pugi::xml_node main = root.append_child("main");
  main.append_child("windowSize").append_attribute("index").set_value(m_cbWindowSize->currentIndex());
  main.append_child("windowState").append_attribute("index").set_value(m_cbWindowState->currentIndex());
  main.append_child("language").append_attribute("index").set_value(m_cbLanguage->currentIndex());

  pugi::xml_node sound = root.append_child("sound");
  sound.append_child("MenuOn").append_attribute("flag").set_value(m_chbSMenuOn->isChecked());
  sound.append_child("MenuVolume").append_attribute("volume").set_value(m_slSMenuVolume->value());
  sound.append_child("GameOn").append_attribute("flag").set_value(m_chbSGameOn->isChecked());
  sound.append_child("GameVolume").append_attribute("volume").set_value(m_slSGameVolume->value());

  doc.save_file("data/settings.xml", PUGIXML_TEXT("  "));
}

bool MainWindow::ReadXml()
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("data/settings.xml", pugi::parse_default | pugi::parse_declaration);

  if (result)
  {
    pugi::xml_node root = doc.document_element();

    pugi::xml_node controlButtons = root.child("controlButtons");
    if (!controlButtons.empty())
    {
      if (!controlButtons.child("gunMoveLeft").attribute("key").empty())
      {
        m_kseGunMoveLeft->setKeySequence(QKeySequence(
          controlButtons.child("gunMoveLeft").attribute("key").value()
        ));
      }
      if (!controlButtons.child("gunMoveRight").attribute("key").empty())
      {
        m_kseGunMoveRight->setKeySequence(QKeySequence(
          controlButtons.child("gunMoveRight").attribute("key").value()
        ));
      }
      if (!controlButtons.child("gunShoot").attribute("key").empty())
      {
        m_kseGunShoot->setKeySequence(QKeySequence(
          controlButtons.child("gunShoot").attribute("key").value()
        ));
      }
      if (!controlButtons.child("gamePause").attribute("key").empty())
      {
        m_kseGamePause->setKeySequence(QKeySequence(
          controlButtons.child("gamePause").attribute("key").value()
        ));
      }
    }

    pugi::xml_node game = root.child("game");
    if (!game.empty())
    {
      if (!game.child("Aliens").attribute("count").empty())
      {
        m_slGPAliensCount->setValue(
          game.child("Aliens").attribute("count").as_int()
        );
      }
      if (!game.child("Obstacles").attribute("count").empty())
      {
        m_slGPObstacleCount->setValue(
          game.child("Obstacles").attribute("count").as_int()
        );
      }
      if (!game.child("Obstacles").attribute("flagRedraw").empty())
      {
        m_chbGPObstacleRedraw->setChecked(
          game.child("Obstacles").attribute("flagRedraw").as_bool()
        );
      }
      if (!game.child("Gun").attribute("lives").empty())
      {
        m_slGPGunStartLives->setValue(
          game.child("Gun").attribute("lives").as_int()
        );
      }
      if (!game.child("Gun").attribute("flagAddLife").empty())
      {
        m_chbGPGunAddLive->setChecked(
          game.child("Gun").attribute("flagAddLife").as_bool()
        );
      }

      ChangeObstacleRedrawState(m_chbGPObstacleRedraw->isChecked());
      ChangeGunAddLiveState(m_chbGPGunAddLive->isChecked());
    }

    pugi::xml_node main = root.child("main");
    if (!main.empty())
    {
      if (!main.child("windowSize").attribute("index").empty())
      {
        m_cbWindowSize->setCurrentIndex(
          main.child("windowSize").attribute("index").as_int()
        );
      }
      if (!main.child("windowState").attribute("index").empty())
      {
        m_cbWindowState->setCurrentIndex(
          main.child("windowState").attribute("index").as_int()
        );
      }
      if (!main.child("language").attribute("index").empty())
      {
        m_cbLanguage->setCurrentIndex(
          main.child("language").attribute("index").as_int()
        );
      }

      SetSize(m_cbWindowSize->currentIndex());
      ChangeWindowState(m_cbWindowState->currentIndex());
      ChangeLanguage(m_cbLanguage->currentIndex());
    }

    pugi::xml_node sound = root.child("sound");
    if (!sound.empty())
    {
      if (!sound.child("MenuOn").attribute("flag").empty())
      {
        m_chbSMenuOn->setChecked(
          sound.child("MenuOn").attribute("flag").as_bool()
        );
      }
      if (!sound.child("MenuVolume").attribute("volume").empty())
      {
        m_slSMenuVolume->setValue(
          sound.child("MenuVolume").attribute("volume").as_int()
        );
      }
      if (!sound.child("GameOn").attribute("flag").empty())
      {
        m_chbSGameOn->setChecked(
          sound.child("GameOn").attribute("flag").as_bool()
        );
      }
      if (!sound.child("GameVolume").attribute("volume").empty())
      {
        m_slSGameVolume->setValue(
          sound.child("GameVolume").attribute("volume").as_int()
        );
      }

      ChangeSoundMenuOn(m_chbSMenuOn->isChecked());
      ChangeSoundGameOn(m_chbSGameOn->isChecked());
    }
  }
  else return false; // not loaded
  return true; // success load
}

// set default settings
void MainWindow::SetDefaultSettings()
{
    // shortcut
  m_shortcutGamePause->setKey(Qt::Key_Escape);
    // settings QObjects
  m_kseGunMoveLeft->setKeySequence(Qt::Key_Left);
  m_kseGunMoveRight->setKeySequence(Qt::Key_Right);
  m_kseGunShoot->setKeySequence(Qt::Key_Up);
  m_kseGamePause->setKeySequence(m_shortcutGamePause->key());
  m_slGPAliensCount->setValue(55);
  m_slGPObstacleCount->setValue(4);
  m_slGPGunStartLives->setValue(3);
  m_chbGPObstacleRedraw->setChecked(false);
  m_chbGPGunAddLive->setChecked(false);
  m_cbWindowState->setCurrentIndex(GameWindowStateTypes::MinimizedWindow);
  m_cbWindowSize->setCurrentIndex(GameResolutionTypes::Size800x600);
  m_cbLanguage->setCurrentIndex(GameLanguages::English);
  m_slSMenuVolume->setValue(25);
  m_slSGameVolume->setValue(25);
  m_chbSMenuOn->setChecked(false);
  m_chbSGameOn->setChecked(false);

    // call change functions
      // game
  ChangeObstacleRedrawState(m_chbGPObstacleRedraw->isChecked());
  ChangeGunAddLiveState(m_chbGPGunAddLive->isChecked());
      // main
  SetSize(m_cbWindowSize->currentIndex());
  ChangeWindowState(m_cbWindowState->currentIndex());
  ChangeLanguage(m_cbLanguage->currentIndex());
      // sound
  ChangeSoundMenuOn(m_chbSMenuOn->isChecked());
  ChangeSoundGameOn(m_chbSGameOn->isChecked());
}

// load settings from file
void MainWindow::LoadSettings()
{
  if (SOUND_MENU_ON) m_soundButtonClick->play();

  if (ReadXml()) ShowDialog(DIALOG_ON_SETTINGS_LOADED, DialogTypes::OnSettingsLoaded);
  else if (ReadJson()) ShowDialog(DIALOG_ON_SETTINGS_LOADED, DialogTypes::OnSettingsLoaded);
  else ShowDialog(DIALOG_ON_SETTINGS_LOAD_ERROR, DialogTypes::OnSettingsLoadError);
  m_settingsChanged = false;
}

// save settings to file
void MainWindow::SaveSettings()
{
  if (SOUND_MENU_ON) m_soundButtonClick->play();

  WriteXml();
  WriteJson();
  m_settingsChanged = false;
}

// settings controls
void MainWindow::ChangeShortcutGunMoveLeft(QKeySequence key)
{
  m_windowGame->SetKey(KeyTypes::KeyMoveLeft, key);
}

void MainWindow::ChangeShortcutGunMoveRight(QKeySequence key)
{
  m_windowGame->SetKey(KeyTypes::KeyMoveRight, key);
}

void MainWindow::ChangeShortcutGunShoot(QKeySequence key)
{
  m_windowGame->SetKey(KeyTypes::KeyShoot, key);
}

void MainWindow::ChangeShortcutGamePause(QKeySequence key)
{
  m_shortcutGamePause->setKey(key);
  m_windowGame->SetKey(KeyTypes::KeyPause, key);
}

// settings game
void MainWindow::ChangeAliensCount(int state)
{
  if (state % 5 != 0)
  {
    state += 5 - state % 5;
    m_slGPAliensCount->setValue(state);
  }
  ALIEN_COUNT = state;
  m_lGPAliensCount->setText(QLabel::tr("Count of aliens = ") + QString::number(state));
  m_settingsChanged = true;
}

void MainWindow::ChangeObstacleCount(int state)
{
  OBSTACLE_COUNT = state;
  m_lGPObstacleCount->setText(QLabel::tr("Count of obstacles = ") + QString(" ") + QString::number(state));
  m_settingsChanged = true;
}

void MainWindow::ChangeGunStartLives(int state)
{
  GUN_LIVES_START = state;
  m_lGPGunStartLives->setText(QLabel::tr("Start gun lives count = ") + QString::number(state));
  m_settingsChanged = true;
}

void MainWindow::ChangeObstacleRedrawState(bool state)
{
  OBSTACLE_REDRAW_EVERY_LEVEL = state;
  m_settingsChanged = true;
}

void MainWindow::ChangeGunAddLiveState(bool state)
{
  GUN_LIVES_INC_EVERY_LEVEL = state;
  m_settingsChanged = true;
}

// settings main
void MainWindow::ChangeResolution(int state)
{
  SetSize(state);
  Resize(m_size.width(),m_size.height());
  m_settingsChanged = true;
}

void MainWindow::ChangeWindowState(int state)
{
  this->setMaximumSize(QSize(QDesktopWidget().availableGeometry().right() + 1, QDesktopWidget().availableGeometry().bottom() + 1));
  this->setMinimumSize(0, 0);

  if (state == GameWindowStateTypes::FullScreen)
  {
    this->showFullScreen();
    m_cbWindowSize->setDisabled(true);
    m_cbWindowSize->setObjectName("NotActiveBox");
    m_cbWindowSize->setStyleSheet(m_style);

    m_size.setWidth(QDesktopWidget().availableGeometry().right() + 1);
    m_size.setHeight(QDesktopWidget().availableGeometry().bottom() + 1);
  }
  else if (state == GameWindowStateTypes::MinimizedWindow)
  {
    setWindowState(Qt::WindowState::WindowActive);
    m_cbWindowSize->setDisabled(false);
    ChangeResolution(m_cbWindowSize->currentIndex());
    m_cbWindowSize->setObjectName("");
    m_cbWindowSize->setStyleSheet(m_style);

    m_size.setWidth(this->width());
    m_size.setHeight(this->height());
  }
  else
  {
    setWindowState(Qt::WindowState::WindowMaximized);
    m_cbWindowSize->setDisabled(true);
    m_cbWindowSize->setObjectName("NotActiveBox");
    m_cbWindowSize->setStyleSheet(m_style);

    m_size.setWidth(this->width());
    m_size.setHeight(this->height());
  }

  m_widgetStacked->resize(m_size);
  m_widgetMenu->resize(m_size);
  m_widgetSettings->resize(m_size);
  m_windowGame->resize(m_size);
  m_settingsChanged = true;
}

void MainWindow::ChangeLanguage(int state)
{
  switch (state)
  {
    case GameLanguages::English:
    {
      m_translator.load("data/translations/en");
      qApp->installTranslator(&m_translator);
      SetTextsForCurLang();
      break;
    }
    case GameLanguages::Russian:
    {
      m_translator.load("data/translations/ru");
      qApp->installTranslator(&m_translator);
      SetTextsForCurLang();
      break;
    }
  }
  m_settingsChanged = true;
}

// settings sound
void MainWindow::ChangeSoundMenuVolume(int state)
{
  SOUND_MENU_VOLUME = ((float)state) / 100;
  m_lSMainVolume->setText(QLabel::tr("Menu page volume = ") + QString::number(state));
  SetVolume();
  m_settingsChanged = true;
}

void MainWindow::ChangeSoundGameVolume(int state)
{
  SOUND_GAME_VOLUME = ((float)state) / 100;
  m_lSGameVolume->setText(QLabel::tr("Game page volume = ") + QString::number(state));
  m_windowGame->SetVolume();
  m_settingsChanged = true;
}

void MainWindow::ChangeSoundMenuOn(bool state)
{
  SOUND_MENU_ON = state;
  m_settingsChanged = true;
}

void MainWindow::ChangeSoundGameOn(bool state)
{
  SOUND_GAME_ON = state;
  m_settingsChanged = true;
}


// RECORDS
void MainWindow::InterfaceAddRecord()
{
  ShowDialog(DIALOG_ON_SAVE_RECORD, DialogTypes::OnSaveRecord);
}

void MainWindow::CreateRecordTable()
{
  if(recordsArray.size() == 0)
  {
    m_table->setText("<center><h1>" + RECORD_NO_MESSAGE + "</h1></center>");
    return;
  }

  QString text = "<code><center><h1>" + RECORD_MESSAGE + "<br><br>";
  for(int i = 0; i < recordsArray.size() && i < MAX_COUNT_RECORDS; i++)
  {
    text += recordsArray[i][0];
    for(int j = recordsArray[i][0].size() + recordsArray[i][1].size(); j < 50; j++)
    {
      text += "-";
    }
    text += recordsArray[i][1];
    text += "<br>";
  }
  text += "</h1></center></code>";
  m_table->setText(text);
}

// sort
struct
{
  bool operator()(std::vector<QString> a, std::vector<QString> b)
  {
    return a[1].toInt() > b[1].toInt();
  }
} customLess;

void MainWindow::SaveRecord(QString const & name)
{
  recordsArray.push_back
  (
    std::vector<QString>
    {
      name,
      QString::number(m_windowGame->GetSpace()->GetGun().GetRate())
    }
  );
  std::sort(recordsArray.begin(), recordsArray.end(), customLess);
  CreateRecordTable();
  WriteJsonRecord(recordsArray);
}

// save/load records
void MainWindow::WriteJsonRecord(RecordsVect & rezults)
{
  Json::Value records;

  auto & root = records["records"];

  root["countRecords"] = rezults.size();

  for (int i = 0; i < rezults.size(); i++)
  {
    root[QString::number(i).toStdString()]["name"] = rezults[i][0].toStdString();
    root[QString::number(i).toStdString()]["rezult"] = rezults[i][1].toStdString();
  }

  std::ofstream recordsFile;
  recordsFile.open("data/records.json");
  if (recordsFile.is_open())
  {
    Json::StyledWriter styledWriter;
    recordsFile << styledWriter.write(records);
    recordsFile.close();
  }
}

bool MainWindow::ReadJsonRecords(RecordsVect & rezults)
{
  rezults.clear();
  Json::Value records;
  std::ifstream file("data/records.json");
  if (file.is_open())
  {
    file >> records;
    file.close();
  }
  else return false; // not loaded

  Json::Value & root = records["records"];

  int countRecords = 0;
  if (!root.empty())
  {
    if (!root["countRecords"].empty())
    {
      countRecords = root["countRecords"].asInt();
    }
  }

  for (int i = 0; i < countRecords; i++)
  {
    if(!root[QString::number(i).toStdString()]["name"].empty() && !root[QString::number(i).toStdString()]["rezult"].empty())
    {
      rezults.push_back
      (
        std::vector<QString>
        {
          QString(root[QString::number(i).toStdString()]["name"].asCString()),
          QString(root[QString::number(i).toStdString()]["rezult"].asCString())
        }
      );
    }
  }
  return true;
}
