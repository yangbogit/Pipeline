/**********************************************************
* Copyright (c) 
* File    : PipelineMain.cpp
*           Description: main file.
*           To create and start threads.
*           Version : 1.0
*           Date    : 2019-8-2
*           Author  : 
*           ------------- Revision History: --------------         
**********************************************************/

#include "ThreadsClass.hpp"

/******************************************* **** * Funcion:*
* threadStarter * Author: *  * Description:
*    A function used to start threads.
*    Parameters: 
*    Returns:
*    void     
************************************************************/
void threadStarter() {
    
    /* generate object pointer */
    ReadPictureClass *m_ReadPicture = new ReadPictureClass(THREAD_NUM-THREAD_NUM);
    WritePictureClass *m_WritePicture = new WritePictureClass(THREAD_NUM + 1 );

    ThreadClass *m_ProcessA = new ThreadClass(PROCESS_A_MARKPLACE);
    ThreadClass *m_ProcessB = new ThreadClass(PROCESS_B_MARKPLACE);
    ThreadClass *m_ProcessC = new ThreadClass(PROCESS_C_MARKPLACE);
    
    /* create thread */
    pthread_create(&(m_ReadPicture->m_Thread), NULL, readPictureThread , (void *) m_ReadPicture);
    pthread_create(&(m_ProcessA->m_Thread), NULL, processThread , (void *) m_ProcessA);
    pthread_create(&(m_ProcessB->m_Thread), NULL, processThread , (void *) m_ProcessB);
    pthread_create(&(m_ProcessC->m_Thread), NULL, processThread , (void *) m_ProcessC);
    pthread_create(&(m_WritePicture->m_Thread), NULL, writePictureThread , (void *) m_WritePicture);

    /* wait for thread return */
    pthread_join(m_ReadPicture->m_Thread, NULL);
    pthread_join(m_ProcessA->m_Thread, NULL);
    pthread_join(m_ProcessB->m_Thread, NULL);
    pthread_join(m_ProcessC->m_Thread, NULL);
    pthread_join(m_WritePicture->m_Thread, NULL);
}

int main() 
{
    threadStarter();
}