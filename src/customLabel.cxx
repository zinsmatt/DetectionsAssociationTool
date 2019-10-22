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
