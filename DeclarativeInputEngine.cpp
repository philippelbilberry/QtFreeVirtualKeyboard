//============================================================================
/// \file   DeclarativeInputEngine.cpp
/// \author Uwe Kindler
/// \date   08.01.2015
/// \brief  Implementation of CDeclarativeInputEngine
///
/// Copyright 2015 Uwe Kindler
/// Licensed under MIT see LICENSE.MIT in project root
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DeclarativeInputEngine.h"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QJSEngine>
#include <QQmlEngine>
#include <QTimer>
#include <QtQml>

/**
 * Private data class
 */
struct DeclarativeInputEnginePrivate
{
    DeclarativeInputEngine *_this = nullptr;
    QTimer *animatingFinishedTimer = nullptr; //< triggers position adjustment of focused QML item is covered by keybaord rectangle
    DeclarativeInputEngine::InputMode inputMode = DeclarativeInputEngine::Latin;
    bool animating = false;
    QRect keyboardRectangle;

    /**
     * Private data constructor
     */
    explicit DeclarativeInputEnginePrivate(DeclarativeInputEngine *_public);
}; // struct DeclarativeInputEnginePrivate

//==============================================================================
DeclarativeInputEnginePrivate::DeclarativeInputEnginePrivate(DeclarativeInputEngine *_public)
    : _this(_public)
{}

//==============================================================================
DeclarativeInputEngine::DeclarativeInputEngine(QObject *parent)
    : QObject(parent)
    , d(new DeclarativeInputEnginePrivate(this))
{
    d->animatingFinishedTimer = new QTimer(this);
    d->animatingFinishedTimer->setSingleShot(true);
    d->animatingFinishedTimer->setInterval(100);
    connect(d->animatingFinishedTimer, &QTimer::timeout, this, &DeclarativeInputEngine::animatingFinished);
}

//==============================================================================
DeclarativeInputEngine::~DeclarativeInputEngine()
{
    delete d;
}

//==============================================================================
void DeclarativeInputEngine::virtualKeyCancel() {}

//==============================================================================
bool DeclarativeInputEngine::virtualKeyClick(Qt::Key key, const QString &text, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(key)
    Q_UNUSED(modifiers)

    QInputMethodEvent ev;
    if (text == QString("\x7F")) {
        //delete one char
        ev.setCommitString("", -1, 1);

    } else {
        //add some text
        ev.setCommitString(text);
    }
    QCoreApplication::sendEvent(QGuiApplication::focusObject(), &ev);
    return true;
}

//==============================================================================
void DeclarativeInputEngine::sendKeyToFocusItem(const QString &text)
{
    QInputMethodEvent ev;
    if (text == QString("\x7F")) {
        //delete one char
        ev.setCommitString("", -1, 1);

    } else {
        //add some text
        ev.setCommitString(text);
    }
    QCoreApplication::sendEvent(QGuiApplication::focusObject(), &ev);
}

//==============================================================================
bool DeclarativeInputEngine::virtualKeyPress(Qt::Key key, const QString &text, Qt::KeyboardModifiers modifiers, bool repeat)
{
    Q_UNUSED(key)
    Q_UNUSED(text)
    Q_UNUSED(modifiers)
    Q_UNUSED(repeat)

    // not implemented yet
    return true;
}

//==============================================================================
bool DeclarativeInputEngine::virtualKeyRelease(Qt::Key key, const QString &text, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(key)
    Q_UNUSED(text)
    Q_UNUSED(modifiers)

    // not implemented yet
    return true;
}

//==============================================================================
QRect DeclarativeInputEngine::keyboardRectangle() const
{
    return d->keyboardRectangle;
}

//==============================================================================
void DeclarativeInputEngine::setKeyboardRectangle(const QRect &Rect)
{
    setAnimating(true);
    d->animatingFinishedTimer->start(100);
    d->keyboardRectangle = Rect;
    emit keyboardRectangleChanged();
}

//==============================================================================
bool DeclarativeInputEngine::isAnimating() const
{
    return d->animating;
}

//==============================================================================
void DeclarativeInputEngine::setAnimating(bool Animating)
{
    if (d->animating != Animating) {
        d->animating = Animating;
        emit animatingChanged();
    }
}

//==============================================================================
void DeclarativeInputEngine::animatingFinished()
{
    setAnimating(false);
}

//==============================================================================
DeclarativeInputEngine::InputMode DeclarativeInputEngine::inputMode() const
{
    return d->inputMode;
}

//==============================================================================
void DeclarativeInputEngine::setInputMode(InputMode mode)
{
    //qDebug() << "CDeclarativeInputEngine::setInputMode " << Mode;
    if (mode != d->inputMode) {
        d->inputMode = mode;
        emit inputModeChanged(mode);
    }
}

//------------------------------------------------------------------------------
// EOF DeclarativeInputEngine.cpp
