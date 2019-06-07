#ifndef ELAPSEDTIMER_H
#define ELAPSEDTIMER_H

#include <QElapsedTimer>

namespace FACGUI {
    class ElapsedTimer : public QElapsedTimer
    {
    public:
        ElapsedTimer();
        void start();
        void stop();
        void advanceBy(qint64 msecs);
        void revertBy(qint64 msecs);
        qint64 restart();
        qint64 elapsed();
        qint64 nsecsElapsed();

    private:
        qint64 msecs;
        qint64 nsecs;
        bool running;
    };
}

#endif // ELAPSEDTIMER_H
