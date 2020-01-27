#ifndef SRC_VITAC_WAITCONDITION_H_
#define SRC_VITAC_WAITCONDITION_H_

#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>

/**
 * This class should be used when you have some sort of boolean flag, and you want
 * to be able to wait for it to be set. This class can be used if you need to wait
 * on the condition -- using either of the wait() calls -- or if you just want a
 * boolean field protected by a mutex lock.
 */
class WaitCondition {
protected:
    bool value = false;
	std::mutex myMutex;
	std::condition_variable condVar;

    void doFlag();

public:
	WaitCondition();
	virtual ~WaitCondition();

    bool getValue();

    bool flag();
	void reset();
    void notify();
    void wait();
    void wait(std::chrono::milliseconds maximumWaitTime, bool allowInterrupt = false);
    void waitUntil(std::chrono::system_clock::time_point untilTime, bool allowInterrupt = false);
};

/**
 * This class lets me add an int value to a WaitCondition.
 *
 * If flagCheckFunction is provided, then we flag() -- call to the parent --
 * whenever the intValue is raised from 0. Or, of course, you can call flag()
 * directly.
 *
 * If flagCheckFunction is provided, it should return true if we should call flag().
 * Be careful, as you'll be called from a mutex-locked method.
 */
class WaitCondition_Int: public WaitCondition {
public:
    typedef std::function<bool(int)> FlagCheckFunction;

protected:
    int                 intValue = 0;
    FlagCheckFunction   flagCheckFunction;

public:
    WaitCondition_Int();
    WaitCondition_Int(FlagCheckFunction function);

    int increment();
    int incrementBy(int byAmount);

    int decrement();
    int decrementBy(int byAmount);

    int getIntValue();
};

/**
 * This wait condition basically lets us wait on a timeout or our flag is set.
 * This would be a trivial use of the base class, except we let them change the
 * time point we're waiting on, and that complicates the wait loop slightly.
 * Just so we could test both flags under a mutex lock, I made a specialized version.
 *
 * To use us:
 *
 * 		WaitCondition_Time waitCondition;
 *
 * 		std::chrono::system_clock::time_point tp = std::chrono::system_clock::now() + std::chrono::minutes(2);
 * 		waitCondition.setTimePoint(tp);
 *
 * 		waitCondition.waitUntilTimePoint();
 *
 * You can call setTimePoint() from other threads, and it will adjust what happens with anyone
 * currently waiting. You can also set the flag normally.
 */
class WaitCondition_Time: public WaitCondition {
protected:
    std::chrono::system_clock::time_point timePoint;

    bool checkConditions();

public:
    WaitCondition_Time();
    WaitCondition_Time(std::chrono::system_clock::time_point);

    void setTimePoint(std::chrono::system_clock::time_point);

    void waitUntilTimePoint();
    bool timeExceeded();
};

#endif /* SRC_VITAC_WAITCONDITION_H_ */
