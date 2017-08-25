/***************************************************************************
 *   Copyright (C) 2017 Hendrik Vennekate                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "librarymodel.h"
#include "constants.h"
#include "molecule.h"
#include "stringify.h"
#include "moleculemodelitem.h"

#include <QElapsedTimer>
#include <QIcon>
#include <QMimeData>
#include <QSet>
#include <QTimer>

namespace Molsketch {
  const int itemIncrement = 10;

  class LibraryModelPrivate {
    Q_DISABLE_COPY(LibraryModelPrivate)
  public:
    LibraryModelPrivate() : fetchCount(0){}
    QList<MoleculeModelItem*> items;
    int fetchCount;
    void cleanMolecules() {
      qInfo("Clearing list of molecules. Count: %d", items.size());
      for (auto molecule : items.toSet()) delete molecule;
      items.clear();
      fetchCount = 0;
    }
    ~LibraryModelPrivate() {
      cleanMolecules();
    }
  };

LibraryModel::LibraryModel(QObject *parent)
  : QAbstractListModel(parent),
    d_ptr(new LibraryModelPrivate)
{}

LibraryModel::~LibraryModel() {}

int LibraryModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  Q_D(const LibraryModel);
  return d->fetchCount;
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const {
  Q_D(const LibraryModel);
  int row = index.row();
  if (row < 0 || row >= d->items.size()) return QVariant();
  if (Qt::DecorationRole == role) return d->items[row]->icon();
  if (Qt::DisplayRole == role) return d->items[row]->name();
  return QVariant();
}

QMimeData *LibraryModel::mimeData(const QModelIndexList &indexes) const {
  qDebug("Preparing MIME data of molecules: %s",
         stringify<QModelIndex>(indexes,
                                [](const QModelIndex& i) {return QString::number(i.row());}).toUtf8().data());
  Q_D(const LibraryModel);
  QMimeData *result = new QMimeData;
  QByteArray xml;
  QXmlStreamWriter out(&xml);
  out.writeStartDocument();
  for (QModelIndex index : indexes) {
    int row = index.row();
    if (row >= 0 && row < d->items.size())
      d->items[row]->writeXml(out);
  }
  out.writeEndDocument();
  result->setData(moleculeMimeType, xml);
  return result;
}

void LibraryModel::setMolecules(QList<MoleculeModelItem *> molecules) {
  qDebug("Setting molecules");
  Q_D(LibraryModel);
  beginResetModel();
  d->cleanMolecules();
  d->items = molecules;
  endResetModel();
}

void LibraryModel::addMolecule(MoleculeModelItem *molecule) {
  qDebug("Adding molecule");
  Q_D(LibraryModel);
  d->items << molecule;
}

QStringList LibraryModel::mimeTypes() const {
  return  moleculeMimeTypes;
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const {
  return QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled;
}

bool LibraryModel::canFetchMore(const QModelIndex &) const {
  Q_D(const LibraryModel);
  return d->fetchCount < d->items.size();
}

void LibraryModel::fetchMore(const QModelIndex &) {
  Q_D(LibraryModel);
  int newCount = qMin(d->fetchCount + itemIncrement, d->items.size());
  beginInsertRows(QModelIndex(), d->fetchCount, newCount-1);
  d->fetchCount = newCount;
  endInsertRows();
}

} // namespace Molsketch