/*
 * Semaphore.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: user
 */

#include "Semaphore.h"

namespace npl {

Semaphore::Semaphore(int n)
{
	sem_init(&sem,0,n);
}

void Semaphore::lock()
{
	sem_wait(&sem);
}

void Semaphore::unlock()
{
	sem_post(&sem);
}

Semaphore::~Semaphore()
{
	sem_destroy(&sem);
}
}
