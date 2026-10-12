.. _prog-guide:

Programming Guide
=================

Including QGMF
--------------

To use QGMF in your project, you need to build it separately (see
:ref:`source-build`) first.

.. warning:: Even if QGMF lib is a C++ only lib, QGMF build system is
             written in Qt's QMake. QMake is needed even if you only
             intend to QGMF lib alone.

.. note:: Currently, QGMF does not come as an *embeddable* subproject
          for your project.
             
* To use the QGMF lib alone, you need to link against the resulting
  ``libQGMF.so``.

  .. code:: shell

     # in your build script, pass the path to QGMF lib build dir
     -L/path/to/QGMF/build/lib

     # pass also the QGMF lib include dir
     -I/path/to/QGMF/lib

     # then link with QGMF lib
     -lQGMF

* To use QGMF Graphics, you need both QGMF and grahics libs.

  .. code:: shell

     # in your build script, pass the path to QGMF build dirs
     -L/path/to/QGMF/build/lib -L/path/to/QGMF/build/graphics

     # pass also the QGMF include dirs
     -I/path/to/QGMF/lib -I/path/to/QGMF/graphics

     # then link with both libs
     -lQGMF -lQGMFGraphics


* In a header or cpp file, add the appropriate includes:

  .. code:: c++

     // for QGMF lib only
     #include <QGMF.h>

     // for QGMF graphics (will auto include lib)
     #include <QGMFGraphics.h>

     
.. _decl-model:

Declaring Models
----------------

If the :ref:`std-model` does not fit your needs, you can declare your
own graph model. A graph model declaration consists in informing QGMF
about the kind of nodes, edges and ports supported by your graph, and
describe the basic relationships on them.

In this section we show how to declare a graph model to represent a
simple GStreamer pipeline.

A model declaration is always located in a ``.h`` file and starts by
declaring port models:

.. code:: c++

   # GStreamerModel.h

   QGMF_DECLARE_PORT_MODEL(gsSource, "Source");
   QGMF_DECLARE_PORT_MODEL(gsSink, "Sink");

This tells QGMF that there two port models with the C identifiers
``gsSource`` (resp. ``gsSink``) and the friendly names ``Source``
(resp. ``Sink``).

.. note:: in QGMF, ports are not explicitely marked as input, output or
          bidirectional ports. Direction of port is inferred by
          looking at the potential incoming and outgoing edges.

.. note:: You need to declare all your ports now even if they belong
          to a deeply nested node.

After ports, we declare edges:

.. code:: c++

   QGMF_DECLARE_EDGE_MODEL(gsConnection, "Source to Sink connection", QGMF_MODELS(gsSource), QGMF_MODELS(gsSink));
   
In addition to a C identifier and a friendly name, an edge model
declaration take two additional arguments in the form of a comma
separated list inside the ``QGMF_MODELS(...)`` directive. The first
list is the list of **source** ports and the second one is the list of
**destination** ports.

.. note:: In QGMF, edges are always directed: they come from a *source
          port* and go to a *destination port*.

We then declare nodes:

.. code:: c++

   QGMF_DECLARE_NODE_MODEL(gsNode, "Node", QGMF_MODELS(gsSource, gsSink));

Declaration of a node model that does not contain subnodes requires
only the list of its supported port models.

If the node is to contain subnodes, the directive is slightly
different:

.. code:: c++

   QGMF_DECLARE_COMPLEX_NODE_MODEL(gsNode, "Node", QGMF_MODELS(portmodels....), QGMF_MODELS(subnodemodels...), QGMF_MODELS(edgemodels...))

We need to specify the node ports, but also the list of its subnodes
and the list of edges that link the subnodes

Finally we declare our GStreamer graph:

.. code:: c++

   QGMF_DECLARE_GRAPH_MODEL(gsPipeline, "Pipeline", QGMF_MODELS(gsNode), QGMF_MODELS(gsConnection));

This tells that our graph contains ``gsNode`` nodes linked together
using ``gsConnection`` edges.

.. _create-graph:

Advanced Models
---------------

TODO


Lib vs Graphics
---------------

Remember that you can chose to create the graph either in C++
mode only using the *lib* or in Qt using *graphics*. Recall that
whichever you chose, the APIs are **similar**, the graphics one having
the ``Item`` suffix.

But there is one notable difference between the lib and the graphics
that consists in the way objects (nodes, ports, edges,
lists,...) are passed and returned.

In the **lib** world, objects are passed and returned by
**reference**. For instance, creating a new node ``n`` will return a
reference:

.. code:: c++

   # Lib
   Node &n1 = g.addNode(...);
   Node &n2 = g.addNode(...);

and passing source and destination ports when creating an edge is also
done as references:

.. code::
   
   Edge &e = g.addEdge(..., src_port,dest_port);

with ``src_port`` and ``dst_port`` being the result of a ``Port &port
= n1.addPort(....)`` call.

When needing to express 'none', QGMF provides the ``Node::null()``,
``Port::null()`` and ``Edge::null()`` special objects that are
returned if for instance the model refuses to create a node or a node
lookup has failed. Returned objects can be compared to one of the null
objects to determine whether the operation succeeded or
not. Alternatively, one can check for 'none' value on the returned
object using the ``isNull()`` call:

.. code::
   
   Node &n1 = g.addNode(...);
   if ( n1 == Node::null() ) {
       ...
   }
   # same as
   if ( n1.isNull() ) {
      ...
   }
   # same as
   if ( n1 ) {
      ...
   }

In the **graphics** world, objects are passed and returned by
**pointer** and ``nullptr`` is returned as none.

.. code:: c++

   # Graphics
   NodeItem *n1 = g->addNodeItem(...);
   EdgeItem *e = g->addEdgeItem(n1,n2);



Creating Graphs
---------------

Now that we have a model, we can create a graph based on it. This is done in a
``cpp`` and depends on whether you use the lib alone or the graphics.


Lib
~~~

.. code:: c++

   dd
   

Accessing items, models and instances
-------------------------------------

Working with models
-------------------

Working with instances
----------------------

Working with items
------------------

