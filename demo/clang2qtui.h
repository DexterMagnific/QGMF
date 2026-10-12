#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>

#include <QtLongLongSpinBox>
#include <CtkCollapsibleButton>

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

namespace Clang2QtUi {
    inline ctkCollapsibleButton *addGroup(const QString &text, QWidget *parent)
    {
      QGridLayout *layout = (QGridLayout *) parent->layout();
      const auto row = layout->rowCount();

      ctkCollapsibleButton *group = new ctkCollapsibleButton(text, parent);
      layout->addWidget(group, row,0, 1,3);

      QGridLayout *groupLayout = new QGridLayout(group);
      groupLayout->setContentsMargins(10,0,0,0);

      return group;
    }

    inline QComboBox *addEnum(const QString &text, QWidget *parent, QStringList values = QStringList())
    {
      QGridLayout *layout = (QGridLayout *) parent->layout();
      const auto row = layout->rowCount();

      QComboBox *w = new QComboBox(parent);
      w->addItems(values);
      layout->addWidget(new QLabel(text, parent), row,0, 1,1);
      layout->addItem(new QSpacerItem(10, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum), row,1, 1, 1);
      layout->addWidget(w, row,2, 1,1);
      return w;
    }

    inline QSpinBox *addInteger(const QString &text, QWidget *parent)
    {
      QGridLayout *layout = (QGridLayout *) parent->layout();
      const auto row = layout->rowCount();

      QSpinBox *w = new QSpinBox(parent);
      layout->addWidget(new QLabel(text, parent), row,0, 1,1);
      layout->addItem(new QSpacerItem(10, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum), row,1, 1, 1);
      layout->addWidget(w, row,2, 1,1);
      return w;
    }

    inline QtLongLongSpinBox *addBigInteger(const QString &text, QWidget *parent)
    {
      QGridLayout *layout = (QGridLayout *) parent->layout();
      const auto row = layout->rowCount();

      QtLongLongSpinBox *w = new QtLongLongSpinBox(parent);
      layout->addWidget(new QLabel(text, parent), row,0, 1,1);
      layout->addItem(new QSpacerItem(10, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum), row,1, 1, 1);
      layout->addWidget(w, row,2, 1,1);
      return w;
    }

    inline QDoubleSpinBox *addDecimal(const QString &text, QWidget *parent)
    {
      QGridLayout *layout = (QGridLayout *) parent->layout();
      const auto row = layout->rowCount();

      QDoubleSpinBox *w = new QDoubleSpinBox(parent);
      layout->addWidget(new QLabel(text, parent), row,0, 1,1);
      layout->addItem(new QSpacerItem(10, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum), row,1, 1, 1);
      layout->addWidget(w, row,2, 1,1);
      return w;
    }

    inline QLineEdit *addText(const QString &text, QWidget *parent)
    {
      QGridLayout *layout = (QGridLayout *) parent->layout();
      const auto row = layout->rowCount();

      QLineEdit *w = new QLineEdit(parent);
      layout->addWidget(new QLabel(text, parent), row,0, 1,1);
      layout->addItem(new QSpacerItem(10, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum), row,1, 1, 1);
      layout->addWidget(w, row,2, 1,1);
      return w;
    }

    inline QCheckBox *addBool(const QString &text, QWidget *parent)
    {
      QGridLayout *layout = (QGridLayout *) parent->layout();
      const auto row = layout->rowCount();

      QCheckBox *w = new QCheckBox(parent);
      layout->addWidget(new QLabel(text, parent), row,0, 1,1);
      layout->addItem(new QSpacerItem(10, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum), row,1, 1, 1);
      layout->addWidget(w, row,2, 1,1);
      return w;
    }

    inline QLabel *addUnsupported(const QString &text, QWidget *parent)
    {
      QGridLayout *layout = (QGridLayout *) parent->layout();
      const auto row = layout->rowCount();

      QLabel *w = new QLabel(text, parent);
      layout->addWidget(new QLabel(text, parent), row,0, 1,1);
      layout->addItem(new QSpacerItem(10, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum), row,1, 1, 1);
      layout->addWidget(w, row,2, 1,1);
      return w;
    }
};
