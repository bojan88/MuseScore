//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef __LYRICS_H__
#define __LYRICS_H__

#include "text.h"
#include "chord.h"

class Segment;
class Chord;
class QPainter;

//---------------------------------------------------------
//   @@ Lyrics
//   @P syllabic  enum SINGLE, BEGIN, END, MIDDLE
//---------------------------------------------------------

class Lyrics : public Text {
      Q_OBJECT
      Q_PROPERTY(Syllabic syllabic READ syllabic WRITE setSyllabic)
      Q_ENUMS(Syllabic)

   public:
      enum Syllabic { SINGLE, BEGIN, END, MIDDLE };

   private:
      int _ticks;             ///< if > 0 then draw an underline to tick() + _ticks
                              ///< (melisma)
      Syllabic _syllabic;
      QList<Line*> _separator;
      Text* _verseNumber;

   protected:
      int _no;                ///< row index

   public:
      Lyrics(Score* = 0);
      Lyrics(const Lyrics&);
      ~Lyrics();
      virtual Lyrics* clone() const    { return new Lyrics(*this); }
      virtual ElementType type() const { return LYRICS; }
      virtual void scanElements(void* data, void (*func)(void*, Element*), bool all=true);
      virtual bool acceptDrop(MuseScoreView*, const QPointF&, Element*) const;
      virtual Element* drop(const DropData&);

      Segment* segment() const { return (Segment*)parent()->parent(); }
      Measure* measure() const { return (Measure*)parent()->parent()->parent(); }
      ChordRest* chordRest() const { return (ChordRest*)parent(); }

      virtual void layout();

      virtual void write(Xml& xml) const;
      virtual void read(XmlReader&);
      void setNo(int n);
      int no() const                { return _no; }
      void setSyllabic(Syllabic s)  { _syllabic = s; }
      Syllabic syllabic() const     { return _syllabic; }
      virtual void add(Element*);
      virtual void remove(Element*);
      virtual void draw(QPainter*) const;
      virtual void endEdit();

      int ticks() const                { return _ticks;    }
      void setTicks(int tick)          { _ticks = tick;    }
      int endTick() const;
      bool isMelisma() const           { return _ticks > 0; }

      void clearSeparator()            { _separator.clear(); } // TODO: memory leak
      QList<Line*>* separatorList()    { return &_separator; }
      virtual void paste();
      Text* verseNumber() const        { return _verseNumber; }
      void setVerseNumber(Text* t)     { _verseNumber = t;    }

      QVariant getProperty(P_ID propertyId) const;
      bool setProperty(P_ID propertyId, const QVariant&);
      QVariant propertyDefault(P_ID id) const;
      };

#endif

