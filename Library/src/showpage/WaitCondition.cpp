/*
 * WaitCondition.cpp
 *
 *  Created on: Feb 12, 2019
 *      Author: jpl
 */

#include "WaitCondition.h"

using namespace std;
using namespace std::chrono;

/**
 * Constructor.
 */
WaitCondition::WaitCondition()
{
}

/**
 * Destructor.
 */
WaitCondition::~WaitCondition() {
}

/**
 * Safely retrieve the value of the flag.
 */
bool
WaitCondition::getValue() {
    unique_lock<std::mutex> lock(myMutex);
    return value;
}

/**
 * Flag us as triggered.
 * @return The previous value.
 */
bool
WaitCondition::flag() {
	unique_lock<std::mutex> lock(myMutex);
    bool retVal =  value;
    doFlag();
    return retVal;
}

/**
 * This is a non-protected flag method that can be called from any of the
 * public methods that are already mutex-locked.
 */
void
WaitCondition::doFlag() {
    value = true;
    condVar.notify_all();
}

/**
 * Reset us to unflagged.
 */
void
WaitCondition::reset() {
	unique_lock<std::mutex> lock(myMutex);
	value = false;
	condVar.notify_all();
}

/**
 * This simply wakes up anyone waiting in one of our timed waits.
 * This is used when some outside condition value has changed, and
 * we want threads to recheck.
 */
void
WaitCondition::notify() {
    unique_lock<std::mutex> lock(myMutex);
    condVar.notify_all();
}

/**
 * Wait forever for our flag to be triggered.
 */
void
WaitCondition::wait() {
    unique_lock<std::mutex> lock(myMutex);
    while (!value) {
		condVar.wait(lock);
	}
}

/**
 * Wait a maximum period of time for our flag to be triggered.
 * If allowInterrupt is set, then we can return early even if we're not yet flagged.
 */
void
WaitCondition::wait(std::chrono::milliseconds maximumWaitTime, bool allowInterupt) {
    time_point<system_clock> untilTime = system_clock::now() + maximumWaitTime;
    waitUntil(untilTime, allowInterupt);
}

/**
 * Wait a maximum period of time for our flag to be triggered.
 * If allowInterrupt is set, then we can return early even if we're not yet flagged.
 */
void
WaitCondition::waitUntil(std::chrono::system_clock::time_point untilTime, bool allowInterrupt) {
    time_point<system_clock> now = system_clock::now();
    unique_lock<std::mutex> lock(myMutex);

    while (!value && now < untilTime) {
        milliseconds waitDuration = duration_cast<milliseconds>(untilTime - now);
        condVar.wait_for(lock, waitDuration);
        if (allowInterrupt) {
            break;
        }

        now = system_clock::now();
    }
}

//======================================================================
// The int version.
//======================================================================

/**
 * Constructor.
 */
WaitCondition_Int::WaitCondition_Int() {
}


/**
 * Constructor.
 */
WaitCondition_Int::WaitCondition_Int(WaitCondition_Int::FlagCheckFunction function)
    : flagCheckFunction(function)
{
}

/**
 * Increments by one and returns the old value. Will flag if the flag condition is set.
 */
int
WaitCondition_Int::increment() {
    return incrementBy(1);
}

/**
 * Increments by the indicated value and returns the old value. Will flag if the flag condition is set.
 */
int
WaitCondition_Int::incrementBy(int byAmount) {
    unique_lock<std::mutex> lock(myMutex);

    int retVal = intValue;
    intValue += byAmount;

    if ( flagCheckFunction ? flagCheckFunction(intValue) : (intValue > 0) ) {
        doFlag();
    }

    return retVal;
}

/**
 * Decrements by one and returns the old value. Will flag if the flag condition is set.
 */
int
WaitCondition_Int::decrement() {
    return decrementBy(1);
}

/**
 * Decrements by the indicated value and returns the old value. Will flag if the flag condition is set.
 */
int
WaitCondition_Int::decrementBy(int byAmount) {
    unique_lock<std::mutex> lock(myMutex);

    int retVal = intValue;
    intValue -= byAmount;

    if ( flagCheckFunction ? flagCheckFunction(intValue) : (intValue > 0) ) {
        doFlag();
    }

    return retVal;
}

/**
 * Return the current value. Thread-safe.
 */
int
WaitCondition_Int::getIntValue() {
    unique_lock<std::mutex> lock(myMutex);

    return intValue;
}

//======================================================================
// The time_point version.
//======================================================================

WaitCondition_Time::WaitCondition_Time() {
}

WaitCondition_Time::WaitCondition_Time(chrono::system_clock::time_point tp) {
    timePoint = tp;
}

void
WaitCondition_Time::setTimePoint(chrono::system_clock::time_point tp) {
    unique_lock<std::mutex> lock(myMutex);
    timePoint = tp;
    condVar.notify_all();
}

/**
 * Exit conditions: flag is set or current time is past the timePoint.
 * Both checked under mutex lock.
 */
void
WaitCondition_Time::waitUntilTimePoint() {
    while(!checkConditions()) {
        waitUntil(timePoint);
    }
}

/**
 * Returns true if either of our loop exit conditions are set.
 */
bool
WaitCondition_Time::checkConditions() {
    unique_lock<std::mutex> lock(myMutex);
    return value || chrono::system_clock::now() >= timePoint;
}

/**
 * Have we met the current time?
 */
bool
WaitCondition_Time::timeExceeded() {
    unique_lock<std::mutex> lock(myMutex);
    return system_clock::now() >= timePoint;
}
