#ifndef KWAITFORSIGNAL_H
#define KWAITFORSIGNAL_H


class QObject;

    /**
     * Starts an event loop that runs until the given signal is received.
     * Optionally the event loop can return earlier on a timeout.
     *
     * \param timeout the timeout in milliseconds
     *
     * \return \p true if the requested signal was received
     *         \p false on timeout
     *
     * Qt5: use QSignalSpy::wait() instead
     */
    bool kWaitForSignal(QObject *obj, const char *signal, int timeout = 0);

#endif // KWAITFORSIGNAL_H
