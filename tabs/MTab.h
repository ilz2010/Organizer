#ifndef ORGANIZER_MTAB_H
#define ORGANIZER_MTAB_H

#include <QtWidgets/QWidget>
#include <QtCore/QJsonObject>
#include <QtCore/QMap>


class MTab : public QWidget {
public:
    enum TabType : int {
	    Text = 1,
	    List = 2,
	    LinksGroup = 4,
	    FilesGroup = 8,
	    NewTab = 1024
    };

	enum TabGroup : int {
		All = Text | List | LinksGroup | FilesGroup,
		Editors = Text | List,
		Links = LinksGroup,
		Files = FilesGroup
	};

private:
	qint64 u_time = 0, u_last = 0;
	bool timer_not_started = true;
	QJsonObject obj;

	TabType type = NewTab;

protected:
	QString name;

	void saveStorage();

public:
	explicit MTab() = default;

    explicit MTab(const QJsonObject &o, TabType t = NewTab);

    void load(QJsonObject o = QJsonObject());

	QJsonObject save();

	void updated();

	QString getName() { return name; }

	bool isInGroup(TabGroup gr) { return gr & type; }

	virtual QString getDesc() = 0;

	virtual void importFrom(QString s) {};

	virtual QString exportTo() { return ""; };

	virtual void fromJson(QJsonValue v) {};

	virtual QJsonValue toJson() { return QJsonObject(); };

	virtual void loadCustomParams(const QJsonObject &o) {}

	virtual void saveCustomParams(QJsonObject &o) {}
};


#endif //ORGANIZER_MTAB_H
