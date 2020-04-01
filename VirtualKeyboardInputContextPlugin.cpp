#include "VirtualKeyboardInputContextPlugin.h"
#include "VirtualKeyboardInputContext.h"
//============================================================================
/// \file   VirtualKeyboardInputContextPlugin.cpp
/// \author Uwe Kindler
/// \date   08.01.2015
/// \brief  Implementation of VirtualKeyboardInputContextPlugin
///
/// Copyright 2015 Uwe Kindler
/// Licensed under MIT see LICENSE.MIT in project root
//============================================================================

QPlatformInputContext* VirtualKeyboardInputContextPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList)

    if (QString::compare(system, QStringLiteral("freevirtualkeyboard"), Qt::CaseInsensitive) == 0) {
        return VirtualKeyboardInputContext::instance();
    }
    return nullptr;
}
