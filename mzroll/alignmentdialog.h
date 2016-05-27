#ifndef ALIGNDIALOG_H
#define ALIGNDIALOG_H

#include <qdialog.h>
#include <qobjectdefs.h>

/**
 * \class AlignmentDialog
 *
 * \ingroup mzroll
 *
 * \brief Class for Alignment Dialog.
 *
 * This class is used for Alignment Dialog.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class AlignmentDialog: public QDialog {
Q_OBJECT

public:
	AlignmentDialog(QWidget *parent);
};

#endif
