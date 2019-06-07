#include "elapsedtimer.h"
#include <QDebug>

FACGUI::ElapsedTimer::ElapsedTimer() : QElapsedTimer () {
    running = false;
    msecs = 0;
    nsecs = 0;
}

void FACGUI::ElapsedTimer::start() {
    QElapsedTimer::restart();
    running = true;
}

void FACGUI::ElapsedTimer::stop() {
    msecs += QElapsedTimer::elapsed();
    nsecs += QElapsedTimer::nsecsElapsed();
    running = false;
}

void FACGUI::ElapsedTimer::advanceBy(qint64 msecs) {
    this->msecs += msecs;
    this->nsecs += 1e6*msecs;
}

void FACGUI::ElapsedTimer::revertBy(qint64 msecs) {
    this->msecs -= msecs;
    this->nsecs -= 1e6*msecs;
}

qint64 FACGUI::ElapsedTimer::elapsed() {
    if(running) {
        msecs += QElapsedTimer::elapsed();
        this->start();
    }
    return msecs;
}

qint64 FACGUI::ElapsedTimer::nsecsElapsed() {
    if(running) {
        nsecs += QElapsedTimer::nsecsElapsed();
        this->start();
    }
    return nsecs;
}

qint64 FACGUI::ElapsedTimer::restart() {
    qint64 time = this->elapsed();
    msecs = qint64(0);
    nsecs = qint64(0);
    QElapsedTimer::restart();
    return time;
}
