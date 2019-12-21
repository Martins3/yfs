// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
    nacquire (0)
{
  VERIFY(pthread_mutex_init(&state_m_, NULL) == 0);
  VERIFY(pthread_cond_init(&state_c_, NULL) == 0);
}

lock_server::~lock_server()
{
  VERIFY(pthread_mutex_destroy(&state_m_) == 0);
  VERIFY(pthread_cond_destroy(&state_c_) == 0);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
    lock_protocol::status ret = lock_protocol::OK;
    printf("stat request from clt %d\n", clt);
    r = nacquire;
    return ret;
}

lock_protocol::status lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &){
    lock_protocol::status ret = lock_protocol::OK;
    VERIFY(pthread_mutex_lock(&state_m_) == 0);
    while(lock_state[lid]){
			VERIFY(pthread_cond_wait(&state_c_, &state_m_) == 0);
    }
    lock_state[lid] = true;
    printf(" ----> %d %lld locked the server !\n", clt, lid);
    VERIFY(pthread_mutex_unlock(&state_m_) == 0);
    return ret;
}

lock_protocol::status lock_server::release(int clt, lock_protocol::lockid_t lid, int &){
    lock_protocol::status ret = lock_protocol::OK;
    VERIFY(pthread_mutex_lock(&state_m_) == 0);
    lock_state[lid] = false;
	  VERIFY(pthread_cond_signal(&state_c_) == 0);
    printf(" ----> %d %lld unlocked the server !\n", clt, lid);
    VERIFY(pthread_mutex_unlock(&state_m_) == 0);
    return ret;
}
