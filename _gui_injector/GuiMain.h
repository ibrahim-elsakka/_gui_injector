#pragma once

#include <QtWidgets/QMainWindow>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qtimer.h>

#include "ui_GuiMain.h"
#include "GuiProcess.h"
#include "Process.h"

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
	Process_State_Struct*	pss;
	Process_Struct*			ps_picker;
	//Process_Struct*			ps_main;

	QString		lastPathStr;
	bool		ignoreUpdate;
	//int			procType;

	QTimer* timer;
	QTimer* delayInjTimer;

public slots:
	void get_from_picker(Process_State_Struct* procStateStruct, Process_Struct* procStruct);

signals:
	void send_to_picker(Process_State_Struct* procStateStruct, Process_Struct* procStruct);

private slots:
	// Titelbar
	void closeEvent(QCloseEvent* event);

	// Settings
	void set_rb_proc();
	void set_rb_pid();
	void unset_rb();
	void pick_process();
	void procName_change();
	void procID_change();

	// Auto, Reset
	void auto_inject();
	void auto_loop_inject();
	void reset_settings();
	void slotReboot();

	// Settings, Color
	void save_settings();
	void load_settings();
	void color_setup();
	void color_change();

	// Method, Cloaking, Advanced
	void load_change(int i);
	void create_change(int i);
	void adv_change();

	// Files
	void add_file_dialog();
	void add_file_to_list(QString str);
	void remove_file();
	void select_file();
	void delay_inject();
	void inject_file();
	void injec_status(bool ok, QString msg);

	// Info
	void tooltip_change();
	void open_help();
	void open_log();
	void check_version();
	void replyFinished(QNetworkReply* rep);
};
