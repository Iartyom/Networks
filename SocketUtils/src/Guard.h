/*
 * Guard.h
 *
 *  Created on: Feb 13, 2013
 *      Author: efi
 */

 #ifndef GUARD_H_
 #define GUARD_H_
 
 #include <pthread.h>
 /**
  * To init a mutex use:	pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;
  */
namespace npl{
 class Guard{
     pthread_mutex_t* mutex;
     bool disposed;
 public:
     Guard(pthread_mutex_t* m){
         disposed = false;
         mutex = m;
         pthread_mutex_lock(mutex);
     }
     void dispose(){
         if(!disposed){
            disposed = true;
            pthread_mutex_unlock(mutex);

         }
     }
     ~Guard(){
        if(!disposed){
            pthread_mutex_unlock(mutex);
        }
     }
 };
}
 #endif /* GUARD_H_ */
 
