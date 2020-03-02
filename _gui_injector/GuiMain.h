#pragma once

#include <QtWidgets/QMainWindow>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include "ui_GuiMain.h"
#include "GuiProcess.h"


class GuiMain : public QMainWindow
{
	Q_OBJECT

public:
	GuiMain(QWidget* parent = Q_NULLPTR);
	~GuiMain();

	static int const EXIT_CODE_REBOOT;

	static int str_to_arch(const QString str);
	static QString arch_to_str(const int arch);


private:
	Ui::GuiMainClass ui;
	GuiProcess* picker = NULL;

	// Design
	QPalette normalPalette;
	QString  normalSheet;
	QPalette darkPalette;
	QString	 darkSheet;

	// Network
	QNetworkAccessManager* manager;

	// Settings
	procState	*pState;
	QString		lastPathStr;
	bool		ignoreUpdate;
	int			procType;


public slots:
	void get_from_picker(procState state);

signals:
	void send_to_picker(procState state);

private slots:
	// Titelbar
	void closeEvent(QCloseEvent* event);

	// Settings
	void set_rb_proc();
	void set_rb_pid();
	void pick_process();
	void procName_change();

	void save_settings();
	void load_settings();
	void color_setup();
	void color_change();

	void reset_settings();
	void slotReboot();

	// Method, Cloaking, Advanced
	void load_change(int i);
	void create_change(int i);
	void adv_change();

	// Files
	void add_file_dialog();
	void add_file_to_list(QString str);
	void remove_file();
	void select_file();
	void inject_file();
	void injec_status(bool ok, QString msg);
	void auto_inject();

	// Info
	void tooltip_change();
	void open_help();
	void open_log();
	void check_version();
	void replyFinished(QNetworkReply* rep);

};
