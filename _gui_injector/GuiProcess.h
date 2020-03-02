#pragma once

#include <QWidget>
#include "ui_GuiProcess.h"

struct procState
{
	int		pid;
	QString procName;
	int		type;
	QString filter;
	int		cmbType;
	bool	cbSession;
};

class GuiProcess : public QWidget
{
	Q_OBJECT

public:
	GuiProcess(QWidget* parent = Q_NULLPTR);
	~GuiProcess();

private:
	Ui::frm_proc ui;

signals:
	void send_to_inj(procState state);

public slots:
	void get_from_inj(procState state);

private slots:
	void refresh_process();
	void filter_change(int i);
	void nameChanged(const QString&);
	void proc_select();
};
