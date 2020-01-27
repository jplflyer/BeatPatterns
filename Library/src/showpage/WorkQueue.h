#ifndef SRC_LIB_WORKQUEUE_H_
#define SRC_LIB_WORKQUEUE_H_

#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>

/**
 * WorkQueue holds instances of these. (You can subclass.)
 */
class WorkQueue_Entry {
public:
    /** When should we be fired? */
	std::chrono::system_clock::time_point	fireAt;

    /** Define A pointer to one of us. */
    typedef std::shared_ptr<WorkQueue_Entry> Ptr;

public:
	WorkQueue_Entry() {}

    /** Constructor. */
	WorkQueue_Entry(std::chrono::system_clock::time_point _fireAt): fireAt(_fireAt) {}

    /** Destructor. */
	virtual ~WorkQueue_Entry() {}

	virtual void print(std::ostream &where) const;
};

std::ostream & operator<<(std::ostream &os, const WorkQueue_Entry & p);

/**
 * This implements a queue of work. To use, you subclass WorkQueue_Entry and add
 * them to the queue. Your worker thread can call getMoreWork() in a loop. This method
 * will block until either this is work to return OR you call the shutdown method, at
 * which point we'll return NULL.
 *
 * If we're destroyed with work still in the queue, we'll delete the contents.
 *
 * Internally, we're represented by a linked list. Adding new untimed items will be O(1)
 * time (very fast). Adding new timed events will likely be O(n).
 */
class WorkQueue_Base {

private:
    /** We're implemented as a linked list, which we hide from the outside. */
	class LinkedList {
	public:
        LinkedList(WorkQueue_Entry::Ptr ptr);
		~LinkedList();

        WorkQueue_Entry::Ptr	entry;      /**< A pointer to our actual WorkQueue_Entry. */
        LinkedList *		    previous;   /**< The previous item in our list. */
        LinkedList *		    next;       /**< The next item in our list. */
	};

public:
	// Define our iterators.

    /** The base class for all our iterators. */
	class IteratorBase {
	protected:
        const WorkQueue_Base *	queue;      /**< What queue are we for? */
        LinkedList *	    ptr;        /**< Where are we currently pointed? */
        bool			    forward;    /**< Are we moving forward or in reverse? */

		void increment();
		void decrement();

	public:
        IteratorBase(const WorkQueue_Base *_queue, bool _forward, LinkedList *pos);
		IteratorBase(const IteratorBase &orig);
		virtual ~IteratorBase();

		IteratorBase &	operator=(const IteratorBase &orig);

        WorkQueue_Entry::Ptr	operator*();
        WorkQueue_Entry::Ptr	operator->();

		bool operator==(const IteratorBase &other) const;
		bool operator!=(const IteratorBase &other) const;

        friend class WorkQueue_Base;
	};

    /** This is a forward iterator. */
	class Iterator: public IteratorBase {
	public:
        Iterator(WorkQueue_Base *_queue, LinkedList *pos);
        /** Copy constructor. */
		Iterator(const Iterator &orig): IteratorBase(orig) {}

		Iterator &	operator=(const Iterator &orig);

		Iterator &	operator++();
		Iterator	operator++(int);
		Iterator &	operator--();
		Iterator	operator--(int);

	};

    /** This is a reverse iterator. */
	class Reverse_Iterator: public IteratorBase {
	public:
        Reverse_Iterator(WorkQueue_Base *_queue, LinkedList *pos);
        /** Copy constructor. */
		Reverse_Iterator(const Reverse_Iterator &orig): IteratorBase(orig) {}

		Reverse_Iterator &	operator=(const Reverse_Iterator &orig);

		Reverse_Iterator &	operator++();
		Reverse_Iterator	operator++(int);
		Reverse_Iterator &	operator--();
		Reverse_Iterator	operator--(int);
	};

    /** Const forward iterator. */
	class Const_Iterator: public IteratorBase {
	public:
        Const_Iterator(const WorkQueue_Base *_queue, LinkedList *pos);

        /** Copy constructor */
		Const_Iterator(const Const_Iterator &orig): IteratorBase(orig) {}

		Const_Iterator &	operator=(const Const_Iterator &orig);

		Const_Iterator &	operator++();
		Const_Iterator		operator++(int);
		Const_Iterator &	operator--();
		Const_Iterator		operator--(int);
	};

    /** Const reverse iterator. */
	class Const_Reverse_Iterator: public IteratorBase {
	public:
        Const_Reverse_Iterator(const WorkQueue_Base *_queue, LinkedList *pos);

        /** Copy constructor. */
		Const_Reverse_Iterator(const Const_Reverse_Iterator &orig): IteratorBase(orig) {}

		Const_Reverse_Iterator &	operator=(const Const_Reverse_Iterator &orig);

		Const_Reverse_Iterator &	operator++();
		Const_Reverse_Iterator		operator++(int);
		Const_Reverse_Iterator &	operator--();
		Const_Reverse_Iterator		operator--(int);
	};

    /** Signature of function for comparing objects. */
    typedef std::function<bool(WorkQueue_Entry::Ptr)> ComparisonFunct;

private:
    LinkedList *	head;       /**< Head of our linked list. */
    LinkedList *	tail;       /**< Tail of our linked list. */
    bool			running;    /**< Shoudl we continue to run? */

    std::mutex					mutex;      /**< For mutex locking. */
    std::condition_variable		condVar;    /**< While waiting for data to appear. */

	IteratorBase endIterator;
	IteratorBase endReverseIterator;

    void _remove(IteratorBase &);
    void _add(WorkQueue_Entry::Ptr);

public:
	friend class IteratorBase;

    WorkQueue_Base();
    virtual ~WorkQueue_Base();

    void baseAdd(WorkQueue_Entry::Ptr);
    void baseAddFront(WorkQueue_Entry::Ptr);
    void baseAdd(WorkQueue_Entry::Ptr, const long millisecondDelay);

    void baseAddIfEmpty(WorkQueue_Entry::Ptr);
    void baseAddIfEmpty(WorkQueue_Entry::Ptr, const long millisecondDelay);

    void remove(ComparisonFunct);
    void remove(IteratorBase &);

	void shutdown();
    void reset();

    WorkQueue_Entry::Ptr baseGetMoreWork();
    WorkQueue_Entry::Ptr baseGetMoreWork(std::chrono::milliseconds duration);

	void listQueue();
	void listQueueReverse();

    /** Are we marked to contiue running? */
	bool isRunning() { return running; }

	Iterator begin();
    /** Return the iterator marking the end of the list. This never changes. */
	IteratorBase & end() { return endIterator; }

	Reverse_Iterator rbegin();
    /** Return the reverse-iterator marking the end of the list. This never changes. */
	IteratorBase & rend() { return endReverseIterator; }

	// The compiler knows to use these methods if
	// the WorkQueue is const, or in the case of cbegin(), we
	// specifically ask.
	Const_Iterator cbegin() const;
    /** Return the const iterator marking the end of the list. This never changes. */
    IteratorBase cend() const { return endIterator; }

	Const_Reverse_Iterator crbegin() const;
    /** Return the const reverse-iterator marking the end of the list. This never changes. */
    IteratorBase crend() const { return endReverseIterator; }

    /** Convenience method. */
	Const_Iterator begin() const { return cbegin(); }

    /** Convenience method. */
	IteratorBase end() const { return cend(); }

    /** Convenience method. */
    Const_Reverse_Iterator rbegin() const { return crbegin(); }

    /** Convenience method. */
    IteratorBase rend() const { return crend(); }
};

/**
 * This defines a generic type.
 */
template<typename T>
class WorkQueue_Template: public WorkQueue_Base {
public:
    typedef std::shared_ptr<T> Ptr;

    void add(Ptr ptr) { baseAdd( std::static_pointer_cast<WorkQueue_Entry>(ptr) ); }
    void addFront(Ptr ptr) { baseAddFront( std::static_pointer_cast<WorkQueue_Entry>(ptr) ); }
    void add(Ptr ptr, const long millisecondDelay) { baseAdd( std::static_pointer_cast<WorkQueue_Entry>(ptr), millisecondDelay ); }
    void addIfEmpty(Ptr ptr) { baseAddIfEmpty( std::static_pointer_cast<WorkQueue_Entry>(ptr) ); }
    void addIfEmpty(Ptr ptr, const long millisecondDelay) { baseAddIfEmpty( std::static_pointer_cast<WorkQueue_Entry>(ptr), millisecondDelay ); }

    Ptr getMoreWork() { return std::static_pointer_cast<T>( baseGetMoreWork() ); }
    Ptr getMoreWork(std::chrono::milliseconds duration) { return std::static_pointer_cast<T>( baseGetMoreWork(duration) ); }
};

typedef WorkQueue_Template<WorkQueue_Entry> WorkQueue;

//======================================================================
// This is for deleting-later, but I want to stop using it.
// It was a bad idea.
//======================================================================

/** This should be deprecated in favor of shared_ptr. */
class DeleteMe: public WorkQueue_Entry {
private:
    std::function<void(void *ptr)> deleteMethod;
	void *ptr;

    std::string fromFile;
    int fromLine;

public:
    DeleteMe(const std::string & _fromFile, int _fromLine, std::function<void(void *ptr)> &_deleteMethod, void *ptr, int delaySeconds);
	virtual ~DeleteMe();

    void report();
};

extern void add_DeleteMe(const std::string &fromFile, int fromLine, std::function<void(void *ptr)> &_deleteMethod, void *ptr);
extern void add_DeleteMe(const std::string &fromFile, int fromLine, std::function<void(void *ptr)> &_deleteMethod, void *ptr, int delaySeconds);

//======================================================================
// The background thread runner.
//======================================================================

/**
 * For this class, you can subclass and override the invoke method, if you want.
 */
class WorkQueueRunner_Entry: public WorkQueue_Entry {
public:
    /** Signature of the method to call when there's work to do. */
	using WorkQueue_Function = std::function<void(void *)>;

private:
    WorkQueue_Function	function;       /**< The callback. */
    void *				object;         /**< User data to pass to the callback. */

public:
	WorkQueueRunner_Entry(std::chrono::system_clock::time_point _fireAt);
	WorkQueueRunner_Entry(WorkQueue_Function _function, void *_object, std::chrono::system_clock::time_point _fireAt);
	virtual ~WorkQueueRunner_Entry();

    void * getObject() { return object; }

	virtual void invoke();
};

/**
 * This implements a thread of timed callbacks.
 */
class WorkQueue_Runner: public WorkQueue_Template<WorkQueueRunner_Entry> {
private:
	static WorkQueue_Runner *	mySingleton;
	static std::mutex			mutex;

	WorkQueue_Runner();

public:
	static WorkQueue_Runner *	singleton();

	virtual ~WorkQueue_Runner();
	void run();
};

#endif /* SRC_LIB_WORKQUEUE_H_ */
