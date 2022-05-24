#ifndef PIPELINE_WATERMARK_HPP
#define PIPELINE_WATERMARK_HPP

#include "ImageAlgorithm.hpp"
#include <opencv2/opencv.hpp>

template <typename T>
class Watermark : public ImageAlgorithm<T>
{
private:
    int watermark;              // 打水印的数字
    std::string watermarkStr;   // 打水印的字符串

public:

    Watermark(int watermark)
    {
        this->watermark = watermark;
        this->watermarkStr = std::to_string(watermark);
    }

    void algorithm(T &image)
    {
        /**
         * img : 编辑的图片
         * text : 要写入的字符串
         * org : 操作的坐标，cv::Point(列， 行)
         * fontFace : 字体类型
         * fontScale : 字体大小
         * color : 字体颜色
         * thickness : 字体厚度
         * */
        cv::putText(image,
                    watermarkStr,
                    cv::Point(image.cols / 3, image.rows / 4 + watermark * 120),
                    cv::FONT_HERSHEY_SIMPLEX,
                    4,
                    cv::Scalar(0, 0, 255),
                    5);
    }
};

#endif //PIPELINE_WATERMARK_HPP
