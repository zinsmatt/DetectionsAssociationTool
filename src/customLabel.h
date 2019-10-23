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

#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>
#include <QMouseEvent>

class MainWindow;

class CustomLabel : public QLabel
{
Q_OBJECT
signals:
    void mousePressed( const QPoint& );

public:
    CustomLabel( QWidget* parent = 0, Qt::WindowFlags f = 0 );
    CustomLabel( const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0 );

    void mousePressEvent( QMouseEvent* ev );

    void setMainWindow(MainWindow* pt) {
      main_window = pt;
    }
private:
    MainWindow* main_window;
};


#endif // CUSTOMLABEL_H
