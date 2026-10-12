TEMPLATE = subdirs

#CONFIG += ordered

SUBDIRS += \
  lib  \
  graphics \
  demo

lib.file = lib/QGMFlib.pro
graphics.file = graphics/QGMFGraphics.pro
demo.file = demo/QGMFDemo.pro

demo.depends = lib graphics
