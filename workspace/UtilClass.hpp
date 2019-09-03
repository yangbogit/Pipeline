/*********************************************************
* Copyright 
* 
* File    : UtilClass.hpp
*           Description: Header file for ThreadsClass.hpp,
*           store some commonly used function
*           Version : 1.0
*           Date    : 2019-8-13
*           Author  : 
*           ------------- Revision History: --------------         
*********************************************************/

#ifndef UTILCLASS_HPP_
#define UTILCLASS_HPP_

#include "Macro.hpp"

class UtilClass {

    public:

        /****************************************** * Funcion:*
        * printTime * Author: *  * Description:
        *    A function used to print time 
        *    Parameters: 
        *    Returns:
        *    long     
        *******************************************************/
        long getTime() {
            struct timeval tv;
            gettimeofday(&tv,NULL);
            return tv.tv_sec*1000 + tv.tv_usec/1000; 
        }

        /******************************************** * Funcion:*
        * nssleep * Author: *  * Description:
        *    A function used to suspend a thread for a while.
        *    Parameters: 
        *       time: [long] set thread suspend time.
        *    Returns:
        *    void     
        *******************************************************/
        void nssleep(long time) {
            struct timespec tim, tim2;
            tim.tv_sec = 0;
            tim.tv_nsec = time;

            if( nanosleep( &tim, &tim2) < 0 ) {
                std::cout << "nano sleep failed" << std::endl;
                exit(-1);
            }
        }
};

#endif /* UTILCLASS_HPP_ */