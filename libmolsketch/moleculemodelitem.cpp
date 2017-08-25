#include "moleculemodelitem.h"
#include "molecule.h"
#include <QIcon>
#include <QLibrary>
#include <QDebug>

#ifdef _WIN32
#define OBABELOSSUFFIX ".dll"
#else
#define OBABELOSSUFFIX
#endif

namespace Molsketch {

  class MoleculeModelItemPrivate {
    Q_DISABLE_COPY(MoleculeModelItemPrivate)
  public:
    MoleculeModelItemPrivate()
      : molecule(nullptr)
    {}
    Molecule *molecule;
    QIcon icon;
    ~MoleculeModelItemPrivate() {
      delete molecule;
    }
    void ensureMolecule(MoleculeModelItem* item) {
      if (molecule) return;
      molecule = item->produceMolecule();
      qDebug() << "produced molecule: " << (void*) molecule << (void*) item;
    }
  };

  MoleculeModelItem::MoleculeModelItem()
    : d_ptr(new MoleculeModelItemPrivate)
  {}

  MoleculeModelItem::~MoleculeModelItem() {}

  QIcon MoleculeModelItem::icon() {
    Q_D(MoleculeModelItem);
    d->ensureMolecule(this);
    if (!d->molecule) return QIcon();
    if (d->icon.isNull()) d->icon = QIcon(renderMolecule(*(d->molecule)));
    return d->icon;
  }

  QString MoleculeModelItem::name() {
    Q_D(MoleculeModelItem);
    d->ensureMolecule(this);
    if (!d->molecule) return QString();
    return d->molecule->getName();
  }

  void MoleculeModelItem::writeXml(QXmlStreamWriter &writer) {
    Q_D(MoleculeModelItem);
    d->ensureMolecule(this);
    if (!d->molecule) return;
    writer << *(d->molecule);
  }

  MoleculeModelItem *MoleculeModelItem::fromXml(QByteArray xml) {
    class XmlItem : public MoleculeModelItem {
        Molecule* molecule;
    public:
        XmlItem(const QByteArray& input)
          : molecule (new Molecule())
        {
          QXmlStreamReader in(input);
          in >> *molecule;
        }
        Molecule *produceMolecule() const {
          return molecule;
        }
      };
    return new XmlItem(xml);
  }
} // namespace Molsketch