#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include "graphicsitem.h"
#include "molscene.h"
#include "coloraction.h"
#include "linewidthaction.h"

namespace Molsketch {

  graphicsItem::graphicsItem(QGraphicsItem *parent GRAPHICSSCENESOURCE)
    : QGraphicsItem(parent GRAPHICSSCENEINIT),
      lineWidthScaling(1)
  {
//    setCacheMode(QGraphicsItem::NoCache);
  }

  void graphicsItem::setColor(const QColor& color)
  {
    m_color = color ;
  }

  QColor graphicsItem::getColor() const
  {
    return m_color ;
  }

  void graphicsItem::setRelativeWidth(const double &w)
  {
    lineWidthScaling = w ;
  }

  qreal graphicsItem::lineWidth() const
  {
    qreal lw = lineWidthScaling ;
    MolScene *myScene = qobject_cast<MolScene*>(scene()) ;
    if (myScene) lw *= sceneLineWidth(myScene) ;
    return lw ;
  }

  qreal graphicsItem::relativeWidth() const
  {
    return lineWidthScaling ;
  }

  void graphicsItem::readAttributes(const QXmlStreamAttributes &attributes)
  {
    readGraphicAttributes(attributes) ;
    setColor(QColor(attributes.value("colorR").toString().toInt(),
                    attributes.value("colorG").toString().toInt(),
                    attributes.value("colorB").toString().toInt())) ;
    if (attributes.hasAttribute("scalingParameter"))
      lineWidthScaling = attributes.value("scalingParameter").toString().toFloat() ;
    else
      lineWidthScaling = 1.0 ;
  }

  QXmlStreamAttributes graphicsItem::xmlAttributes() const
  {
    QXmlStreamAttributes attributes = graphicAttributes() ;
    attributes.append("colorR", QString::number(getColor().red())) ;
    attributes.append("colorG", QString::number(getColor().green())) ;
    attributes.append("colorB", QString::number(getColor().blue())) ;
    attributes.append("scalingParameter", QString::number(lineWidthScaling));
    return attributes ;
  }

  qreal graphicsItem::sceneLineWidth(MolScene *scene) const
  {
    return scene->bondWidth() ;
  }

  void graphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
  {
    QMenu *contextMenu = new QMenu(QObject::tr("Bond actions")) ;
    colorAction *myAction = new colorAction(contextMenu) ;
    lineWidthAction *otherAction = new lineWidthAction(contextMenu) ;
    myAction->setItem(this) ;
    otherAction->setItem(this) ;
    contextMenu->addAction(myAction) ;
    contextMenu->addAction(otherAction) ;
    contextMenu->exec(event->screenPos()) ;
    delete contextMenu ;
    event->accept();
  }

  arrowGraphicsItem::arrowGraphicsItem(QGraphicsItem *parent GRAPHICSSCENESOURCE )
    : graphicsItem(parent GRAPHICSSCENEINIT )
  {}

  qreal arrowGraphicsItem::sceneLineWidth(MolScene *scene) const
  {
    return scene->arrowLineWidth() ;
  }

} // namespace
