#include <stdio.h>
#include <iostream>
#include <thread>
#include <algorithm>

#include "WorkQueue.h"

using namespace std;
using namespace std::chrono;

/**
 * Constructor.
 */
WorkQueue_Base::WorkQueue_Base()
    : head(nullptr),
      tail(nullptr),
	  running(true),
      endIterator(this, true, nullptr),
      endReverseIterator(this, false, nullptr)
{
}

/**
 * Destructor.
 */
WorkQueue_Base::~WorkQueue_Base()
{
}

/**
 * A convenience method for setting the fired at.
 */
void
WorkQueue_Base::baseAdd(WorkQueue_Entry::Ptr entry, const long millisecondDelay) {
	entry->fireAt = system_clock::now() + milliseconds(millisecondDelay);
    baseAdd(entry);
}

/**
 * Add to our queue.
 */
void
WorkQueue_Base::baseAdd(WorkQueue_Entry::Ptr entry)
{
    std::unique_lock<std::mutex> mlock(mutex);
    _add(entry);
}

/**
 * Add to the queue only if the queue is empty.
 */
void
WorkQueue_Base::baseAddIfEmpty(WorkQueue_Entry::Ptr entry) {
    std::unique_lock<std::mutex> mlock(mutex);
    if (head != nullptr) {
        _add(entry);
    }
}

/**
 * Add to the queue only if the queue is empty.
 */
void
WorkQueue_Base::baseAddIfEmpty(WorkQueue_Entry::Ptr entry, const long millisecondDelay) {
    std::unique_lock<std::mutex> mlock(mutex);
    if (head != nullptr) {
        entry->fireAt = system_clock::now() + milliseconds(millisecondDelay);
        _add(entry);
    }
}

/**
 * This is the worker method for the add methods. It is assumed we're
 * already mutex-locked.
 */
void
WorkQueue_Base::_add(WorkQueue_Entry::Ptr entry) {
	LinkedList * ll = new LinkedList(entry);

	// Insertions are PROBABLY going on the tail, so begin searching there.
	// When we're done, we insert AFTER the searcher.
	LinkedList * searcher = tail;
    while ( (searcher != nullptr) && (searcher->entry->fireAt > entry->fireAt) ) {
		searcher = searcher->previous;
	}

    if (searcher == nullptr) {
		// Insert at head.
		ll->next = head;
		head = ll;
        if (tail == nullptr) {
			tail = ll;
		}
	}
	else {
		// Goes after searcher.
		ll->next = searcher->next;
		ll->previous = searcher;
		searcher->next = ll;
        if (ll->next == nullptr) {
			// Inserted at the end, so set tail.
			tail = ll;
		}
		else {
			ll->next->previous = ll;
		}
	}

	condVar.notify_all();
}

/**
 * Add to the very front of the queue.
 */
void
WorkQueue_Base::baseAddFront(WorkQueue_Entry::Ptr entry) {
	std::unique_lock<std::mutex> mlock(mutex);
	LinkedList * ll = new LinkedList(entry);

	ll->next = head;
    if (head != nullptr) {
		head->previous = ll;
	}
	head = ll;
    if (tail == nullptr) {
		tail = ll;
	}

	condVar.notify_all();
}

/**
 * Remove anything matching this lambda. This is DESTRUCTIVE.
 */
void
WorkQueue_Base::remove(ComparisonFunct funct) {
    std::unique_lock<std::mutex> mlock(mutex);

    for (Iterator it = begin(); it != end(); ) {
        if (funct(*it)) {
            _remove(it);
        }
        else {
            ++it;
        }
    }
}

/**
 * Remove the object at this iterator. This is DESTRUCTIVE.
 * Don't use it if you're holding onto the contents at the iterator.
 */
void
WorkQueue_Base::remove(IteratorBase &it) {
    std::unique_lock<std::mutex> mlock(mutex);
    _remove(it);
}

/**
 * This one assumes we're already locked, so it's private and called by one of the public remove() methods.
 * This is DESTRUCTIVE.
 */
void
WorkQueue_Base::_remove(IteratorBase &it) {
    // Save a pointer to the object we're going to free.
    LinkedList * ptr = it.ptr;

    // Move the iterator to the next item automatically.
    it.increment();

    // Clean up pointers.
    if (ptr->next == nullptr) {
        tail = ptr->previous;
    }
    else {
        ptr->next->previous = ptr->previous;
    }
    if (ptr->previous == nullptr) {
        head = ptr->next;
    }
    else {
        ptr->previous->next = ptr->next;
    }

    // Delete this one.
    delete ptr;
}

/**
 * Signal we should shut down.
 */
void
WorkQueue_Base::shutdown() {
	std::unique_lock<std::mutex> mlock(mutex);
	running = false;
	condVar.notify_all();
}

/**
 * Signal a reset in running (turn it back on).
 */
void
WorkQueue_Base::reset() {
    std::unique_lock<std::mutex> mlock(mutex);
    running = true;
    condVar.notify_all();
}

/**
 * Get the next one, blocking until either it's ready or we're shut down.
 */
WorkQueue_Entry::Ptr
WorkQueue_Base::baseGetMoreWork() {
    WorkQueue_Entry::Ptr retVal ( nullptr );
	milliseconds longTime(3600000);	// one hour

    while (running && retVal == nullptr) {
		// We can use the version with a duration because we'll loop if he returns null.
        retVal = baseGetMoreWork(longTime);
	}
	return retVal;
}

/**
 * Get the first entry in the queue, blocking until something appears or we've expired our duration.
 * Note that entries in the queue can have a fire-at time, so there may be things in the queue that
 * we're not returning yet.
 */
WorkQueue_Entry::Ptr
WorkQueue_Base::baseGetMoreWork(std::chrono::milliseconds duration) {
	system_clock::time_point waitUntil = system_clock::now() + duration;

	while (running) {
		std::unique_lock<std::mutex> mlock(mutex);
		system_clock::time_point now = system_clock::now();

		// If nothing in the queue, we'll wait until our timer expires or something shows up.
        if (head == nullptr) {
			if (now > waitUntil) {
				// Time expired.
                return nullptr;
			}
			condVar.wait_until(mlock, waitUntil);
		}

		// Something is in the queue, but is it ready to fire?
		else if (head->entry->fireAt <= now) {
			// Fire now.
			LinkedList * oldHead = head;
            WorkQueue_Entry::Ptr retVal = head->entry;

			head = head->next;
            if (head == nullptr) {
                tail = nullptr;
			}
            else {
                head->previous = nullptr;
            }

			// We delete our linked list entry but the caller is responsible for the work queue entry.
            oldHead->entry = nullptr;	// So we don't destroy it in the destructor for LL.
			delete oldHead;

			return retVal;
		}

		// There's something in the queue, but it's not ready yet, so we have to wait.
		else {
			system_clock::time_point thisWait = head->entry->fireAt < waitUntil ? head->entry->fireAt : waitUntil;
			condVar.wait_until(mlock, thisWait);
		}
	}

    return nullptr;
}


/**
 * This does a simple list.
 */
void
WorkQueue_Base::listQueue() {
    for (LinkedList *ptr = head; ptr != nullptr; ptr = ptr->next) {
        WorkQueue_Entry::Ptr entry = ptr->entry;
		cout << *entry << endl;
	}
}

/**
 * List the queue but in reverse.
 */
void
WorkQueue_Base::listQueueReverse() {
    for (LinkedList *ptr = tail; ptr != nullptr; ptr = ptr->previous) {
        WorkQueue_Entry::Ptr entry = ptr->entry;
		cout << *entry << endl;
	}
}

/**
 * Start of the list.
 */
WorkQueue_Base::Iterator
WorkQueue_Base::begin() {
    return WorkQueue_Base::Iterator(this, head);
}

/**
 * Tail of the list.
 */
WorkQueue_Base::Reverse_Iterator
WorkQueue_Base::rbegin() {
    return WorkQueue_Base::Reverse_Iterator(this, tail);
}

/**
 * Start of the list.
 */
WorkQueue_Base::Const_Iterator
WorkQueue_Base::cbegin() const {
    return WorkQueue_Base::Const_Iterator(this, head);
}

/**
 * Tail of the list.
 */
WorkQueue_Base::Const_Reverse_Iterator
WorkQueue_Base::crbegin() const {
    return WorkQueue_Base::Const_Reverse_Iterator(this, head);
}

//======================================================================
// LinkedList.
//======================================================================

/**
 * Constructor.
 */
WorkQueue_Base::LinkedList::LinkedList(WorkQueue_Entry::Ptr ptr)
    : entry(ptr), previous(nullptr), next(nullptr)
{
}

/**
 * Destructor.
 */
WorkQueue_Base::LinkedList::~LinkedList() {
    entry.reset();
}

/**
 * Write ourself out to this stream.
 */
void
WorkQueue_Entry::print(ostream &os) const {
	system_clock::time_point now = system_clock::now();

	if (fireAt + hours(24) < now) {
		os << "Fire At: now.";
	}
	else {
		time_t t = system_clock::to_time_t(fireAt);

	    string str = string(std::ctime(&t));
	    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

		os << "Fire At: " << str << ".";
	}
}

/** Write ourself out. */
std::ostream &
operator<<(std::ostream &os, const WorkQueue_Entry & p) {
	p.print(os);
	return os;
}


//======================================================================
// Iterators.
//======================================================================

/**
 * Construct the base.
 */
WorkQueue_Base::IteratorBase::IteratorBase(const WorkQueue_Base *_queue, bool _forward, LinkedList *pos)
	: queue(_queue),
	  forward(_forward),
	  ptr(pos)
{
}

/**
 * Copy constructor.
 */
WorkQueue_Base::IteratorBase::IteratorBase(const IteratorBase &orig)
	: queue(orig.queue),
	  forward(orig.forward),
	  ptr(orig.ptr)
{
}

/**
 * Destructor. We don't own any of our data, so nothing to do.
 */
WorkQueue_Base::IteratorBase::~IteratorBase() {
}

/**
 * Copy operator.
 */
WorkQueue_Base::IteratorBase &
WorkQueue_Base::IteratorBase::operator=(const WorkQueue_Base::IteratorBase &orig) {
	queue = orig.queue;
	ptr = orig.ptr;
	forward = orig.forward;

	return *this;
}

/**
 * Dereference.
 */
WorkQueue_Entry::Ptr
WorkQueue_Base::IteratorBase::operator*() {
    return ptr->entry;
}

/**
 * Dereference.
 */
WorkQueue_Entry::Ptr
WorkQueue_Base::IteratorBase::operator->() {
	return ptr->entry;
}

/**
 * Increment (which is based of the direction of travel).
 */
void
WorkQueue_Base::IteratorBase::increment() {
	// ptr becomes null when we've incremented to the end.
	// Incrementing past that does nothing.
    if (ptr != nullptr) {
		if (forward) {
			ptr = ptr->next;
		}
		else {
			ptr = ptr->previous;
		}
	}
}

/**
 * Decrement (which is based off the direction of travel).
 */
void
WorkQueue_Base::IteratorBase::decrement() {
	// ptr can be NULL for an empty list or if we've incremented
	// past the end, but we allow backing up.
    if (ptr == nullptr) {
		if (forward) {
			ptr = queue->tail;
		}
		else {
			ptr = queue->head;
		}
	}

	else {
		// This is exactly the reverse from increment().
		if (forward) {
			ptr = ptr->previous;
		}
		else {
			ptr = ptr->next;
		}
	}
}

/**
 * Are these identical.
 */
bool
WorkQueue_Base::IteratorBase::operator==(const WorkQueue_Base::IteratorBase &orig) const {
	return queue == orig.queue && ptr == orig.ptr && forward == orig.forward;
}

/**
 * Are these different?
 */
bool
WorkQueue_Base::IteratorBase::operator!=(const WorkQueue_Base::IteratorBase &orig) const {
	return queue != orig.queue || ptr != orig.ptr || forward != orig.forward;
}

//----------------------------------------------------------------------
// The forward iterator.
//----------------------------------------------------------------------

/**
 * Constructor the simple, forward iterator.
 */
WorkQueue_Base::Iterator::Iterator(WorkQueue_Base *_queue, LinkedList *pos)
    : WorkQueue_Base::IteratorBase::IteratorBase(_queue, true, pos)
{
}

/**
 * Copy operator.
 */
WorkQueue_Base::Iterator &
WorkQueue_Base::Iterator::operator=(const WorkQueue_Base::Iterator &orig) {
    WorkQueue_Base::IteratorBase::operator=(orig);
	return *this;
}

/**
 * Increment.
 */
WorkQueue_Base::Iterator &
WorkQueue_Base::Iterator::operator++() {
	increment();
	return *this;
}

/**
 * This is a post-increment.
 */
WorkQueue_Base::Iterator
WorkQueue_Base::Iterator::operator++(int) {
    WorkQueue_Base::Iterator tmp(*this);
	increment();
	return tmp;
}

/**
 * Decrement.
 */
WorkQueue_Base::Iterator &
WorkQueue_Base::Iterator::operator--() {
	increment();
	return *this;
}

/**
 * This is a post-decrement.
 */
WorkQueue_Base::Iterator
WorkQueue_Base::Iterator::operator--(int) {
    WorkQueue_Base::Iterator tmp(*this);
	decrement();
	return tmp;
}


//----------------------------------------------------------------------
// The reverse iterator.
//----------------------------------------------------------------------

/**
 * Construct the reverse iterator.
 */
WorkQueue_Base::Reverse_Iterator::Reverse_Iterator(WorkQueue_Base *_queue, LinkedList *pos)
    : WorkQueue_Base::IteratorBase::IteratorBase(_queue, false, pos)
{
}

/**
 * Copy operator.
 */
WorkQueue_Base::Reverse_Iterator &
WorkQueue_Base::Reverse_Iterator::operator=(const WorkQueue_Base::Reverse_Iterator &orig) {
    WorkQueue_Base::IteratorBase::operator=(orig);
	return *this;
}

/**
 * Increment.
 */
WorkQueue_Base::Reverse_Iterator &
WorkQueue_Base::Reverse_Iterator::operator++() {
	increment();
	return *this;
}

/**
 * This is a post-increment.
 */
WorkQueue_Base::Reverse_Iterator
WorkQueue_Base::Reverse_Iterator::operator++(int) {
    WorkQueue_Base::Reverse_Iterator tmp(*this);
	increment();
	return tmp;
}

/**
 * Decrement.
 */
WorkQueue_Base::Reverse_Iterator &
WorkQueue_Base::Reverse_Iterator::operator--() {
	increment();
	return *this;
}

/**
 * This is a post-decrement.
 */
WorkQueue_Base::Reverse_Iterator
WorkQueue_Base::Reverse_Iterator::operator--(int) {
    WorkQueue_Base::Reverse_Iterator tmp(*this);
	decrement();
	return tmp;
}


//----------------------------------------------------------------------
// The const forward iterator.
//----------------------------------------------------------------------

/**
 * Constructor the simple, forward iterator.
 */
WorkQueue_Base::Const_Iterator::Const_Iterator(const WorkQueue_Base *_queue, LinkedList *pos)
    : WorkQueue_Base::IteratorBase::IteratorBase(_queue, true, pos)
{
}

/**
 * Copy operator.
 */
WorkQueue_Base::Const_Iterator &
WorkQueue_Base::Const_Iterator::operator=(const WorkQueue_Base::Const_Iterator &orig) {
    WorkQueue_Base::IteratorBase::operator=(orig);
	return *this;
}

/**
 * Increment.
 */
WorkQueue_Base::Const_Iterator &
WorkQueue_Base::Const_Iterator::operator++() {
	increment();
	return *this;
}

/**
 * This is a post-increment.
 */
WorkQueue_Base::Const_Iterator
WorkQueue_Base::Const_Iterator::operator++(int) {
    WorkQueue_Base::Const_Iterator tmp(*this);
	increment();
	return tmp;
}

/**
 * Decrement.
 */
WorkQueue_Base::Const_Iterator &
WorkQueue_Base::Const_Iterator::operator--() {
	increment();
	return *this;
}

/**
 * This is a post-decrement.
 */
WorkQueue_Base::Const_Iterator
WorkQueue_Base::Const_Iterator::operator--(int) {
    WorkQueue_Base::Const_Iterator tmp(*this);
	decrement();
	return tmp;
}


//----------------------------------------------------------------------
// The reverse iterator.
//----------------------------------------------------------------------

/**
 * Construct the reverse iterator.
 */
WorkQueue_Base::Const_Reverse_Iterator::Const_Reverse_Iterator(const WorkQueue_Base *_queue, LinkedList *pos)
    : WorkQueue_Base::IteratorBase::IteratorBase(_queue, false, pos)
{
}

/**
 * Copy operator.
 */
WorkQueue_Base::Const_Reverse_Iterator &
WorkQueue_Base::Const_Reverse_Iterator::operator=(const WorkQueue_Base::Const_Reverse_Iterator &orig) {
    WorkQueue_Base::IteratorBase::operator=(orig);
	return *this;
}

/**
 * Increment.
 */
WorkQueue_Base::Const_Reverse_Iterator &
WorkQueue_Base::Const_Reverse_Iterator::operator++() {
	increment();
	return *this;
}

/**
 * This is a post-increment.
 */
WorkQueue_Base::Const_Reverse_Iterator
WorkQueue_Base::Const_Reverse_Iterator::operator++(int) {
    WorkQueue_Base::Const_Reverse_Iterator tmp(*this);
	increment();
	return tmp;
}

/**
 * Decrement.
 */
WorkQueue_Base::Const_Reverse_Iterator &
WorkQueue_Base::Const_Reverse_Iterator::operator--() {
	increment();
	return *this;
}

/**
 * This is a post-decrement.
 */
WorkQueue_Base::Const_Reverse_Iterator
WorkQueue_Base::Const_Reverse_Iterator::operator--(int) {
    WorkQueue_Base::Const_Reverse_Iterator tmp(*this);
	decrement();
	return tmp;
}

//======================================================================
// For things we want to delete, we can make one of these.
//======================================================================

/**
 * Constructor.
 */
DeleteMe::DeleteMe(const string &_fromFile, int _fromLine, std::function<void(void *ptr)> &_deleteMethod, void *_ptr, int delaySeconds)
    : deleteMethod(_deleteMethod), ptr(_ptr), fromFile(_fromFile), fromLine(_fromLine)
{
	fireAt = system_clock::now() + seconds(delaySeconds);
}

/**
 * Destructor.
 */
DeleteMe::~DeleteMe() {
    if (ptr != nullptr && deleteMethod) {
		deleteMethod(ptr);
	}
}

/**
 * Log that we're about to run, helping to find problems.
 */
void
DeleteMe::report() {
    // CTLogStream::info() << "DeleteMe from " << fromFile << " :: " << fromLine << endl;
}

static WorkQueue_Template<DeleteMe> deleteMeQueue;
static bool deleteMeQueue_Running = false;
static mutex deleteMeMutex;

[[ noreturn ]] void
deleteMeRunner() {
	while (true) {
        shared_ptr<DeleteMe> deleteMe = deleteMeQueue.getMoreWork();
        if (deleteMe != nullptr) {
            deleteMe->report();
		}
	}
}

void
add_DeleteMe(const string &fromFile, int fromLine, std::function<void(void *ptr)> &_deleteMethod, void *ptr) {
    add_DeleteMe(fromFile, fromLine, _deleteMethod, ptr, 600);
}

void
add_DeleteMe(const string &fromFile, int fromLine, std::function<void(void *ptr)> &_deleteMethod, void *ptr, int delaySeconds) {
	std::unique_lock<std::mutex> mlock(deleteMeMutex);

    deleteMeQueue.add( make_shared<DeleteMe>(fromFile, fromLine, _deleteMethod, ptr, delaySeconds) );

	if (!deleteMeQueue_Running) {
		deleteMeQueue_Running = true;
		std::thread runner(deleteMeRunner);
		runner.detach();
	}
}

//======================================================================
// My runner classes.
//======================================================================

WorkQueue_Runner * WorkQueue_Runner::mySingleton = nullptr;
mutex WorkQueue_Runner::mutex;

/**
 * Constructor.
 */
WorkQueueRunner_Entry::WorkQueueRunner_Entry(std::chrono::system_clock::time_point _fireAt)
: WorkQueue_Entry(_fireAt), function(nullptr), object(nullptr)
{
}

/**
 * Constructor.
 */
WorkQueueRunner_Entry::WorkQueueRunner_Entry(WorkQueue_Function _function, void *_object, std::chrono::system_clock::time_point _fireAt)
:	WorkQueue_Entry(_fireAt), function(_function), object(_object)
{
}

/**
 * Destructor.
 */
WorkQueueRunner_Entry::~WorkQueueRunner_Entry() {
}

/**
 * Invoke our method.
 */
void
WorkQueueRunner_Entry::invoke() {
	function(object);
}

/**
 * Entry point because C++ threads are stupid.
 */
void
workqueueRunMethod() {
	WorkQueue_Runner::singleton()->run();
}

/**
 * Constructor.
 */
WorkQueue_Runner::WorkQueue_Runner() {
}

/**
 * Destructor.
 */
WorkQueue_Runner::~WorkQueue_Runner() {
}

/**
 * Return the singleton, starting it if necessary.
 */
WorkQueue_Runner *
WorkQueue_Runner::singleton() {
	std::unique_lock<std::mutex> mlock(mutex);
    if (mySingleton == nullptr) {
		mySingleton = new WorkQueue_Runner();
		std::thread thread(workqueueRunMethod);
		thread.detach();
	}
	return mySingleton;
}

/**
 * Run forever.
 */
void
WorkQueue_Runner::run() {
	while(true) {
        WorkQueue_Entry::Ptr ptr = getMoreWork();
        if (ptr != nullptr) {
            WorkQueueRunner_Entry & entry = static_cast<WorkQueueRunner_Entry &>(*ptr);
            entry.invoke();
		}
	}
}
