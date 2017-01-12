/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "kjobprogresstracker.h"

#include <KJob>
#include <KMessageBox>

#include <QProgressDialog>
#include <QVariant>

KJobProgressTracker::KJobProgressTracker(QWidget *parentWidget, QObject *parent)
    : QObject(parent)
    , mParentWidget(parentWidget)
    , mProgressDialog(nullptr)
{
}

KJobProgressTracker::~KJobProgressTracker()
{
    foreach (KJob *job, mJobs) {
        disconnect(job, SIGNAL(result(KJob*)), this, SLOT(jobFinished(KJob*)));
        job->deleteLater();
    }
}

void KJobProgressTracker::setCaption(const QString &caption)
{
    mCaption = caption;
}

void KJobProgressTracker::setLabel(const QString &label)
{
    mLabel = label;
}

void KJobProgressTracker::addJob(KJob *job, const QString &errorMessage)
{
    job->setProperty("__errorMessage", errorMessage);
    connect(job, SIGNAL(result(KJob*)), SLOT(jobFinished(KJob*)));
    mJobs.append(job);
}

void KJobProgressTracker::start()
{
    // cppcheck-suppress publicAllocationError as widget deletes itself later
    mProgressDialog = new QProgressDialog(mParentWidget);
    mProgressDialog->setWindowTitle(mCaption);
    mProgressDialog->setLabelText(mLabel);
    mProgressDialog->setRange(0, mJobs.count());
    mProgressDialog->setValue(0);
}

void KJobProgressTracker::jobFinished(KJob *job)
{
    mJobs.remove(mJobs.indexOf(job));
    mProgressDialog->setValue(mProgressDialog->value() + 1);

    if (job->error()) {
        const QString errorMessage = job->property("__errorMessage").toString();
        KMessageBox::error(mParentWidget, errorMessage.arg(job->errorString()));
    }

    if (mJobs.isEmpty()) {
        emit finished();

        mProgressDialog->deleteLater();
        mProgressDialog = nullptr;
    }
}
