/*
 * Semaphore.h
 *
 *  Created on: Sep 5, 2017
 *      Author: user
 */

#ifndef SRC_SEMAPHORE_H_
#define SRC_SEMAPHORE_H_
#include <semaphore.h>
namespace npl {

class Semaphore {

private:
	sem_t sem;

public:
	Semaphore(int n);
	void lock();
	void unlock();
	~Semaphore();

};

} /* namespace npl */

#endif /* SRC_SEMAPHORE_H_ */
