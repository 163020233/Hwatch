#include "pthread_impl.h"

#ifndef __LITEOS__
#include "fork_impl.h"
#endif
#if defined LOSCFG_COMPAT_POSIX_THREAD_KERNEL_MODE_CALL || !(defined LOSCFG_BOX)
static void (*keys[PTHREAD_KEYS_MAX])(void *);

#ifdef __LITEOS__
static pthread_mutex_t key_lock = PTHREAD_MUTEX_INITIALIZER;
#else
static pthread_rwlock_t key_lock = PTHREAD_RWLOCK_INITIALIZER;
#endif
static pthread_key_t next_key;

typedef void (*pthread_keys_func)(void *);

#endif

#ifdef LOSCFG_BOX
#include "los_perbox_var_pri.h"
#include "los_task_pri.h"

static inline void *get_perbox_pthread_key(void)
{
#ifdef LOSCFG_COMPAT_POSIX_THREAD_KERNEL_MODE_CALL
	if (OS_IN_KER_STATE) {
		return (void *)(uintptr_t)-1;
	}
#endif
	perbox_data *box_pri_data = (perbox_data *)OsPerboxDataAddrGet(LOS_CurTaskIDGet());
	void *array = NULL;
	if (box_pri_data == NULL) {
		return NULL;
	}
	array = box_pri_data->pthreadKey;
	return array;
}

static inline pthread_keys_func *pthread_get_keys(void *array)
{
#ifdef LOSCFG_COMPAT_POSIX_THREAD_KERNEL_MODE_CALL
	if (OS_IN_KER_STATE) {
		(void)array;
		return &keys[0];
	}
#endif
	return &((perbox_pthread_key *)(uintptr_t)array)->keys[0];
}

static inline pthread_mutex_t *pthread_get_key_lock(void *array)
{
#ifdef LOSCFG_COMPAT_POSIX_THREAD_KERNEL_MODE_CALL
	if (OS_IN_KER_STATE) {
		(void)array;
		return &key_lock;
	}
#endif
	return &((perbox_pthread_key *)(uintptr_t)array)->keyLock;
}

static inline pthread_key_t *pthread_get_next_key(void *array)
{
#ifdef LOSCFG_COMPAT_POSIX_THREAD_KERNEL_MODE_CALL
	if (OS_IN_KER_STATE) {
		(void)array;
		return &next_key;
	}
#endif
	return &((perbox_pthread_key *)(uintptr_t)array)->nextKey;
}

#else /* !LOSCFG_BOX */
volatile size_t __pthread_tsd_size = sizeof(void *) * PTHREAD_KEYS_MAX;
void *__pthread_tsd_main[PTHREAD_KEYS_MAX] = { 0 };

static inline void *get_perbox_pthread_key(void)
{
	return (void *)(uintptr_t)-1;
}

static inline pthread_keys_func *pthread_get_keys(void *array)
{
	(void)array;
	return &keys[0];
}

static inline pthread_mutex_t *pthread_get_key_lock(void *array)
{
	(void)array;
	return &key_lock;
}

static inline pthread_key_t *pthread_get_next_key(void *array)
{
	(void)array;
	return &next_key;
}
#endif /* LOSCFG_BOX */

static void nodtor(void *dummy)
{
}

static void dummy_0(void)
{
}

weak_alias(dummy_0, __tl_lock);
weak_alias(dummy_0, __tl_unlock);

#ifdef __LITEOS__
int __pthread_key_create(pthread_key_t *k, void (*dtor)(void *))
{
	void *array = get_perbox_pthread_key();
	if (array == NULL) {
		return EINVAL;
	}
	pthread_mutex_t *k_lock = pthread_get_key_lock(array);
	pthread_keys_func *k_func = pthread_get_keys(array);
	pthread_key_t *k_next = pthread_get_next_key(array);
	/* Purely a sentinel value since null means slot is free. */
	if (!dtor) dtor = nodtor;

	(void)pthread_mutex_lock(k_lock);
	pthread_key_t j = *k_next;
	do {
		if (!k_func[j]) {
			k_func[*k_next = *k = j] = dtor;
			(void)pthread_mutex_unlock(k_lock);
			return 0;
		}
	} while ((j=(j+1)%PTHREAD_KEYS_MAX) != *k_next);

	(void)pthread_mutex_unlock(k_lock);
	return EAGAIN;
}

int __pthread_key_delete(pthread_key_t k)
{
	_pthread_data *self = __pthread_self(), *td = self;
	void *array = get_perbox_pthread_key();
	if ((array == NULL) || (self == NULL) || ((uintptr_t)k >= PTHREAD_KEYS_MAX)) {
		return EINVAL;
	}

	pthread_mutex_t *k_lock = pthread_get_key_lock(array);
	pthread_keys_func *k_func = pthread_get_keys(array);

	(void)pthread_mutex_lock(k_lock);

	__tl_lock();
	td->tsd[k] = 0;
	__tl_unlock();

	k_func[k] = 0;

	(void)pthread_mutex_unlock(k_lock);

	return 0;
}

void __pthread_tsd_run_dtors(void)
{
	_pthread_data *self = __pthread_self();
	void *array = get_perbox_pthread_key();
	int i, j;

	if ((array == NULL) || (self == NULL)) {
		return;
	}

	pthread_mutex_t *k_lock = pthread_get_key_lock(array);
	pthread_keys_func *k_func = pthread_get_keys(array);

	for (j=0; self->tsd_used && j<PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
		(void)pthread_mutex_lock(k_lock);
		self->tsd_used = 0;
		for (i=0; i<PTHREAD_KEYS_MAX; i++) {
			void *val = self->tsd[i];
			void (*dtor)(void *) = k_func[i];
			self->tsd[i] = 0;
			if (val && dtor && dtor != nodtor) {
				(void)pthread_mutex_unlock(k_lock);
				dtor(val);
				(void)pthread_mutex_lock(k_lock);
			}
		}
		(void)pthread_mutex_unlock(k_lock);
	}
}
#else
void __pthread_key_atfork(int who)
{
	if (who<0) __pthread_rwlock_rdlock(&key_lock);
	else if (!who) __pthread_rwlock_unlock(&key_lock);
	else key_lock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
}

int __pthread_key_create(pthread_key_t *k, void (*dtor)(void *))
{
	pthread_t self = __pthread_self();

	/* This can only happen in the main thread before
	 * pthread_create has been called. */
	if (!self->tsd) self->tsd = __pthread_tsd_main;

	/* Purely a sentinel value since null means slot is free. */
	if (!dtor) dtor = nodtor;

	__pthread_rwlock_wrlock(&key_lock);
	pthread_key_t j = next_key;
	do {
		if (!keys[j]) {
			keys[next_key = *k = j] = dtor;
			__pthread_rwlock_unlock(&key_lock);
			return 0;
		}
	} while ((j=(j+1)%PTHREAD_KEYS_MAX) != next_key);

	__pthread_rwlock_unlock(&key_lock);
	return EAGAIN;
}

int __pthread_key_delete(pthread_key_t k)
{
	sigset_t set;
	pthread_t self = __pthread_self(), td=self;

	__block_app_sigs(&set);
	__pthread_rwlock_wrlock(&key_lock);

	__tl_lock();
	do td->tsd[k] = 0;
	while ((td=td->next)!=self);
	__tl_unlock();

	keys[k] = 0;

	__pthread_rwlock_unlock(&key_lock);
	__restore_sigs(&set);

	return 0;
}

void __pthread_tsd_run_dtors()
{
	pthread_t self = __pthread_self();
	int i, j;
	for (j=0; self->tsd_used && j<PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
		__pthread_rwlock_rdlock(&key_lock);
		self->tsd_used = 0;
		for (i=0; i<PTHREAD_KEYS_MAX; i++) {
			void *val = self->tsd[i];
			void (*dtor)(void *) = keys[i];
			self->tsd[i] = 0;
			if (val && dtor && dtor != nodtor) {
				__pthread_rwlock_unlock(&key_lock);
				dtor(val);
				__pthread_rwlock_rdlock(&key_lock);
			}
		}
		__pthread_rwlock_unlock(&key_lock);
	}
}
#endif

weak_alias(__pthread_key_create, pthread_key_create);
weak_alias(__pthread_key_delete, pthread_key_delete);
