#include "mwgt.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtGui/QHideEvent>
#include <editors/MEditorsController.h>
#include <links/MLinksController.h>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <network/NAuth.h>

// Constructor
MWindow::MWindow() {
	tabs = new QTabWidget;

	constructMenuBar();

	qApp->setWindowIcon(QIcon(":/icon.ico"));

	// Create tray
	tray = new QSystemTrayIcon;
	tray->setIcon(QIcon(":/icon.ico"));
	tray->show();

	auto *trayMenu = new QMenu;

	QAction *trayClick = new QAction("Toggle state");
	connect(trayClick, &QAction::triggered, this, &MWindow::trayToggle);

	trayMenu->addAction(trayClick);
	tray->setContextMenu(trayMenu);
	// Create tray

	connect(tabs, &QTabWidget::currentChanged, this, &MWindow::tabChange);
	connect(tray, &QSystemTrayIcon::activated, this, &MWindow::trayClick);

	changeWidget();

	updateMenu();

	setCentralWidget(tabs);
	setGeometry(100, 100, 750, 500);
}

void MWindow::constructMenuBar() {
	QMenuBar *menu = this->menuBar();

	mfile = new QMenu("File");
	mfile->addAction("Import", this, &MWindow::importFrom);
	mfile->addAction("Export", this, &MWindow::exportTo);
	mfile->addSeparator();
	mfile->addAction("Enable sync", this, &MWindow::enableSync);
	mfile->addAction("Register", this, &MWindow::newRegister);
	mfile->addSeparator();
	mfile->addAction("Save", [=]() { contr->save(); }, QKeySequence::Save);
	mfile->addAction("Exit", this, &MWindow::close, QKeySequence(Qt::CTRL + Qt::Key_Q));

	QMenu *mtabs = new QMenu("Tabs");
	mtabs->addAction("Add new tab", this, &MWindow::tabNew);
	mtabs->addAction("Delete tab", this, &MWindow::tabClose);

	chAction = new QAction("To Editors");
	connect(chAction, &QAction::triggered, this, &MWindow::changeWidget);

	QMenu *mhelp = new QMenu("Help");
	mhelp->addAction("About program");

	menu->addMenu(mfile);
	menu->addMenu(mtabs);
	menu->addAction(chAction);
	menu->addMenu(mhelp);
}
// Constructor


// Tab Slots
void MWindow::tabNew() {

	if (newTab == nullptr && contr != nullptr && tabs->count() > 0) {
		newTab = contr->addNew();
		tabs->insertTab(tabs->count() - 1, newTab, "New Tab");
		tabs->setCurrentIndex(tabs->count() - 2);

	} else {
		tabs->setCurrentIndex(tabs->count() - 2);
	}
}

void MWindow::tabClose() {
	const QString &name = tabs->tabText(tabs->currentIndex());

	if (name == "New Tab" || QMessageBox::question(this, tr("Close tab ?"), name) == QMessageBox::Yes) {
		contr->tabDel(name);
		tabs->removeTab(tabs->currentIndex());
	}

	if (name == "New Tab")
		newTab = nullptr;
}

void MWindow::tabChange(int i) {
	if (lastTab != nullptr) lastTab->save();
	lastTab = getCurrentTab();

	if (i == (tabs->count() - 1)) {
		tabNew();

	} else {
		MTab *t;
		if ((t = getCurrentTab()) != nullptr)
			setWindowTitle(t->getDesc());

		if (newTab != getCurrentTab() && newTab != nullptr) {
			if (getTab(tabs->count() - 2)->getDesc() == "New Tab")
				tabs->removeTab(tabs->count() - 2);
			newTab = nullptr;
		}
	}
}
// Tab Slots


// Menu Slots
void MWindow::importFrom() {
	bool ok;
	QString s = QInputDialog::getMultiLineText(this, "Enter text", "Text: ", "", &ok);
	if (ok) getCurrentTab()->importFrom(s);
}

void MWindow::exportTo() {
	QInputDialog::getMultiLineText(this, "Exported text", "Text: ", getCurrentTab()->exportTo());
}
// Menu Slots


// Widget events
void MWindow::closeEvent(QCloseEvent *e) {
	Q_UNUSED(e)

	contr->save();
	tray->hide();
}

void MWindow::hideEvent(QHideEvent *e) {
	hide();

	Storage::getInstance()->saveJson();
	e->accept();
}

void MWindow::trayClick(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		trayToggle();
	}
}

void MWindow::trayToggle() {
	if (!isVisible()) {
		if (isMinimized()) showNormal();
		this->show();
		qApp->setActiveWindow(this);

	} else {
		this->hide();
	}
}
// Widget events


void MWindow::changeWidget() {
	bool l = chAction->text() == "To Links";
	chAction->setText(l ? "To Editors" : "To Links");


//	Unload previous controller
	if (contr != nullptr) {
		contr->save();
		delete contr;
	}

//	Construct and load new
	if (l) {
		contr = new MLinksController(this);

	} else {
		contr = new MEditorsController(this);

	}

//	Clear Tabs
	tabs->clear();
	newTab = nullptr;
	tabs->addTab(new QWidget(), "+");

	contr->load();

	tabs->setCurrentIndex(0);
}

void MWindow::updateMenu() {
	auto actions = mfile->actions();
	auto st = Storage::getInstance();

	actions[3]->setText(st->getB("sync") ? "Disable sync" : "Enable sync");
}

void MWindow::enableSync() {
	auto st = Storage::getInstance();
	SSecure *se = st->secureStorage();

	bool sync = st->getB("sync");
	st->set("sync", !sync);

	if (sync)
		se->initNewLogin();
	else
		se->clearLoginInfo();

	updateMenu();
}

void MWindow::newRegister() {
	NAuth auth;

	auth.newRegister();
}


// MTab
void MTab::updated() {
	if (timer_not_started) {
		auto *u_timer = new QTimer();

		connect(u_timer, &QTimer::timeout, this, &MTab::saveStorage);

		u_last = QDateTime::currentMSecsSinceEpoch();
		u_timer->start(400);

		timer_not_started = false;
	}

	u_time = QDateTime::currentMSecsSinceEpoch();
}

void MTab::saveStorage() {
	if (u_time >= u_last && (u_time + 2000 < QDateTime::currentMSecsSinceEpoch())) {
		u_last = QDateTime::currentMSecsSinceEpoch();
		Storage::getInstance()->saveJson();
	}
}
