#ifndef PIPELINE_IMAGEALGORITHM_HPP
#define PIPELINE_IMAGEALGORITHM_HPP

/**
 * 算法类，用于封装各种图像处理算法，各种子类算法通过ImageAlgorithm进行多态调用
 * */

template <typename T>
class ImageAlgorithm
{

public:

    virtual void algorithm(T &image){}

};

#endif //PIPELINE_IMAGEALGORITHM_HPP
