#ifndef QGMFGRAPHICS_CAPTIONITEM_H
#define QGMFGRAPHICS_CAPTIONITEM_H

#include "QGMFGraphicsGlobals.h"

#include "StandardCaptionStyle.h"

#include "Item.h"

namespace QGMF {

class StandardCaptionTextItem;
class StandardCaptionRectItem;

class QGMFGRAPHICS_PUBLIC StandardCaptionItem : public Item {
  public:
    StandardCaptionItem(QGraphicsItem *parent);
    virtual ~StandardCaptionItem() = default;

    void setText(const QString &text);
    QString text() const { return m_text; }
    QRectF textRect() const;

    void setMargins(const QMargins &m);
    QMargins margins() const { return m_margins; }

    void setCaptionStyle(const StandardCaptionStyle &s);
    void setCaptionStyle(const QColor &fg, const QColor &bg);
    const StandardCaptionStyle &captionStyle() const { return m_style; }

  private:
    StandardCaptionStyle m_style;

    QString m_text;
    QMargins m_margins;
    StandardCaptionTextItem *m_textItem;
    StandardCaptionRectItem *m_rectItem;
};

}

#endif // CAPTIONITEM_H
