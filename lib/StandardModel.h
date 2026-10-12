#ifndef STANDARDMODEL_H
#define STANDARDMODEL_H

// QGMF lib
#include "QGMF.h"

// The standard model provides one kind of node and edge, and two kinds of
// ports (inputs and outputs)

namespace QGMF {

// We start by ports.
QGMF_DECLARE_PORT_MODEL(stdInput, "Input");
QGMF_DECLARE_PORT_MODEL(stdOutput, "Output");

// Then edges that connect ports. From an output port to an input port
QGMF_DECLARE_EDGE_MODEL(stdConnection, "Input to Output connection", QGMF_MODELS(stdOutput), QGMF_MODELS(stdInput));

// Then nodes that contain I/O ports.
QGMF_DECLARE_NODE_MODEL(stdNode, "Node", QGMF_MODELS(stdInput, stdOutput));

// Finally the graph that contains nodes and edges
QGMF_DECLARE_GRAPH_MODEL(stdGraph, "Graph", QGMF_MODELS(stdNode), QGMF_MODELS(stdConnection));

};

#endif // STANDARDMODEL_H
