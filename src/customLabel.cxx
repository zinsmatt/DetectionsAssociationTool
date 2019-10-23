//=========================================================================
//
// Copyright 2019
// Author: Matthieu Zins
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "customLabel.h"

#include "MainWindow.h"

void CustomLabel::mousePressEvent( QMouseEvent* ev )
{
    const QPoint p = ev->pos();
    if (main_window)
      main_window->click(p.x(), p.y());
}

CustomLabel::CustomLabel( QWidget * parent, Qt::WindowFlags f )
    : QLabel( parent, f ) {}

CustomLabel::CustomLabel( const QString& text, QWidget* parent, Qt::WindowFlags f )
    : QLabel( text, parent, f ) {}
