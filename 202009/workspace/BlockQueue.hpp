/*********************************************************
* Copyright 
* 
* File    : BlockQueue.hpp
*           Description: Header file for ThreadsClass.hpp.
*           To define some macros and declare a template 
*           blocking queue class.
*           Version : 1.0
*           Date    : 2019-8-2
*           Author  : 
*           ------------- Revision History: --------------         
*********************************************************/

#ifndef  BLOCKQUEUE_HPP_
#define  BLOCKQUEUE_HPP_

#include "Macro.hpp"

/**************************************************** * Class:*
*  BlockQueue * Author: *  * Description:
*    A template class used to generate deffrent type of queue.    
*************************************************************/
template<class T>
class BlockQueue {
	public:

        /* Constructor Function */
		BlockQueue() {
            this->m_MaxCapacity = BLOCK_SIZE;
            this->m_Queue_Empty = true;
            this->m_Queue_Full = false;
            pthread_mutex_init( &(this->m_Mutex), NULL);
            pthread_cond_init( &(this->m_Cond_Empty), NULL);
            pthread_cond_init( &(this->m_Cond_Full), NULL);
        }

        /********************************************** * Funcion:*
        * elemPush * Author: *  * Description:
        *    A function used to push element into a blocking queue.
        *    Parameters: 
        *       temp:[char *] a element put into queue.
        *    Returns:
        *    void     
        **********************************************************/
        void elemPush(ElemType &temp) {
            pthread_mutex_lock(&(m_Mutex));

            if (m_Queue.size() == BLOCK_SIZE) {
                m_Queue_Full = true;
            }

            while(m_Queue_Full) {
                pthread_cond_wait(&(m_Cond_Full), &(m_Mutex));
            }

            m_Queue.push_back(temp);
            m_Queue_Empty = false;
            pthread_cond_signal(&(m_Cond_Empty));

            pthread_mutex_unlock(&(m_Mutex));
        }

        /********************************************* * Funcion:*
        * elemPop * Author: *  * Description:
        *    A function used to pop element from a blocking queue.
        *    Parameters: 
        *       temp:[char *] a element pop from queue.
        *    Returns:
        *    void     
        **********************************************************/
        void elemPop(ElemType &temp) {
            pthread_mutex_lock( &(m_Mutex));

            if (m_Queue.size() == 0) {
                m_Queue_Empty = true;
            }

            while(m_Queue_Empty) {
                pthread_cond_wait ( &(m_Cond_Empty), &(m_Mutex));
            }

            temp = *m_Queue.begin();
			m_Queue.pop_front();
            m_Queue_Full = false;
            pthread_cond_signal ( &(m_Cond_Full));

            pthread_mutex_unlock( &(m_Mutex));
        }
	
	public:

        int m_MaxCapacity;              // blocking queue maximum capacity
		std::deque<T> m_Queue;          // blocking queue generate from a deque type

        bool m_Queue_Empty;             // declare queue empty status
        bool m_Queue_Full;              // declare queue full status
        pthread_mutex_t m_Mutex;        // a mutex for this blocking queue.
        pthread_cond_t  m_Cond_Empty;   // a condition varianble to declare queue empty
        pthread_cond_t  m_Cond_Full;    // a condition varianble to declare queue full
};

BlockQueue<ElemType> g_Queue[THREAD_NUM+1];

#endif /* BLOCKQUEUE_HPP_ */