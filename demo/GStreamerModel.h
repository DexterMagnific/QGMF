#ifndef GSTREAMERMODEL_H
#define GSTREAMERMODEL_H

// QGMF lib
#include "QGMF.h"

// Example of a model to implement GStreamer pipelines
namespace GStreamerModel {

// We start by ports. We would like to distinguish between sources and sinks
QGMF_DECLARE_PORT_MODEL(gsSource, "Source");
QGMF_DECLARE_PORT_MODEL(gsSink, "Sink");

// Then edges that connect ports. From a source port to a sink port
QGMF_DECLARE_EDGE_MODEL(gsConnection, "Source to Sink connection", QGMF_MODELS(gsSource), QGMF_MODELS(gsSink));

// Then nodes that contain ports. Both sources and sinks are possible in a node
QGMF_DECLARE_NODE_MODEL(gsNode, "Node", QGMF_MODELS(gsSource, gsSink));

// Finally the graph that contains nodes and edges
QGMF_DECLARE_GRAPH_MODEL(gsPipeline, "Pipeline", QGMF_MODELS(gsNode), QGMF_MODELS(gsConnection));

};

#endif // GSTREAMERMODEL_H
