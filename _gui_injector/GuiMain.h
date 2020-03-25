#pragma once

#include <QtWidgets/QMainWindow>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qtimer.h>

#include "ui_GuiMain.h"
#include "DownloadManager.h"
#include "GuiProcess.h"
#include "Process.h"

enum class UPDATE
{
	NOTHING,
	UPDATE,
	DOWNLOAD,
};

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
	GuiProcess* gui_Picker = NULL;

	// Design
	QPalette normalPalette;
	QString  normalSheet;
	QPalette darkPalette;
	QString	 darkSheet;

	// Network
	QNetworkAccessManager*	ver_Manager;
	DownloadManager			dl_Manager;
	QString					zipName;
	QString					onlineVersion;

	// Settings
	Process_State_Struct*	pss;
	Process_Struct*			ps_picker;
	//Process_Struct*		ps_main;

	QString		lastPathStr;
	bool		ignoreUpdate;
	bool		lightMode;
	UPDATE		update;

	QTimer* t_Auto_Inj;
	QTimer* t_Delay_Inj;

public slots:
	void get_from_picker(Process_State_Struct* procStateStruct, Process_Struct* procStruct);

signals:
	void send_to_picker(Process_State_Struct* procStateStruct, Process_Struct* procStruct);

private slots:
	// Titelbar
	void closeEvent(QCloseEvent* event);

	// Settings
	void rb_process_set();
	void rb_pid_set();
	void rb_unset_all();
	void cmb_proc_name_change();
	void txt_pid_change();
	void btn_pick_process_click();

	// Auto, Reset
	void auto_inject();
	void auto_loop_inject();
	void reset_settings();
	void slotReboot();
	void hook_Scan();

	// Settings, Color
	void save_settings();
	void load_settings();
	void color_setup();
	void color_change();

	// Method, Cloaking, Advanced
	void load_change(int i);
	void create_change(int i);
	void cb_main_clicked();

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

	// Update
	void check_online_version();
	void replyFinished(QNetworkReply* rep);
	void download_start();
	void download_finish();
};
