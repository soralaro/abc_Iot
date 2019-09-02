#ifndef __FG_MUTEX_H__
#define __FG_MUTEX_H__

#include <assert.h>
#include <syscall.h>
#include <pthread.h>
#include <unistd.h>

#define locker_t pthread_mutex_t
#define locker_lock pthread_mutex_lock
#define locker_unlock pthread_mutex_unlock
#define locker_destroy pthread_mutex_destroy

inline int locker_create(locker_t *locker)
{
    // create a recusive locker
    int r;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    r = pthread_mutex_init(locker, &attr);
    pthread_mutexattr_destroy(&attr);
    return r;
}

class CMutex
{
public:
    inline CMutex()
    {
        int ret = locker_create(&m_Mutex);
        assert(0 == ret);
        m_LockThreadID = 0;
        m_nLockCount = 0;
    };

    inline ~CMutex()
    {
        locker_destroy(&m_Mutex);
    };

    inline bool Enter()
    {
        // DLOG(INFO) << "++++++++++++++++++++++++++++++++Mutex Enter";
        if (locker_lock(&m_Mutex) == 0)
        {
            m_LockThreadID = syscall(__NR_gettid);
            m_nLockCount++;
            return true;
        }
        return false;
    };

    inline bool Leave()
    {
        // DLOG(INFO) << "++++++++++++++++++++++++++++++++Mutex Leave";
        m_nLockCount--;
        m_LockThreadID = syscall(__NR_gettid);
        if (locker_unlock(&m_Mutex) == 0)
        {
            return true;
        }
        return false;
    };

protected:
    pid_t m_LockThreadID;
    int m_nLockCount;

private:
    locker_t m_Mutex;
};

#endif //__MUTEX_H__
