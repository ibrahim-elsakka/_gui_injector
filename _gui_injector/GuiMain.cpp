#include "GuiMain.h"

#include <qdesktopservices.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qstylefactory.h>
#include <qsettings.h>
#include <qthread.h>

#include <string>
#include "Banner.h"
#include "Process.h"
#include "Injection.h"

int const GuiMain::EXIT_CODE_REBOOT = -123456789;

GuiMain::GuiMain(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Settings
	connect(ui.rb_proc, SIGNAL(clicked()), this, SLOT(set_rb_proc()));
	connect(ui.rb_pid, SIGNAL(clicked()), this, SLOT(set_rb_pid()));
	connect(ui.btn_proc, SIGNAL(clicked()), this, SLOT(pick_process()));
	//connect(ui.cmb_proc, SIGNAL(keyPressEvent()), this, SLOT(procName_change()));
	connect(ui.cmb_proc, SIGNAL(currentTextChanged(const QString&)), this, SLOT(procName_change()));

	connect(ui.btn_reset, SIGNAL(clicked()), this, SLOT(reset_settings()));
	connect(ui.btn_light, SIGNAL(clicked()), this, SLOT(color_change()));

	// Method, Cloaking, Advanced
	connect(ui.cmb_load, SIGNAL(currentIndexChanged(int)), this, SLOT(load_change(int)));
	connect(ui.cmb_create, SIGNAL(currentIndexChanged(int)), this, SLOT(create_change(int)));
	connect(ui.btn_adv, SIGNAL(clicked()), this, SLOT(adv_change()));

	// Files
	connect(ui.btn_add, SIGNAL(clicked()), this, SLOT(add_file_dialog()));
	connect(ui.btn_remove, SIGNAL(clicked()), this, SLOT(remove_file()));
	connect(ui.tree_files, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(select_file()));
	connect(ui.btn_inject, SIGNAL(clicked()), this, SLOT(inject_file()));

	// Info
	connect(ui.btn_tooltip, SIGNAL(clicked()), this, SLOT(tooltip_change()));
	connect(ui.btn_help, SIGNAL(clicked()), this, SLOT(open_help()));
	connect(ui.btn_log, SIGNAL(clicked()), this, SLOT(open_log()));
	connect(ui.btn_version, SIGNAL(clicked()), this, SLOT(check_version()));

	manager = new QNetworkAccessManager(this);
	connect(manager, &QNetworkAccessManager::finished, this, &GuiMain::replyFinished);

	// GuiProcess
	picker = new GuiProcess();
	connect(this, SIGNAL(send_to_picker(procState)), picker, SLOT(get_from_inj(procState)));
	connect(picker, SIGNAL(send_to_inj(procState)), this, SLOT(get_from_picker(procState)));

	

	for (int i = 0; i <= 3; i++)
		ui.tree_files->resizeColumnToContents(i);
	ui.tree_files->clear();

	pState = new procState;
	color_setup();
	load_settings();
	color_change();
	adv_change();

	// Reduze Height
	QSize winSize = this->size();
	winSize.setHeight(500);
	//winSize.setWidth(1000);
	this->resize(winSize);

	auto_inject();
}

GuiMain::~GuiMain()
{
	delete picker;
	delete manager;
	delete pState;
}

int GuiMain::str_to_arch(const QString str)
{
	if (str == "x64") return X64;
	else if (str == "x86") return X86;
	else return NONE;
}

QString GuiMain::arch_to_str(const int arch)
{
	if (arch == 1) return QString("x64");
	else if (arch == 2) return QString("x86");
	else return QString("NONE");
}


void GuiMain::closeEvent(QCloseEvent* event)
{
	save_settings();
}

void GuiMain::set_rb_proc()
{
	ui.cmb_proc->setEnabled(true);
	ui.txt_pid->setEnabled(false);
	ui.btn_proc->setEnabled(false);
	ui.rb_proc->setChecked(true);

}

void GuiMain::set_rb_pid()
{
	ui.cmb_proc->setEnabled(false);
	ui.txt_pid->setEnabled(true);
	ui.btn_proc->setEnabled(true);
	ui.rb_pid->setChecked(true);
}

void GuiMain::pick_process()
{
	picker->show();
	emit send_to_picker(*pState);
}

void GuiMain::procName_change()
{
	QString proc = ui.cmb_proc->currentText();
	process_list pl = getProcessByName(proc.toStdString().c_str());
	if (pl.pid)
	{
		procType = pl.arch;
		pState->pid = pl.pid;
		ui.txt_pid->setText(QString::number(pl.pid));
	}
}

void GuiMain::color_setup()
{
	// https://gist.github.com/QuantumCD/6245215
	// Style bullshit
	qApp->setStyle(QStyleFactory::create("Fusion"));
	normalPalette = qApp->palette();
	normalSheet = qApp->styleSheet();

	darkPalette.setColor(QPalette::Window, QColor(0x2D, 0x2D, 0x2D));
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
	darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::BrightText, Qt::red);
	darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);

	darkSheet = ("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

}

void GuiMain::color_change()
{
	if (ui.btn_light->isChecked())
	{
		qApp->setPalette(normalPalette);
		qApp->setStyleSheet(normalSheet);
		ui.btn_light->setText("Dark mode");

		QPixmap pix_banner;
		pix_banner.loadFromData(getBannerWhite(), getBannerWhiteLen(), "JPG");
		ui.lbl_img->setPixmap(pix_banner);
	}
	else
	{
		qApp->setPalette(darkPalette);
		qApp->setStyleSheet(darkSheet);
		ui.btn_light->setText("Light mode");

		QPixmap pix_banner;
		pix_banner.loadFromData(getBanner(), getBannerLen(), "JPG");
		ui.lbl_img->setPixmap(pix_banner);
	}
}

void GuiMain::reset_settings()
{
	// delete file
	QFile qf("settings.ini");
	if (qf.exists())
		qf.remove();

	emit slotReboot();
}

void GuiMain::slotReboot()
{
	//qDebug() << "Performing application reboot...";
	qApp->exit(GuiMain::EXIT_CODE_REBOOT);
}

void GuiMain::save_settings()
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	//settings.beginGroup("FILES");

	settings.beginWriteArray("FILES");
	int i = 0;
	QTreeWidgetItemIterator it(ui.tree_files);
	while (*it)
	{
		settings.setArrayIndex(i);
		settings.setValue(QString::number(i), (*it)->text(2));
		++it; i++;
	}
	settings.endArray();


	settings.beginWriteArray("PROCESS");
	for (int i = 0; i < ui.cmb_proc->count(); i++)
	{
		settings.setArrayIndex(i);
		settings.setValue(QString::number(i), ui.cmb_proc->itemText(i));
	}
	settings.endArray();

	//settings.endGroup();
	settings.beginGroup("CONFIG");
	settings.setValue("PROCESS", ui.cmb_proc->currentIndex());
	settings.setValue("PID", ui.txt_pid->text());
	settings.setValue("PROCESSBYNAME", ui.rb_proc->isChecked());
	settings.setValue("DELAY", ui.txt_delay->text());	
	settings.setValue("CLOSEONINJ", ui.cb_close->isChecked());
	settings.setValue("AUTOINJ", ui.cb_auto->isChecked());
	settings.setValue("METHOD", ui.cmb_load->currentIndex());
	settings.setValue("LAUNCHMETHOD", ui.cmb_create->currentIndex());
	settings.setValue("HIJACK", ui.cb_hijack->isChecked());
	settings.setValue("CLOAK", ui.cb_clock->isChecked());
	settings.setValue("PEH", ui.cmb_peh->currentIndex());
	settings.setValue("RANDOMIZE", ui.cb_random->isChecked());
	settings.setValue("DLLCOPY", ui.cb_copy->isChecked());
	settings.setValue("ADVANCED", ui.btn_adv->isChecked());
	settings.setValue("UNLINKPEB", ui.cb_unlink->isChecked());
	settings.setValue("SHIFTMODULE", ui.cb_shift->isChecked());
	settings.setValue("CLEANDIR", ui.cb_clean->isChecked());
	settings.setValue("TOOLTIPSON", ui.btn_tooltip->isChecked());
	settings.setValue("DARKTHEME", ui.btn_light->isChecked());

	settings.setValue("LASTDIR", lastPathStr);
	settings.setValue("IGNOREUPDATES", ignoreUpdate);
	settings.setValue("PROCNAMEFILTER", pState->filter);
	settings.setValue("CURRENTSESSION", pState->cbSession);
	settings.setValue("PROCESSTYPE", pState->cmbType);
	settings.setValue("CURRENTPROCTYPE", procType);
	settings.setValue("GEOMETRY", saveGeometry());
	settings.setValue("STATE", saveState());
	settings.endGroup();
}

void GuiMain::load_settings()
{
	pState->pid = 0;
	pState->type = 0;
	pState->cbSession = true;
	pState->cmbType = 0;
	pState->procName = "";

	QSettings settings("settings.ini", QSettings::IniFormat);

	int fileSize = settings.beginReadArray("FILES");
	for (int i = 0; i < fileSize; ++i) {
		settings.setArrayIndex(i);
		add_file_to_list(settings.value(QString::number(i)).toString());
	}
	settings.endArray();

	int procSize = settings.beginReadArray("PROCESS");
	for (int i = 0; i < procSize; ++i) {
		settings.setArrayIndex(i);
		ui.cmb_proc->addItem(settings.value(QString::number(i)).toString());
	}
	settings.endArray();

	settings.beginGroup("CONFIG");
	ui.cmb_proc->setCurrentIndex(settings.value("PROCESS").toInt());
	ui.txt_pid->setText(settings.value("PID").toString());
	ui.rb_proc->setChecked(settings.value("PROCESSBYNAME").toBool());
	ui.txt_delay->setText(settings.value("DELAY").toString());
	ui.cb_auto->setChecked(settings.value("AUTOINJ").toBool());
	ui.cb_close->setChecked(settings.value("CLOSEONINJ").toBool());
	ui.cmb_load->setCurrentIndex(settings.value("METHOD").toInt());
	ui.cmb_create->setCurrentIndex(settings.value("LAUNCHMETHOD").toInt());
	ui.cb_hijack->setChecked(settings.value("HIJACK").toBool());
	ui.cb_clock->setChecked(settings.value("CLOAK").toBool());
	ui.cmb_peh->setCurrentIndex(settings.value("PEH").toInt());
	ui.cb_random->setChecked(settings.value("RANDOMIZE").toBool());
	ui.cb_copy->setChecked(settings.value("DLLCOPY").toBool());
	ui.btn_adv->setChecked(settings.value("ADVANCED").toBool());
	ui.cb_unlink->setChecked(settings.value("UNLINKPEB").toBool());
	ui.cb_shift->setChecked(settings.value("SHIFTMODULE").toBool());
	ui.cb_clean->setChecked(settings.value("CLEANDIR").toBool());
	ui.btn_tooltip->setChecked(settings.value("TOOLTIPSON").toBool());
	ui.btn_light->setChecked(settings.value("DARKTHEME").toBool());

	lastPathStr			= settings.value("LASTDIR").toString();
	ignoreUpdate		= settings.value("IGNOREUPDATES").toBool();
	pState->filter		= settings.value("PROCNAMEFILTER").toString();
	pState->cmbType		= settings.value("PROCESSTYPE").toInt();
	pState->cbSession	= settings.value("CURRENTSESSION").toBool();
	procType			= settings.value("CURRENTPROCTYPE").toInt();
	restoreGeometry(settings.value("GEOMETRY").toByteArray());
	restoreState(settings.value("STATE").toByteArray());

	//settings.value("PROCESSTYPE", ui.txt_pid->text());
	settings.endGroup();
}

void GuiMain::load_change(int i)
{
	adv_change();

	if (i == 0)			ui.cmb_load->setToolTip("LoadLibraryExW is the default injection method which simply uses LoadLibraryExW.");
	else if (i == 1)	ui.cmb_load->setToolTip("LdrLoadDll is an advanced injection method which uses LdrLoadDll and bypasses LoadLibrary(Ex) hooks.");
	else				ui.cmb_load->setToolTip("ManualMap is an advanced injection technique which bypasses most module detection methods.");
}

void GuiMain::create_change(int i)
{
	// Change Cloak
	if (ui.cmb_create->currentIndex() == 0)
	{
		ui.cb_clock->setEnabled(true);
	}
	else
	{
		ui.cb_clock->setEnabled(false);
		ui.cb_clock->setChecked(false);
	}

	if (i == 0)			ui.cmb_load->setToolTip("NtCreateThreadEx: Creates a simple remote thread to load the dll(s).");
	else if (i == 1)	ui.cmb_load->setToolTip("Thread hijacking: Redirects a thread to a codecave to load the dll(s).");
	else if (i == 2)	ui.cmb_load->setToolTip("SetWindowsHookEx: Adds a hook into the window callback list which then loads the dll(s).");
	else				ui.cmb_load->setToolTip("QueueUserAPC: Registers an asynchronous procedure call to the process' threads which then loads the dll(s).");
}



void GuiMain::adv_change()
{
	if (ui.btn_adv->isChecked())
		ui.grp_adv->setVisible(true);
	else
		ui.grp_adv->setVisible(false);


	if (ui.cmb_load->currentIndex() < 2)
	{
		ui.cb_unlink->setEnabled(true);
		ui.cb_shift->setEnabled(false);
		ui.cb_clean->setEnabled(false);
		ui.cb_shift->setChecked(false);
		ui.cb_clean->setChecked(false);
	}
	else
	{
		ui.cb_unlink->setEnabled(false);
		ui.cb_unlink->setChecked(false);
		ui.cb_shift->setEnabled(true);
		ui.cb_clean->setEnabled(true);
	}
}



void GuiMain::tooltip_change()
{
	if (ui.btn_tooltip->isChecked())
		ui.btn_tooltip->setText("Disable tooltips");
	else
		ui.btn_tooltip->setText("Enable tooltips");

	int duration = 1;
	if (ui.btn_tooltip->isChecked())
		duration = -1;

	// Settings
	ui.lbl_proc->setToolTipDuration(duration);
	ui.rb_proc->setToolTipDuration(duration);
	ui.cmb_proc->setToolTipDuration(duration);

	ui.lbl_pid->setToolTipDuration(duration);
	ui.rb_pid->setToolTipDuration(duration);
	ui.txt_pid->setToolTipDuration(duration);
	ui.btn_proc->setToolTipDuration(duration);

	ui.lbl_delay->setToolTipDuration(duration);
	ui.txt_delay->setToolTipDuration(duration);
	ui.cb_close->setToolTipDuration(duration);
	ui.cb_auto->setToolTipDuration(duration);

	// Method
	ui.cmb_load->setToolTipDuration(duration);
	ui.cb_hijack->setToolTipDuration(duration);
	ui.cmb_create->setToolTipDuration(duration);
	ui.cb_clock->setToolTipDuration(duration);

	// Cloaking
	ui.cmb_peh->setToolTipDuration(duration);
	ui.cb_random->setToolTipDuration(duration);
	ui.btn_adv->setToolTipDuration(duration);
	ui.cmb_peh->setToolTipDuration(duration);
	ui.cb_copy->setToolTipDuration(duration);

	// Advanced
	ui.cb_unlink->setToolTipDuration(duration);
	ui.cb_shift->setToolTipDuration(duration);
	ui.cb_clean->setToolTipDuration(duration);

	ui.btn_reset->setToolTipDuration(duration);
	ui.btn_light->setToolTipDuration(duration);

	// Files
	ui.btn_add->setToolTipDuration(duration);
	ui.btn_inject->setToolTipDuration(duration);
	ui.btn_remove->setToolTipDuration(duration);

	// Info
	ui.btn_tooltip->setToolTipDuration(duration);
	ui.btn_help->setToolTipDuration(duration);
	ui.btn_log->setToolTipDuration(duration);
	ui.btn_version->setToolTipDuration(duration);
}

void GuiMain::open_help()
{
	bool ok = QDesktopServices::openUrl(QUrl("https://guidedhacking.com/resources/guided-hacking-dll-injector.4/", QUrl::TolerantMode));
}

void GuiMain::open_log()
{
	bool ok = QDesktopServices::openUrl(QUrl("https://pastebin.com/eN7KPX3x", QUrl::TolerantMode));
}

void GuiMain::check_version()
{
	ui.btn_version->setText("loading...");
	ui.btn_version->setEnabled(false);

	manager->get(QNetworkRequest(QUrl("https://guidedhacking.com/gh/inj/")));

	//QUrl url_ver("http://guidedhacking.com/gh/inj");
	//QString ver = url_ver.query();
	
	//QUrl url_download("http://guidedhacking.com/gh/inj/V" + ver + "/GH Injector.zip");
}

void GuiMain::replyFinished(QNetworkReply* reply)
{
	QByteArray bytes = reply->readAll();
	QString str = QString::fromUtf8(bytes.data(), bytes.size());
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 200)
	{
		ui.btn_version->setText("Version " + str);
	}
	else
	{
		ui.btn_version->setText("failed");
	}
	
}

void GuiMain::add_file_dialog()
{
	QFileDialog fileDialog(this, "Select dll files");
	fileDialog.setNameFilter("Dynamic Link Libraries (*.dll)");
	fileDialog.setWindowFilePath(lastPathStr);
	fileDialog.exec();
	lastPathStr = fileDialog.windowFilePath();
	QStringList list(fileDialog.selectedFiles());

	for (auto l : list)
		GuiMain::add_file_to_list(l);
}

void GuiMain::add_file_to_list(QString str)
{
	QFileInfo fi(str);
	QTreeWidgetItem* item = new QTreeWidgetItem(ui.tree_files);

	item->setText(1, fi.fileName());
	item->setText(2, fi.absoluteFilePath());
	int arch = getFileArch(fi.absoluteFilePath().toStdString().c_str());
	item->setText(3, arch_to_str(arch));
}

void GuiMain::remove_file()
{
	QTreeWidgetItem* item = ui.tree_files->currentItem();
	delete item;

}


void GuiMain::select_file()
{
	QTreeWidgetItem* it2 = ui.tree_files->currentItem();

	QTreeWidgetItemIterator it(ui.tree_files);
	while (*it)
	{
		(*it)->setText(0, "");
		++it;
	}

	it2->setText(0, "YES");
}

void GuiMain::get_from_picker(procState state)
{
	if (state.pid)
	{
		ui.cmb_proc->addItem(state.procName);
		ui.txt_pid->setText(QString::number(state.pid));
		procType = state.type;
		set_rb_pid();
	}
	*pState = state;
}

void GuiMain::inject_file()
{
	INJECTIONDATAA data;
	memset(&data, 0, sizeof(INJECTIONDATAA));

	int fileType	= NONE;
	int processType = NONE;


	for (QTreeWidgetItemIterator it(ui.tree_files); (*it) != nullptr; ++it)
	{
		// Find Item
		if ((*it)->text(0) != "YES")
			continue;

		// Convert String
		QString fileStr	= (*it)->text(2);
		for (int i = 0, j = 0; fileStr[i].toLatin1() != '\0'; i++, j++)
		{
			if (fileStr[i] == '\/')
				data.szDllPath[j] = '\\';
			else
				data.szDllPath[j] = fileStr[i].toLatin1();		
		}
		
		// Check Existens
		QFile qf(fileStr);
		if (!qf.exists())
		{
			emit injec_status(false, "File not found");
			return;
		}

		// Check Architecture
		fileType = str_to_arch((*it)->text(3));
		if (fileType == NONE)
		{
			emit injec_status(false, "File Architecture invalid");
			return;
		}
	}

	// Check File Selected
	if (fileType == NONE)
	{
		emit injec_status(false, "File not selected");
		return;
	}

	// Process ID
	if (ui.rb_pid->isChecked())
	{
		int id = ui.txt_pid->text().toInt();
		if (id)
		{
			data.ProcessID = id;
			processType = getProcArch(id);
		}
		else
		{
			emit injec_status(false, "Invalid PID");
			return;
		}
	}
	else // Process Name
	{
		
		int index = ui.cmb_proc->currentIndex();
		process_list p = getProcessByName(ui.cmb_proc->itemText(index).toStdString().c_str());
		if (p.pid)
		{
			data.ProcessID = p.pid;
			processType = p.arch;		
		}
		else
		{
			emit injec_status(false, "Invalid Process Name");
			return;
		}
	}

	if (processType != fileType || processType == NULL || fileType == NULL)
	{
		emit injec_status(false, "File and Process are incompatible");
		return;
	}

	data.Mode	= INJECTION_MODE::IM_LoadLibrary;
	data.Method = LAUNCH_METHOD::LM_NtCreateThreadEx;

	if (ui.cmb_peh->currentIndex() == 1)	data.Flags |= INJ_ERASE_HEADER;
	if (ui.cmb_peh->currentIndex() == 2)	data.Flags |= INJ_FAKE_HEADER;
	if (ui.cb_unlink->isChecked())			data.Flags |= INJ_UNLINK_FROM_PEB;
	if (ui.cb_shift->isChecked())			data.Flags |= INJ_SHIFT_MODULE;
	if (ui.cb_clean->isChecked())			data.Flags |= INJ_CLEAN_DATA_DIR;
	if (ui.cb_clock->isChecked())			data.Flags |= INJ_THREAD_CREATE_CLOAKED;
	if (ui.cb_random->isChecked())			data.Flags |= INJ_SCRAMBLE_DLL_NAME;
	if (ui.cb_copy->isChecked())			data.Flags |= INJ_LOAD_DLL_COPY;
	if (ui.cb_hijack->isChecked())			data.Flags |= INJ_HIJACK_HANDLE;


	int delay = ui.txt_delay->text().toInt();
	if (delay > 0)
	{
		QThread::sleep(10);
	}
	
	//HINSTANCE	hinstLib = LoadLibraryA("C:\\Users\\kage\\Downloads\\GuidedHacking-Injector-master\\GuidedHacking-Injector-master\\GH Injector Library\\Release\\x64\\GH Injector - x64.dll");

	HINSTANCE hinstLib = LoadLibraryA("GH Injector - x64.dll");
	if (hinstLib == NULL)
	{
		emit injec_status(false, "GH Injector - x64.dll not found");
		return;
	}

	InjectA	injectFunc = (InjectA)GetProcAddress(hinstLib, "InjectA");
	if (injectFunc == NULL)
	{
		BOOL fFreeResult = FreeLibrary(hinstLib);
		emit injec_status(false, "InjectA not found");
		return;
	}

	DWORD res = injectFunc(&data);
	if (res)
	{
		BOOL fFreeResult = FreeLibrary(hinstLib);
		QString errorCode("\nLast Errorcode" + QString::number(data.LastErrorCode));
		emit injec_status(false, "InjectA failed with " + errorCode);
		return;
	}

	if (ui.cb_close->isChecked())
	{
		qApp->exit(0);
		return;
	}

	emit injec_status(true, "Sucess Injection");
	return;
}

void GuiMain::injec_status(bool ok, QString msg)
{
	if(ok)
	{
		QMessageBox messageBox;
		messageBox.information(0, "Success", msg);
		messageBox.setFixedSize(500, 200);

		if (ui.cb_close->isChecked())
		{
			this->close();
		}
	}
	else
	{
		QMessageBox messageBox;
		messageBox.critical(0, "Error", msg);
		messageBox.setFixedSize(500, 200);
	}
}

void GuiMain::auto_inject()
{
	if (ui.cb_auto->isChecked())
	{
		inject_file();
	}
}
