/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

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

#ifndef SUGARJOB_H
#define SUGARJOB_H

#include <KJob>

class KDSoapMessage;
class SugarSession;
namespace KDSoapGenerated
{
class Sugarsoap;
class TNS__Set_entry_result;
class TNS__Error_value;
}

class SugarJob : public KJob
{
    Q_OBJECT

public:
    enum Errors {
        LoginError = KJob::UserDefinedError + 1,
        CouldNotConnectError,
        SoapError,
        InvalidContextError,
        TaskError
    };

    explicit SugarJob(SugarSession *session, QObject *parent = 0);

    ~SugarJob() override;

    void start();

public Q_SLOTS:
    void restart();

protected:
    virtual bool doKill();
    virtual void startSugarTask() = 0;

    bool handleError(const KDSoapGenerated::TNS__Error_value &errorValue);
    bool handleLoginError(const KDSoapMessage &fault);

    QString sessionId() const;
    KDSoapGenerated::Sugarsoap *soap();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void startLogin())
    Q_PRIVATE_SLOT(d, void startTask())
    Q_PRIVATE_SLOT(d, void slotPasswordAvailable())
};

#endif
