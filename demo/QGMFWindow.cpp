#include "QGMFWindow.h"

#include "QGMFGraphics.h"
#include "StandardItems.h"

#include <QVector>
#include <QFontDatabase>
#include <QScreen>
#include <QPushButton>
#include <QGraphicsProxyWidget>

#include <QJsonDocument>

#include "ui_FileSrcNodeUI.h"

QGMFWindow::QGMFWindow(QWidget *parent)
  : QMainWindow(parent)
{
  setupUi(this);

  QRect r = qApp->primaryScreen()->availableGeometry();
  QPoint center = r.center();
  r.setSize(QSize(r.width()*0.9,r.height()*0.6));
  r.moveCenter(center);
  setGeometry(r);

  // 1. Create a QGMF scene given:
  //    . a Graph Model
  //    . a Graph name
  //    . an Item Factory (defaults to Standard factory)
  //
  // The Graph Model describes what kind of nodes/ports/edges are available
  // and how they can be linked together.
  // QGMF provides a Standard Graph Model with simple nodes/ports/edges
  //
  // The Item Factory creates graphical shapes for nodes/ports/edges
  //
  // The Standard Item Factory should fit to visualize any kind of graph
  // in general, by providing classic rectangular shapes for nodes
  // and simple lines for edges
  QGMF::Scene *scn = new QGMF::Scene(0,0,2000,2000,
                                     GStreamerModel::gsPipeline,      // the graph model
                                     "OGG Player GStreamer pipeline", // the name
                                     nullptr,                         // the factory
                                     this);

  // 2. Attach the scene to a QGraphicsView widget to visualize it
  view->setScene(scn);
  view->setSceneRect(scn->sceneRect().adjusted(-30,-30,30,30));

  // 3. Get the root node item (i.e. the Graph item) from which you can
  // add nodes/ports/edges
  QGMF::GraphItem *root = scn->graphItem();
  root->ensureVisible();
  root->setPos(50,50);

  // You can also access the underlying graph and graph models,
  // but these are strictly read only: all graph modifications must be
  // done through the graphics items
  // scn->graph();
  // scn->graphModel();

  // Node items inherit NodeItem class. This class provides basic node item
  // functions like addPortItem()
  // Additional behavior is implemented by the classes of the used factory
  // You need to know and cast to the appropriate type used by the factory
  // in order to access additional features
  ((QGMF::StandardNodeItem *) root)->setNodeStyle(QColor(70, 70, 70), QColor(240, 240, 245, 200));

  // 4. Add your nodes, ports and edges

  // file source
  auto *fileSrcNode = root->addNodeItem(GStreamerModel::gsNode, "File Source");
  // You should check that fileSrcNode is not null.
  // In general the model can prohibit
  // the creation of nodes/ports/edges (e.g. maximum port count reached, cannot add
  // that kind of edge between these nodes, ...)

  fileSrcNode->setPos(0,20); // relative to where parent node puts subnodes

  ((QGMF::StandardNodeItem *) fileSrcNode)->setNodeStyle(QColor(170, 0, 127), QColor(255, 246, 253));

  // The StandardNodeItem implementation provides ability to add custom content
  // to nodes. The content is made scrollable inside the node
  QGMF::Item *c = ((QGMF::StandardNodeItem *) fileSrcNode)->contentItem();
  // For example here, we add a QWidget
  QWidget *ui = new QWidget(nullptr);
  ui->setAttribute(Qt::WA_NoSystemBackground); // you need this for transparent background
  Ui::FileSrcNodeUI().setupUi(ui);
  QGraphicsProxyWidget *w = new QGraphicsProxyWidget(c); // QWidgets must be wrapped into QGraphicsProxyWidget for the scene
  w->setWidget(ui);

  fileSrcNode->resizeToContents(); // Fit to contents

  // Add some port
  auto *fileSrcNode_src = fileSrcNode->addPortItem(GStreamerModel::gsSource, "src");
  // The Standard Factory will automatically position ports on the node,
  // depending on their nature (sources, sinks or both)
  // You can however force the position and the angle

  //fileSrcNode_src->setPos(175,50); // relative to where node puts ports
  //fileSrcNode_src->setAngle(0); // in degrees

  // OGG demuxer
  auto *oggDemuxNode = root->addNodeItem(GStreamerModel::gsNode, "OGG Demux");
  oggDemuxNode->setPos(300,150);
  ((QGMF::StandardNodeItem *) oggDemuxNode)->setNodeStyle(QColor(0, 85, 127), QColor(240, 250, 255));

  auto *oggDemuxNode_sink = oggDemuxNode->addPortItem(GStreamerModel::gsSink, "sink");
  auto *oggDemuxNode_src1 = oggDemuxNode->addPortItem(GStreamerModel::gsSource, "src1");
  auto *oggDemuxNode_src2 = oggDemuxNode->addPortItem(GStreamerModel::gsSource, "src2");

  // Vorbis decoder
  auto *vorbisNode = root->addNodeItem(GStreamerModel::gsNode, "Vorbis decode");
  vorbisNode->setPos(600,50);
  ((QGMF::StandardNodeItem *) vorbisNode)->setNodeStyle(QColor(85, 85, 0), QColor(250, 250, 240));

  auto *vorbisNode_sink = vorbisNode->addPortItem(GStreamerModel::gsSink, "sink");
  auto *vorbisNode_src  = vorbisNode->addPortItem(GStreamerModel::gsSource, "src");

  // Theora decoder
  auto *theoraNode = root->addNodeItem(GStreamerModel::gsNode, "Theora decode");
  theoraNode->setPos(600,250);
  ((QGMF::StandardNodeItem *) theoraNode)->setNodeStyle(QColor(85, 85, 0), QColor(250, 250, 240));

  auto *theoraNode_sink = theoraNode->addPortItem(GStreamerModel::gsSink, "sink");
  auto *theoraNode_src  = theoraNode->addPortItem(GStreamerModel::gsSource, "src");

  // Audio out
  auto *audioNode = root->addNodeItem(GStreamerModel::gsNode, "Audio out");
  audioNode->setPos(900,50);
  ((QGMF::StandardNodeItem *) audioNode)->setNodeStyle(QColor(0, 127, 85), QColor(240, 250, 240));

  auto *audioNode_sink = audioNode->addPortItem(GStreamerModel::gsSink, "sink");

  // Video out
  auto *videoNode = root->addNodeItem(GStreamerModel::gsNode, "Video out");
  videoNode->setPos(900,250);
  ((QGMF::StandardNodeItem *) videoNode)->setNodeStyle(QColor(0, 85, 85), QColor(240, 250, 240));

  auto *videoNode_sink = videoNode->addPortItem(GStreamerModel::gsSink, "sink");

  // Edges
  // NOTE An edge that connects two nodes N1 and N2 must be added at the
  // parent node level
  root->addEdgeItem(GStreamerModel::gsConnection, fileSrcNode_src, oggDemuxNode_sink);
  root->addEdgeItem(GStreamerModel::gsConnection, oggDemuxNode_src1, vorbisNode_sink);
  root->addEdgeItem(GStreamerModel::gsConnection, oggDemuxNode_src2, theoraNode_sink);
  root->addEdgeItem(GStreamerModel::gsConnection, vorbisNode_src, audioNode_sink);
  root->addEdgeItem(GStreamerModel::gsConnection, theoraNode_src, videoNode_sink);

  // You can remove nodes/edges/ports simply using delete
  // delete vorbisNode;

  // Resize root node so that it contains all it subnodes
  root->resizeToContents();

  // misc
  connect(gridBtn,&QToolButton::toggled, view,&QGMF::View::setGridVisible);
  connect(rulerBtn,&QToolButton::toggled, this,[&](bool b) { view->setRulerVisible(b,b); });
}

QGMFWindow::~QGMFWindow()
{
}
