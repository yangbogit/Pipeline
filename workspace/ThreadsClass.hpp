/*************************************************************
* Copyright 
* 
* File    : ThreadsClass.hpp
*           Description: Header file for PipelineMain.cpp.
*           To declare a thread class, a read picture class,
*           a write picture class and friend thread functions.
*           Version : 1.0
*           Date    : 2019-8-2
*           Author  : 
*           ------------- Revision History: --------------         
*************************************************************/

#ifndef  THREADSCLASS_HPP_
#define  THREADSCLASS_HPP_

#include "BlockQueue.hpp"
#include "UtilClass.hpp"
#include "AlgorithmClassA.hpp"
#include "AlgorithmClassB.hpp"
#include "AlgorithmClassC.hpp"

/************************************************** * Class:*
*  ThreadClass * Author: *  * Description:
*    A  class used to package thread attribute  
***********************************************************/

class ThreadClass {

    public:

        /* declare a friend funciton*/
        friend void *processThread(void *arg);

        /* Constructor Function */
        ThreadClass(int Class_ID) : m_Class_ID(Class_ID){}

        /*********************************************** * Funcion:*
        * processPicture * Author: *  * Description:
        *    A function used to watermark original picture with
        *    defferent algorithm.
        *    Parameters: 
        *       raw_picture_yuv: [char *] yuv of orignal picture
        *       mark_offset: [int] used to get watermark place.
        *    Returns:
        *    void     
        ***********************************************************/
        void processPicture(ElemType &raw_picture_yuv, int m_Class_ID){

            switch (m_Class_ID)
            {
                case 1:
                    m_Algorithm_A.waterMark(raw_picture_yuv, m_Class_ID);
                    break;
                case 2:
                    m_Algorithm_B.waterMark(raw_picture_yuv, m_Class_ID);
                    break;
                case 3:
                    m_Algorithm_C.waterMark(raw_picture_yuv, m_Class_ID);
                    break;
                
                default:
                    std::cout << "algorithm wrong!!!" << std::endl;
                    exit(-1);
            }
        }

    public:

        int m_Class_ID;      // watermark place
        pthread_t m_Thread;  // thread identifier

        UtilClass m_Util;    // use to print time and delay
        AlgorithmClassA m_Algorithm_A; // a algorithm in pipeline
        AlgorithmClassB m_Algorithm_B; // a algorithm in pipeline
        AlgorithmClassC m_Algorithm_C; // a algorithm in pipeline
};

/***************************************************** * Class:*
*  ReadPictureClass * Author: *  * Description:
*    A  class used to package thread attribute  
**************************************************************/
class ReadPictureClass {

    public:

        /* declare a friend funciton*/
        friend void *readPictureThread(void *arg);

        /* Constructor Function */
        ReadPictureClass(int Class_ID) : m_Class_ID(Class_ID){}

        /**************************************** * Funcion:*
        * readPicture * Author: *  * Description:
        *    A function used to read a picture
        *    Parameters: 
        *       read_picture_count: [int] picture number.
        *       temp: [char *] save picture yuv byte.
        *    Returns:
        *    void     
        ****************************************************/
        void readPicture(int read_picture_count,  ElemType &temp) {
            std::string pathin = m_PicturePathIn 
                                 + std::to_string(read_picture_count) 
                                 + m_PictureFormat;

            const char * fopen_path = pathin.c_str();
           
            FILE * read = nullptr;
            read = fopen(fopen_path, "rb"); 
            if (read == nullptr) {
                std::cout << "Read picture thread open file failed" << std::endl;
                exit(-1);
            }

            fread(temp, sizeof(char), PICTURE_LENGTH, read);
            fclose(read);
        }

    public:

        int m_Class_ID;       // thread name
        pthread_t m_Thread;   // thread identifier
        UtilClass m_Util;     // use to print time and delay
        
        /* path to read picture */
        std::string m_PictureFormat = ".yuv";
        std::string m_PicturePathIn = ".../camera_nv12_1536x1088/nv12_";  
};

/****************************************************** * Class:*
 *  WritePictureClass * Author: *  * Description:
 *    A  class used to package thread attribute  
 ***************************************************************/
class WritePictureClass {

    public:

        /* declare a friend funciton*/
        friend void *writePictureThread(void *arg);

        /* Constructor Function */
        WritePictureClass(int Class_ID) : m_Class_ID(Class_ID){}

        /********************************************** * Funcion:*
        * writePicture * Author: *  * Description:
        *    A function used to write a picture
        *    Parameters: 
        *       write_picture_count: [int] picture number.
        *       temp: [char *] get picture yuv byte.
        *    Returns:
        *    void     
        **********************************************************/
        void writePicture(int write_picture_count, ElemType &temp) {

            std::string pathout = m_PicturePathOut 
                                  + std::to_string(write_picture_count) 
                                  + m_PictureFormat;

            const char * fopen_path = pathout.c_str();
            
            FILE * write = nullptr;
            write = fopen(fopen_path, "wb+"); 
            if (write == nullptr) {
                std::cout << "Write picture thread open file failed" << std::endl;  
                exit(-1);
            }

            fwrite(temp, sizeof(char), PICTURE_LENGTH, write);
            fclose(write);
        }

    public:

        int m_Class_ID;      // thread name
        pthread_t m_Thread;  // thread identifier
        UtilClass m_Util;    // use to print time and delay
        
        /* path to read picture */
        std::string m_PictureFormat = ".yuv";
        std::string m_PicturePathOut = ".../camera_out/output_"; 
};

/*************************************************** * Funcion:*
* readPictureThread * Author: *  * Description:
*    A thread and friend function of ReadPictureClass.
*    Parameters: 
*       arg: [void *] use to pass a ReadPictureClass pointer.
*    Returns:
*    void     
***************************************************************/
void *readPictureThread(void *arg) {
    ReadPictureClass *readPointer = (ReadPictureClass *)arg;

    for (int i = 1; i <= PICTURE_NUMBER ; i++) {

        /* print current time  */
        std::cout << "th = " << readPointer->m_Class_ID << " ; " 
                  <<readPointer->m_Util.getTime() << " ms" << std::endl;   

        ElemType temp = (ElemType)malloc( PICTURE_LENGTH ); 

        /* read a picture */
        readPointer->readPicture(i, temp);     

        /* delay for while */
        readPointer->m_Util.nssleep(READ_DELAY_NS);  

        /* push yuv string into queue */ 
        g_Queue[readPointer->m_Class_ID].elemPush(temp);

        std::cout << "th = " << readPointer->m_Class_ID << " ; Q" 
                  << readPointer->m_Class_ID << ".size = " 
                  << g_Queue[readPointer->m_Class_ID].m_Queue.size() << std::endl;
        
        /* print current time  */
        std::cout << "th = " << readPointer->m_Class_ID   << " ; " 
                  <<readPointer->m_Util.getTime() << " ms" << std::endl;
    }
}

/*************************************************** * Funcion:*
* processThread * Author: *  * Description:
*    A thread and friend function of ThreadClass.
*    Parameters: 
*       arg: [void *] use to pass a ThreadClass pointer.
*    Returns:
*    void     
***************************************************************/
void *processThread(void *arg) { 
    ThreadClass *processPointer = (ThreadClass *)arg;

    for (int i = 1; i <= PICTURE_NUMBER ; i++) {
        
       /* print current time  */
        std::cout << "th = " << processPointer->m_Class_ID << " ; " 
                  <<processPointer->m_Util.getTime() << " ms" << std::endl;  

        ElemType temp = (ElemType)malloc( PICTURE_LENGTH ); 

        /* pop a element from last queue */
        g_Queue[ processPointer->m_Class_ID-1].elemPop(temp); 

        /* watermark  */         
        processPointer->processPicture(temp, processPointer->m_Class_ID);   
        
        /* delay for a while */
        processPointer->m_Util.nssleep(PROCESS_DELAY_NS);   

        /* push element into next queue after watermark  */                    
        g_Queue[ processPointer->m_Class_ID].elemPush(temp);           
        
        std::cout << "th = " << processPointer->m_Class_ID << " ; Q" 
                  << processPointer->m_Class_ID-1 << ".size = " 
                  << g_Queue[processPointer->m_Class_ID-1].m_Queue.size() << std::endl;
        
        std::cout << "th = " << processPointer->m_Class_ID << " ; Q" 
                  << processPointer->m_Class_ID << ".size = " 
                  << g_Queue[processPointer->m_Class_ID].m_Queue.size() << std::endl;
        
        /* print current time  */
        std::cout << "th = " << processPointer->m_Class_ID   << " ; " 
                  <<processPointer->m_Util.getTime() << " ms" << std::endl; 
    }   
}

/*************************************************** * Funcion:*
* writePictureThread * Author: *  * Description:
*    A thread and friend function of WritePictureClass.
*    Parameters: 
*       arg: [void *] use to pass a WritePictureClass pointer.
*    Returns:
*    void     
***************************************************************/
void *writePictureThread(void *arg) {
    WritePictureClass *writePointer = (WritePictureClass *)arg;

    for (int i = 1; i <= PICTURE_NUMBER ; i++) {

        /* print current time  */
        std::cout << "th = " << writePointer->m_Class_ID   << " ; " 
                  <<writePointer->m_Util.getTime() << " ms" << std::endl;

        ElemType temp = (ElemType)malloc( PICTURE_LENGTH ); 

        /* pop a element from last queue */
        g_Queue[writePointer->m_Class_ID-1].elemPop(temp);  

        /* write it into file system */
        writePointer->writePicture(i, temp);                

        std::cout << "th = " << writePointer->m_Class_ID << " ; Q" 
                  << writePointer->m_Class_ID-1 << ".size = " 
                  << g_Queue[writePointer->m_Class_ID-1].m_Queue.size() << std::endl;
            
        /* print current time  */
        std::cout << "th = " << writePointer->m_Class_ID   << " ; " 
                  <<writePointer->m_Util.getTime() << " ms" << std::endl;  
    }
}

#endif /* THREADSCLASS_HPP_ */