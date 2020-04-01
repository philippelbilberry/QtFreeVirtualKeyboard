//============================================================================
/// \file   VirtualKeyboardInputContext.cpp
/// \author Uwe Kindler
/// \date   08.01.2015
/// \brief  Implementation of VirtualKeyboardInputContext
///
/// Copyright 2015 Uwe Kindler
/// Licensed under MIT see LICENSE.MIT in project root
//============================================================================

#include "VirtualKeyboardInputContext.h"
#include "DeclarativeInputEngine.h"

#include <QEvent>
#include <QGuiApplication>
#include <QJSEngine>
#include <QPropertyAnimation>
#include <QQmlContext>
#include <QQmlEngine>
#include <QVariant>
#include <private/qquickflickable_p.h>

/**
 * Private data class for VirtualKeyboardInputContext
 */
class VirtualKeyboardInputContextPrivate
{
public:
    VirtualKeyboardInputContextPrivate();
    VirtualKeyboardInputContextPrivate(const VirtualKeyboardInputContextPrivate &) = delete;
    QQuickFlickable *flickable = nullptr;
    QQuickItem *focusItem = nullptr;
    bool visible = false;
    DeclarativeInputEngine *inputEngine = nullptr;
    QPropertyAnimation *flickableContentScrollAnimation = nullptr; //< for smooth scrolling of flickable content item
};

VirtualKeyboardInputContextPrivate::VirtualKeyboardInputContextPrivate()
    : inputEngine(new DeclarativeInputEngine)
{}

VirtualKeyboardInputContext::VirtualKeyboardInputContext()
    : QPlatformInputContext()
    , d(new VirtualKeyboardInputContextPrivate)
{
    d->flickableContentScrollAnimation = new QPropertyAnimation(this);
    d->flickableContentScrollAnimation->setPropertyName("contentY");
    d->flickableContentScrollAnimation->setDuration(400);
    d->flickableContentScrollAnimation->setEasingCurve(QEasingCurve(QEasingCurve::OutBack));
    qmlRegisterSingletonType<DeclarativeInputEngine>("FreeVirtualKeyboard", 1, 0, "InputEngine", inputEngineProvider);
    connect(d->inputEngine, &DeclarativeInputEngine::animatingChanged, this, &VirtualKeyboardInputContext::ensureFocusedObjectVisible);
}

VirtualKeyboardInputContext::~VirtualKeyboardInputContext() {}

VirtualKeyboardInputContext *VirtualKeyboardInputContext::instance()
{
    static VirtualKeyboardInputContext *InputContextInstance = new VirtualKeyboardInputContext;
    return InputContextInstance;
}

bool VirtualKeyboardInputContext::isValid() const
{
    return true;
}

QRectF VirtualKeyboardInputContext::keyboardRect() const
{
    return QRectF();
}

void VirtualKeyboardInputContext::showInputPanel()
{
    d->visible = true;
    QPlatformInputContext::showInputPanel();
    emitInputPanelVisibleChanged();
}

void VirtualKeyboardInputContext::hideInputPanel()
{
    d->visible = false;
    QPlatformInputContext::hideInputPanel();
    emitInputPanelVisibleChanged();
}

bool VirtualKeyboardInputContext::isInputPanelVisible() const
{
    return d->visible;
}

bool VirtualKeyboardInputContext::isAnimating() const
{
    return false;
}

void VirtualKeyboardInputContext::setFocusObject(QObject *object)
{
    static const int NumericInputHints = Qt::ImhPreferNumbers | Qt::ImhDate | Qt::ImhTime | Qt::ImhDigitsOnly | Qt::ImhFormattedNumbersOnly;
    static const int DialableInputHints = Qt::ImhDialableCharactersOnly;

    if (!object) {
        return;
    }

    // we only support QML at the moment - so if this is not a QML item, then
    // we leave immediatelly
    d->focusItem = dynamic_cast<QQuickItem *>(object);
    if (!d->focusItem) {
        return;
    }

    // Check if an input control has focus that accepts text input - if not,
    // then we can leave immediatelly
    bool AcceptsInput = d->focusItem->inputMethodQuery(Qt::ImEnabled).toBool();
    if (!AcceptsInput) {
        return;
    }

    // Set input mode depending on input method hints queried from focused
    // object / item
    Qt::InputMethodHints InputMethodHints(d->focusItem->inputMethodQuery(Qt::ImHints).toInt());
    if (InputMethodHints & DialableInputHints) {
        d->inputEngine->setInputMode(DeclarativeInputEngine::Dialable);
    } else if (InputMethodHints & NumericInputHints) {
        d->inputEngine->setInputMode(DeclarativeInputEngine::Numeric);
    } else {
        d->inputEngine->setInputMode(DeclarativeInputEngine::Latin);
    }

    // Search for the top most flickable so that we can scroll the control
    // into the visible area, if the keyboard hides the control
    QQuickItem *i = d->focusItem;
    d->flickable = nullptr;
    while (i) {
        QQuickFlickable *Flickable = dynamic_cast<QQuickFlickable *>(i);
        if (Flickable) {
            d->flickable = Flickable;
            qDebug() << "is QQuickFlickable";
        }
        i = i->parentItem();
    }

    ensureFocusedObjectVisible();
}

void VirtualKeyboardInputContext::ensureFocusedObjectVisible()
{
    // If the keyboard is hidden, no scrollable element exists or the keyboard
    // is just animating, then we leave here
    if (!d->visible || !d->flickable || d->inputEngine->isAnimating()) {
        return;
    }

    QRectF FocusItemRect(0, 0, d->focusItem->width(), d->focusItem->height());
    FocusItemRect = d->flickable->mapRectFromItem(d->focusItem, FocusItemRect);
    d->flickableContentScrollAnimation->setTargetObject(d->flickable);
    qreal ContentY = d->flickable->contentY();
    if (FocusItemRect.bottom() >= d->flickable->height()) {
        ContentY = d->flickable->contentY() + (FocusItemRect.bottom() - d->flickable->height()) + 20;
        d->flickableContentScrollAnimation->setEndValue(ContentY);
        d->flickableContentScrollAnimation->start();
    } else if (FocusItemRect.top() < 0) {
        ContentY = d->flickable->contentY() + FocusItemRect.top() - 20;
        d->flickableContentScrollAnimation->setEndValue(ContentY);
        d->flickableContentScrollAnimation->start();
    }
}

QObject *VirtualKeyboardInputContext::inputEngineProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return VirtualKeyboardInputContext::instance()->d->inputEngine;
}
