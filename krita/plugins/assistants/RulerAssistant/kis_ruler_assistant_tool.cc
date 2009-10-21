/*
 * Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <kis_ruler_assistant_tool.h>

#include <qpainter.h>

#include <kis_debug.h>
#include <klocale.h>

#include <KoViewConverter.h>
#include <KoPointerEvent.h>

#include <canvas/kis_canvas2.h>
#include <kis_cursor.h>
#include <kis_image.h>
#include <kis_view2.h>

#include "RulerAssistant.h"
#include "Ruler.h"
#include <kis_painting_assistants_manager.h>

KisRulerAssistantTool::KisRulerAssistantTool(KoCanvasBase * canvas)
        : KisTool(canvas, KisCursor::arrowCursor()), m_canvas(dynamic_cast<KisCanvas2*>(canvas))
{
    Q_ASSERT(m_canvas);
    setObjectName("tool_rulerassistanttool");
    m_widget = 0;
}

KisRulerAssistantTool::~KisRulerAssistantTool()
{
}

QPointF adjustPointF(const QPointF& _pt, const QRectF& _rc)
{
    return QPointF(qBound(_rc.left(), _pt.x(), _rc.right()), qBound(_rc.top(), _pt.y(), _rc.bottom()));
}

void KisRulerAssistantTool::activate(bool)
{
    // Add code here to initialize your tool when it got activated
    KisTool::activate();

    RulerAssistant* m_rulerAssistant = new RulerAssistant();

    QRectF imageArea = QRectF(pixelToView(QPoint(0, 0)),
                              m_canvas->image()->pixelToDocument(QPoint(m_canvas->image()->width(), m_canvas->image()->height())));

    m_canvas->view()->paintingAssistantManager()->addAssistant(m_rulerAssistant);

    dbgPlugins << imageArea << *m_rulerAssistant->handles()[0] << *m_rulerAssistant->handles()[1];

    m_handles = m_canvas->view()->paintingAssistantManager()->handles();
    m_canvas->view()->paintingAssistantManager()->setVisible(true);
    m_canvas->updateCanvas();
    m_handleDrag = 0;
}

void KisRulerAssistantTool::deactivate()
{
    // Add code here to initialize your tool when it got deactivated
    KisTool::deactivate();
}


inline double norm2(const QPointF& p)
{
    return sqrt(p.x() * p.x() + p.y() * p.y());
}

void KisRulerAssistantTool::mousePressEvent(KoPointerEvent *event)
{
    m_handleDrag = 0;
    foreach(const KisPaintingAssistantHandleSP handle, m_handles) {
        if (norm2(event->point - *handle) < 10) {
            m_canvas->updateCanvas(); // TODO update only the relevant part of the canvas
            m_handleDrag = handle;
            break;
        }
    }
    if (!m_handleDrag) {
        event->ignore();
    }
}


void KisRulerAssistantTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (m_handleDrag) {
        *m_handleDrag = event->point;
        m_canvas->updateCanvas();
    } else {
        event->ignore();
    }
}

void KisRulerAssistantTool::mouseReleaseEvent(KoPointerEvent *event)
{
    if (m_handleDrag) {
        m_handleDrag = 0;
        m_canvas->updateCanvas(); // TODO update only the relevant part of the canvas
    } else {
        event->ignore();
    }
}

void KisRulerAssistantTool::paint(QPainter& _gc, const KoViewConverter &_converter)
{
    foreach(const KisPaintingAssistantHandleSP handle, m_handles) {
        if (handle == m_handleDrag) {
            _gc.setBrush(QColor(0, 0, 0, 125));
        } else {
            _gc.setBrush(QColor(0, 0, 0, 0));
        }
        _gc.drawEllipse(QRectF(_converter.documentToView(*handle) -  QPointF(5, 5), QSizeF(10, 10)));
    }
}


#include "kis_ruler_assistant_tool.moc"
