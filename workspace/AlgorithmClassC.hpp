/*********************************************************
* Copyright 
* 
* File    : AlgorithmClassC.hpp
*           Description: Header file for ThreadsClass.hpp,
*           store a algorithm in pipeline.
*           Version : 1.0
*           Date    : 2019-8-13
*           Author  : 
*           ------------- Revision History: --------------         
*********************************************************/

#ifndef  ALGORITHMCLASSC_HPP_
#define  ALGORITHMCLASSC_HPP_

#include "Macro.hpp"

class AlgorithmClassC {

    public:

        /******************************************* * Funcion:*
        * waterMark * Author: *  * Description:
        *    A function used to watermark original picture 
        *    in each thread
        *    Parameters: 
        *       raw_picture_yuv: [char *] yuv of orignal picture
        *       mark_offset: [int] used to get watermark place.
        *    Returns:
        *    void     
        *******************************************************/
        void waterMark(ElemType &raw_picture_yuv, int mark_offset) {
            std::string path = m_WaterMarkPath 
                               + std::to_string(mark_offset) 
                               + m_WaterMarkFormat;

            const char * fopen_path = path.c_str();
            
            FILE * read = nullptr;
            read = fopen(fopen_path, "rb");
            if (read == nullptr) {
                std::cout << "watermark open file failed" << std::endl;
                exit(-1);
            }

            ElemType watermark_yuv = new char[WATERMARK_LENGTH];
            fread(watermark_yuv, sizeof(char), WATERMARK_LENGTH, read);
            
            /* watermark picture's y,u and v component */
            ElemType watermark_yuv_y = watermark_yuv;
            ElemType watermark_yuv_u = watermark_yuv_y 
                                       + WATERMARK_WIDTH * WATERMARK_HEIGHT;

            ElemType watermark_yuv_v = watermark_yuv_u 
                                       + WATERMARK_WIDTH * WATERMARK_HEIGHT / 4;

            /* original picture's y,u and v component */
            ElemType raw_picture_yuv_y = raw_picture_yuv
                                         + (mark_offset - 1) 
                                         * WATERMARK_HEIGHT * PICTURE_WIDTH;  
         
            ElemType raw_picture_yuv_u = raw_picture_yuv_y 
                                         + PICTURE_WIDTH * PICTURE_HEIGHT; 

            ElemType raw_picture_yuv_v = raw_picture_yuv_u 
                                         + PICTURE_WIDTH * PICTURE_HEIGHT / 4;

            /* use memory copy to watermark */
            for (int i = 0; i < WATERMARK_HEIGHT; i++) { 
                
                memcpy(raw_picture_yuv_y, watermark_yuv_y, WATERMARK_WIDTH);
                memcpy(raw_picture_yuv_u, watermark_yuv_u, WATERMARK_WIDTH / 2);
                memcpy(raw_picture_yuv_v, watermark_yuv_v, WATERMARK_WIDTH / 2);

                watermark_yuv_y += WATERMARK_WIDTH;
                watermark_yuv_u += WATERMARK_WIDTH / 2;
                watermark_yuv_v += WATERMARK_WIDTH / 2;

                raw_picture_yuv_y += PICTURE_WIDTH;
                raw_picture_yuv_u += PICTURE_WIDTH / 2;
                raw_picture_yuv_v += PICTURE_WIDTH / 2;
            }
            fclose(read);
        } 

    private:
        /* path to watermark picture */                 
        std::string m_WaterMarkFormat = "mark.yuv";         
        std::string m_WaterMarkPath = ".../watermark/thread"; 

};

#endif /* ALGORITHMCLASSC_HPP_ */

