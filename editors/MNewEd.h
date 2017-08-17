#ifndef ORGANIZER_MNEWED_H
#define ORGANIZER_MNEWED_H

#include <mwgt.h>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include "MTabsController.h"

class MWindow;

class MTabsController;

class MNewEd : public MTab {
	MWindow *wnd;
	MTabsController *contr;

	QComboBox *type;
	QLineEdit *name;

public:
	MNewEd(MWindow *w, MTabsController *c);

	void addClick();

	QString getDesc() override { return "New Tab"; }

	void importFrom(QString s) override { Q_UNUSED(s) }

	QString exportTo() override { return ""; }

	void fromJson(QJsonValue v) override {}

	QJsonValue toJson() override { return QJsonValue(); }

	~MNewEd() override = default;

	friend class MWindow;
};

#endif //ORGANIZER_MNEWED_H
