/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KJOBPROGRESSTRACKER_H
#define KJOBPROGRESSTRACKER_H

#include <QObject>
#include <QVector>

class KJob;
class QProgressDialog;
class QWidget;

class KJobProgressTracker : public QObject
{
    Q_OBJECT

public:
    explicit KJobProgressTracker(QWidget *parentWidget, QObject *parent = nullptr);
    ~KJobProgressTracker() override;

    void setCaption(const QString &caption);
    void setLabel(const QString &label);

    void addJob(KJob *job, const QString &errorMessage);

    void start();

signals:
    void finished();

private slots:
    void jobFinished(KJob *job);

private:
    QWidget *mParentWidget;
    QString mCaption;
    QString mLabel;

    QVector<KJob*> mJobs;
    QProgressDialog *mProgressDialog;
};

#endif
