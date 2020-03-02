#include "GuiProcess.h"
#include "Process.h"
#include "GuiMain.h"

GuiProcess::GuiProcess(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.btn_refresh, SIGNAL(clicked()), this, SLOT(refresh_process()));
	connect(ui.cmb_arch, SIGNAL(currentIndexChanged(int)), this, SLOT(filter_change(int)));
	connect(ui.txt_filter, &QLineEdit::textChanged, this, &GuiProcess::nameChanged);
	connect(ui.btn_select, SIGNAL(clicked()), this, SLOT(proc_select()));

	for (int i = 0; i <= 3; i++)
		ui.tree_process->resizeColumnToContents(i);
}

GuiProcess::~GuiProcess()
{
}


void GuiProcess::refresh_process()
{
	std::vector<process_list> all_proc;
	getProcessList(all_proc);

	ui.tree_process->clear();

	for (auto proc : all_proc)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(ui.tree_process);

		item->setText(1, QString::number(proc.pid));
		item->setText(2, proc.name);
		item->setText(3, GuiMain::arch_to_str(proc.arch));
	}

	nameChanged("");
}

void GuiProcess::filter_change(int i)
{
	int j = ui.cmb_arch->currentIndex();

	QTreeWidgetItemIterator it(ui.tree_process);
	while (*it)
	{
		if (j == NONE)
		{
			(*it)->setHidden(false);
		}
		else
		{
			if (GuiMain::str_to_arch((*it)->text(3)) == QString::number(j))
			{
				(*it)->setHidden(false);
			}
			else
			{
				(*it)->setHidden(true);
			}
		}

		++it;
	}
}

void GuiProcess::nameChanged(const QString& str)
{
	filter_change(0);
	QString txt = ui.txt_filter->text();

	QTreeWidgetItemIterator it(ui.tree_process);
	while (*it)
	{
		if (!(*it)->isHidden())
		{
			bool contain = (*it)->text(2).contains(txt, Qt::CaseInsensitive);
			if (!contain)
			{
				(*it)->setHidden(true);
			}
		}
		++it;
	}

	//for (int i = 0; i <= 3; i++)
	//	ui.tree_process->resizeColumnToContents(i);
}

void GuiProcess::proc_select()
{
	procState state;
	QTreeWidgetItem* item = ui.tree_process->currentItem();

	state.pid		= 0;
	state.filter	= ui.txt_filter->text();
	state.cmbType	= ui.cmb_arch->currentIndex();
	state.cbSession = ui.cb_session->isChecked();
	if (item)
	{
		state.pid		= item->text(1).toInt();
		state.procName	= item->text(2);
		state.type		= GuiMain::str_to_arch(item->text(3));
	}
	
	emit send_to_inj(state);
	this->hide();
}


void GuiProcess::get_from_inj(procState state)
{
	ui.txt_filter->setText(state.filter);
	ui.cb_session->setChecked(state.cbSession);
	ui.cmb_arch->setCurrentIndex(state.cmbType);

	refresh_process();
}