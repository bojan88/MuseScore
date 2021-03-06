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

#ifndef __OTTAVA_H__
#define __OTTAVA_H__

#include "textline.h"

//---------------------------------------------------------
//   OttavaE
//---------------------------------------------------------

struct OttavaE {
      int offset;
      unsigned start;
      unsigned end;
      };

class Ottava;

//---------------------------------------------------------
//   @@ OttavaSegment
//---------------------------------------------------------

class OttavaSegment : public TextLineSegment {
      Q_OBJECT

   protected:

   public:
      OttavaSegment(Score* s) : TextLineSegment(s) {}
      virtual ElementType type() const     { return OTTAVA_SEGMENT; }
      virtual OttavaSegment* clone() const { return new OttavaSegment(*this); }
      Ottava* ottava() const               { return (Ottava*)spanner(); }
      virtual void layout();
      };

//---------------------------------------------------------
//   @@ Ottava
//   @P ottavaType   enum OttavaType OTTAVA_8VA, OTTAVA_15MA, OTTAVA_8VB, OTTAVA_15MB
//---------------------------------------------------------

class Ottava : public TextLine {
      Q_OBJECT
      Q_ENUMS(OttavaType)

   public:
      enum OttavaType {
            OTTAVA_8VA,
            OTTAVA_15MA,
            OTTAVA_8VB,
            OTTAVA_15MB
            };

   private:
      Q_PROPERTY(OttavaType ottavaType READ ottavaType WRITE undoSetOttavaType)
      OttavaType _ottavaType;

   protected:
      QString text;
      int _pitchShift;
      mutable qreal textHeight;     ///< cached value

      friend class OttavaSegment;

   public:
      Ottava(Score* s);
      virtual Ottava* clone() const    { return new Ottava(*this); }
      virtual ElementType type() const { return OTTAVA; }

      void setOttavaType(OttavaType val);
      OttavaType ottavaType() const { return _ottavaType; }
      void undoSetOttavaType(OttavaType val);

      virtual LineSegment* createLineSegment();
      virtual void layout();
      int pitchShift() const { return _pitchShift; }
      virtual void endEdit();
      virtual void write(Xml& xml) const;
      virtual void read(XmlReader& de);

      virtual QVariant getProperty(P_ID propertyId) const;
      virtual bool setProperty(P_ID propertyId, const QVariant&);
      virtual QVariant propertyDefault(P_ID) const;
      virtual void setYoff(qreal);
      };

Q_DECLARE_METATYPE(Ottava::OttavaType)
#endif

