//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "config.h"
#include "style.h"
#include "mscore.h"
#include "sequencer.h"
#include "accidental.h"
#include "figuredbass.h"
#include "stafftype.h"
#include "note.h"
#include "spanner.h"
#include "volta.h"
#include "ottava.h"
#include "trill.h"
#include "repeat.h"
#include "jump.h"
#include "marker.h"

qreal MScore::PDPI = 1200;
qreal MScore::DPI  = 1200;
qreal MScore::DPMM;
bool  MScore::debugMode;
bool  MScore::testMode = false;

MStyle* MScore::_defaultStyle;
MStyle* MScore::_baseStyle;
QString MScore::_globalShare;
int     MScore::_vRaster;
int     MScore::_hRaster;
QColor  MScore::selectColor[VOICES];
QColor  MScore::defaultColor;
QColor  MScore::layoutBreakColor;
QColor  MScore::bgColor;
QColor  MScore::dropColor;
bool    MScore::warnPitchRange;

bool    MScore::replaceFractions;
bool    MScore::playRepeats;
bool    MScore::panPlayback;
qreal   MScore::nudgeStep;
int     MScore::defaultPlayDuration;
QString MScore::partStyle;
QString MScore::lastError;
bool    MScore::layoutDebug = false;
int     MScore::division    = 480;
int     MScore::sampleRate  = 44100;
int     MScore::mtcType;

Sequencer* MScore::seq = 0;

extern void initSymbols(int);
extern void initStaffTypes();
extern void initDrumset();

//---------------------------------------------------------
//   init
//---------------------------------------------------------

void MScore::init()
      {
      Sym::init();

#ifdef SCRIPT_INTERFACE
      qRegisterMetaType<Element::ElementType>("ElementType");
      qRegisterMetaType<MScore::ValueType>("ValueType");
      qRegisterMetaType<MScore::Direction>("Direction");
      qRegisterMetaType<MScore::DirectionH>("DirectionH");
      qRegisterMetaType<Element::Placement>("Placement");
      qRegisterMetaType<Accidental::AccidentalRole>("AccidentalRole");
      qRegisterMetaType<Accidental::AccidentalType>("AccidentalType");
      qRegisterMetaType<Spanner::Anchor>("Anchor");
      qRegisterMetaType<Note::NoteHeadGroup>("NoteHeadGroup");
      qRegisterMetaType<Note::NoteHeadType>("NoteHeadType");
      qRegisterMetaType<Segment::SegmentType>("SegmentType");
      qRegisterMetaType<FiguredBassItem::Modifier>("Modifier");
      qRegisterMetaType<FiguredBassItem::Parenthesis>("Parenthesis");
      qRegisterMetaType<VoltaType>("VoltaType");
      qRegisterMetaType<Ottava::OttavaType>("OttavaType");
      qRegisterMetaType<Trill::TrillType>("TrillType");
      qRegisterMetaType<Element::DynamicRange>("DynamicRange");
      qRegisterMetaType<JumpType>("JumpType");
      qRegisterMetaType<MarkerType>("MarkerType");
      qRegisterMetaType<BeamMode>("BeamMode");
#endif

      DPMM = DPI / INCH;       // dots/mm

#ifdef __MINGW32__
      QDir dir(QCoreApplication::applicationDirPath() + QString("/../" INSTALL_NAME));
      _globalShare = dir.absolutePath() + "/";
#elif defined(Q_WS_IOS)
      {
      extern QString resourcePath();
      _globalShare = resourcePath();
      }
#elif defined(Q_WS_MAC)
      QDir dir(QCoreApplication::applicationDirPath() + QString("/../Resources"));
      _globalShare = dir.absolutePath() + "/";
#else
      _globalShare = QString( INSTPREFIX "/share/" INSTALL_NAME);
#endif

      selectColor[0].setRgb(0, 0, 255);     //blue
      selectColor[1].setRgb(0, 150, 0);     //green
      selectColor[2].setRgb(230, 180, 50);  //yellow
      selectColor[3].setRgb(200, 0, 200);   //purple
      defaultColor        = Qt::black;
      dropColor           = Qt::red;
      nudgeStep           = .1;       // in spatium units (default 0.1)
      defaultPlayDuration = 300;      // ms
      warnPitchRange      = true;
      replaceFractions    = true;
      playRepeats         = true;
      panPlayback         = true;
      partStyle           = "";

      lastError           = "";

      layoutBreakColor    = Qt::green;
      bgColor.setRgb(0x76, 0x76, 0x6e);

      _defaultStyle         = new MStyle();
      ::initStyle(_defaultStyle);
      _baseStyle            = new MStyle(*_defaultStyle);

      //
      //  load internal fonts
      //
      //
      // do not load application specific fonts
      // for MAC, they are in Resources/fonts
      //
#if !defined(Q_WS_MAC) && !defined(Q_WS_UIKIT)
      static const char* fonts[] = {
            "mscore-20.otf",
            "MuseJazz.ttf",
            "FreeSans.ttf",
            "FreeSerifMscore.ttf",
            "FreeSerifBold.ttf",
            "gonville-20.otf",
            "mscoreTab.ttf",
            "mscore-BC.ttf"
            };

      for (unsigned i = 0; i < sizeof(fonts)/sizeof(*fonts); ++i) {
            QString s = QString(":/fonts/%1").arg(fonts[i]);
            if (-1 == QFontDatabase::addApplicationFont(s)) {
                  qDebug("Mscore: fatal error: cannot load internal font <%s>\n", fonts[i]);
                  if (!MScore::debugMode)
                        exit(-1);
                  }
            }
#endif
      StaffTypeTablature::readConfigFile(0);          // get TAB font config, before initStaffTypes()
      initSymbols(0);   // init emmentaler symbols
      initStaffTypes();
      initDrumset();
      FiguredBass::readConfigFile(0);
      }

//---------------------------------------------------------
//   defaultStyle
//---------------------------------------------------------

MStyle* MScore::defaultStyle()
      {
      return _defaultStyle;
      }

//---------------------------------------------------------
//   baseStyle
//---------------------------------------------------------

MStyle* MScore::baseStyle()
      {
      return _baseStyle;
      }

//---------------------------------------------------------
//   setDefaultStyle
//---------------------------------------------------------

void MScore::setDefaultStyle(MStyle* s)
      {
      delete _defaultStyle;
      _defaultStyle = s;
      }

