/*********************************************************
* Copyright (c) 
* 
* File    : Macro.hpp
*           Description: Header file, store some common
*           header files.
*           Version : 1.0
*           Date    : 2019-8-13
*           Author  : 
*           ------------- Revision History: --------------         
*********************************************************/

#ifndef  MACRO_HPP_
#define  MACRO_HPP_

#include <deque>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

/* set thread amount and size of blocking queue */
#define  THREAD_NUM           3   
#define  BLOCK_SIZE           50   

/* set each thread watermark place */
#define  PROCESS_A_MARKPLACE  1        
#define  PROCESS_B_MARKPLACE  2
#define  PROCESS_C_MARKPLACE  3

/* set picture amount , read and process delay time */
#define  PICTURE_NUMBER       199        
#define  READ_DELAY_NS        30000000L  
#define  PROCESS_DELAY_NS     20000000L  

/* set picture width and length */
#define  PICTURE_WIDTH        1536      
#define  PICTURE_HEIGHT       1088      

/* picture byte length */
#define  PICTURE_LENGTH       PICTURE_WIDTH * PICTURE_HEIGHT * 3 / 2    

/* set watermark picture width and length */
#define  WATERMARK_WIDTH      100       
#define  WATERMARK_HEIGHT     100        

/* watermark picture byte length */
#define  WATERMARK_LENGTH     WATERMARK_WIDTH * WATERMARK_WIDTH * 3 / 2  

/* queue element type */
typedef  char *  ElemType; 

#endif /* MACRO_HPP_ */