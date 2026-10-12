#ifndef QGMFGRAPHICS_VIEW_H
#define QGMFGRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QFont>

#include "GridStyle.h"
#include "RulerStyle.h"

namespace QGMF {

class QGMFGRAPHICS_PUBLIC View : public QGraphicsView
{
    Q_OBJECT

  public:
    View(QGraphicsScene *scene, QWidget *parent = nullptr);
    View(QWidget *parent = nullptr);

    virtual ~View() { }

    // Ruler
    void setRulerStep(qreal val);
    void setRulerStepDivisor(unsigned val);
    void setRulerSize(qreal val);
    void setRulerFont(const QFont &f);
    QRectF hRulerRect() const;
    QRectF vRulerRect() const;

    // Grid
    void setGridStep(qreal val);
    void setGridStepDivisor(unsigned val);

    // Tracker

  public slots:
    void setGridVisible(bool val);
    void setRulerVisible(bool h, bool v);

  protected:
    // drawing functions
    virtual void drawGrid(QPainter *painter, const QRectF &rect);
    virtual void drawRuler(QPainter *painter, const QRectF &rect);

    // map scene coordinate to ruler number
    virtual qreal mapToHRuler(qreal x);
    virtual qreal mapToVRuler(qreal y);

    // reimplementations
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

  private:
    // invalidate background regions. xpos and ypos are scrollbar values
    void scrollInvalidate(const QRectF &rect);
    void invalidateBackground();

  private:
    GridStyle m_gridStyle;
    RulerStyle m_rulerStyle;

    bool m_grid;
    qreal m_gridStep;
    unsigned m_gridStepDiv;

    bool m_hruler;
    bool m_vruler;
    qreal m_rulerStep;
    unsigned m_rulerStepDiv;
    qreal m_rulerSz;

    int m_hpos;
    int m_vpos;

    bool m_moving;
    QPoint m_moveStart;
    QCursor m_moveOrigCursor;
};

}
#endif // VIEW_H
