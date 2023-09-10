#include "my_img_process.h"


// 析构函数
my_img_process::~my_img_process()
{
    std::cout<<"my_img_process has been delect!" <<std::endl;
}


/**
 * @brief my_img_process::calti_vec_Interpol
 * @param src
 * @param x
 * @param y
 * @return 目标像素点rgb
 * 放大时将会根据周边像素点值按权重填充像素点
 * 缩小时将会均匀舍弃部分像素点，依然根据周边像素点值按权重填充像素点
 * 双线性插值算法
 */
cv::Vec3b my_img_process::calti_vec_Interpol(const cv::Mat src, double x, double y)
{
    // 向下取整
    int x1 = static_cast<int>(x);
    int y1 = static_cast<int>(y);

    // 向上取整
    int x2 = x1 + 1;
    int y2 = y1 + 1;


    // 限幅，边界点以外的像素点取值为边界值
    if (x2 >= src.cols) {
        x2 = x1;
    }
    if (y2 >= src.rows) {
        y2 = y1;
    }


    // 距离（权重）, 1 - alpha 即为相对另一个点的权重系数
    double alpha = x - x1;
    double beta  = y - y1;

    // 获得目标点四周 p00 p01 p10 p12 四个像素点的rgb值
    cv::Vec3b Q11 = src.at<cv::Vec3b>(y1, x1);
    cv::Vec3b Q12 = src.at<cv::Vec3b>(y1, x2);
    cv::Vec3b Q21 = src.at<cv::Vec3b>(y2, x1);
    cv::Vec3b Q22 = src.at<cv::Vec3b>(y2, x2);


    // 双线性插值算法计算目标点像素点rgb值
    cv::Vec3b result;
    for(int i = 0 ; i < 3 ; i++) // 循环计算rgb三个通道值
    {
        double interpolatedValue = (1 - alpha) * (1 - beta) * Q11[i] + alpha * (1 - beta) * Q12[i] +
                               (1 - alpha) * beta * Q21[i] + alpha * beta * Q22[i];
        result[i] = static_cast<uchar>(interpolatedValue);
    }


    return result;
}


/**
 * @brief my_img_process::customConvolve
 * @param inputImage
 * @param kernel
 * @return 处理后的图片
 * 卷积处理函数
 */
cv::Mat my_img_process::customConvolve(const cv::Mat& inputImage, const cv::Mat& kernel)
{
    cv::Mat result = cv::Mat::zeros(inputImage.size(), CV_32F);

    int kernelRadiusX = (kernel.size[0] - 1) / 2;
    int kernelRadiusY = (kernel.size[1] - 1) / 2;

    for (int y = kernelRadiusY; y < inputImage.rows - kernelRadiusY; ++y) {
        for (int x = kernelRadiusX; x < inputImage.cols - kernelRadiusX; ++x) {
            float sum = 0.0f;

            for (int j = -kernelRadiusY; j <= kernelRadiusY; ++j) {
                for (int i = -kernelRadiusX; i <= kernelRadiusX; ++i) {
                    float pixel = inputImage.at<uchar>(y + j, x + i);
                    float kernelValue = kernel.at<float>(j + kernelRadiusY, i + kernelRadiusX);
                    sum += pixel * kernelValue;
                }
            }

            result.at<float>(y, x) = sum;
        }
    }

    return result;
}


/**
 * @brief my_img_process::my_get2dtran
 * @param src
 * @param seta_x
 * @param seta_y
 * @param M [2x3] 包含平移、仿射变换
 * @param size
 */
void my_img_process::my_get2dMatrix(cv::Mat src,double seta_x, double seta_y,cv::Mat &M,cv::Size &size)
{
    // 仿射变换后图片大小
    size.width =src.cols +  src.rows*fabs(tan(seta_x*M_PI/180));
    size.height =src.rows + src.cols *fabs(tan(seta_y*M_PI/180));

    double offset_x=0,offset_y=0; // x，y偏移量

    // 平移矫正量计算
    if(seta_x >= 0) offset_x =  -src.rows*tan(seta_x*M_PI/180);// 图片向左变换，向右平移矫正

    if(seta_y >= 0) offset_y = -src.cols*tan(seta_y*M_PI/180);// 图片向左变换，向右平移矫正

    // 整合变换矩阵
    M = (cv::Mat_<double>(3, 3) << 1, tan(seta_x*M_PI/180), offset_x, tan(seta_y*M_PI/180), 1,offset_y,0,0,1);
}

/**
 * @brief my_img_process::my_get3dMatrix
 * @param src
 * @param dst
 * @param srcPoints
 * @param dstPoints
 * 生成透视变换的变换矩阵(3*3)
 * 详细推导过程https://www.bilibili.com/video/BV1HD4y197nW/?spm_id_from=333.337.search-card.all.click&vd_source=7f619786fbbb795d5c20f0d84b1a1066
 */
void my_img_process::my_get3dMatrix(const cv::Mat &src, std::vector<cv::Point2f> srcPoints, std::vector<cv::Point2f> dstPoints,cv::Mat &M)
{
    // 计算透视变换矩阵
//    M = cv::getPerspectiveTransform(srcPoints, dstPoints);
        // 检查输入点的数量是否正确
        if (srcPoints.size() != 4 || dstPoints.size() != 4) {
            std::cerr << "Error: Four source points and four destination points are required for perspective transform." << std::endl;
            return;
        }

        // 构建矩阵A和矩阵B
        // 详细推导：https://www.bilibili.com/video/BV1HD4y197nW/?spm_id_from=333.337.search-card.all.click&vd_source=7f619786fbbb795d5c20f0d84b1a1066
        cv::Mat A(8, 8, CV_64F, cv::Scalar(0));
        cv::Mat B(8, 1, CV_64F);
        cv::Mat X(8, 1, CV_64F);
        cv::Mat C(9, 1, CV_64F);

        for (int i = 0; i < 4; i++) {
            double x = srcPoints[i].x;
            double y = srcPoints[i].y;
            double u = dstPoints[i].x;
            double v = dstPoints[i].y;

            A.at<double>(i * 2, 0) = x;
            A.at<double>(i * 2, 1) = y;
            A.at<double>(i * 2, 2) = 1;
            A.at<double>(i * 2, 6) = -x * u;
            A.at<double>(i * 2, 7) = -y * u;

            A.at<double>(i * 2 + 1, 3) = x;
            A.at<double>(i * 2 + 1, 4) = y;
            A.at<double>(i * 2 + 1, 5) = 1;
            A.at<double>(i * 2 + 1, 6) = -x * v;
            A.at<double>(i * 2 + 1, 7) = -y * v;

            B.at<double>(i * 2, 0) = u;
            B.at<double>(i * 2 + 1, 0) = v;
        }


        // 解线性方程组，获得透视变换矩阵
        cv::solve(A, B, X, cv::DECOMP_NORMAL);

        X.copyTo(cv::Mat(C,cv::Rect(0,0,1,8)));
        C.ptr<double>(8)[0] = 1.0;
        C = C.reshape(0,3);
        M = C.clone();

}

/**
 * @brief my_img_process::my_customEqualizeHist
 * 直方图均衡函数，让每个灰度级拥有相同的像素个数
 * 将每个灰度级的像素个数归一化然后*255生成映射表，并且将原先的像素信息重新映射
 */
void my_img_process::my_customEqualizeHist(const cv::Mat& src, cv::Mat& dst)
{
    int Dm=256; // 灰度级
    cv::Mat hist; // 灰度信息

    int A0 = src.total();
    // 计算灰度信直方图(概率密度函数)
    my_calcHist(src,hist); // hist：0-255灰度级别分别对应的像素数量


    // 累积分布函数（Cumulative Distribution Function，CDF）
    //CDF: cumulativeHist(i) 表示在灰度级别 i 及其之前的灰度级别中像素的累积数量

    cv::Mat cumulativeHist(Dm, 1, CV_32F);

    cumulativeHist.at<float>(0) = hist.at<float>(0);
    for (int i = 1; i < Dm; ++i) { // 积分
        cumulativeHist.at<float>(i) = cumulativeHist.at<float>(i - 1) + hist.at<float>(i);
    }

    // 概率分布函数(归一化)
    cumulativeHist /= A0;// src.total()：像素总数

    // 将概率分布函数拓展到0-255区间,即将灰度直方图向两边拉伸,使灰度级更均匀
    cumulativeHist *= 255.0;


    dst = src.clone();
    // 遍历全图像素点
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            // 获取当前像素点原始灰度值
            int intensity = src.at<uchar>(y, x);
            // 重新映射
            dst.at<uchar>(y, x) = cumulativeHist.at<float>(intensity);
        }
    }
}

/**
 * @brief my_img_process::my_rgb2gray
 * @param src
 * @return 灰度图
 *
 * rgb转灰度图
 */
cv::Mat my_img_process::my_rgb2gray(const cv::Mat& src)
{
    cv::Mat grayImage(src.rows, src.cols, CV_8UC1);

    if (src.empty()) {
            std::cerr << "Error: Failed to load color image." << std::endl;
            return grayImage;
        }
    // 创建灰度图像

    // 转换为灰度图像
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            cv::Vec3b pixel = src.at<cv::Vec3b>(y, x);
            // 计算灰度值
            // 按rgb对亮度的贡献，进行权重分配
            uchar grayValue = static_cast<uchar>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
            grayImage.at<uchar>(y, x) = grayValue;
        }
    }
    return grayImage;
}

/**
 * @brief my_img_process::my_warpAffine
 * @param src
 * @param dst
 * @param M
 * @param size
 * @param way
 * 矩阵变换运算函数
 * 为保证透视变换时能完整预览图像，先将图片缩小一半，在进行变换
 */
void my_img_process::my_warpAffine(cv::Mat src, cv::Mat &dst, cv::Mat M,cv::Size size,int way)
{
    // 获取图像的宽度和高度
    int width = src.cols;
    int height = src.rows;

    // 创建一个黑色背景板
    cv::Mat bg_img(height, width, src.type(), cv::Scalar(0, 0, 0));

    // 获取图像的宽度和高度
    int new_width = src.cols/2;
    int new_height = src.rows/2;

    // 计算在背景图像中放置原始图像的位置
    int x = (bg_img.cols - new_width) / 2;
    int y = (bg_img.rows - new_height) / 2;

    cv::Mat resizedImage;
    cv::resize(src, resizedImage, cv::Size(new_width, new_height));

    // 将缩小后的图像复制到背景图像的中心位置
    resizedImage.copyTo(bg_img(cv::Rect(x, y, new_width, new_height)));


    // 创建输出图像
    cv::Mat dstImage(size, src.type());

    for(int y=0;y< size.height;y++)
    {
        for(int x = 0;x< size.width;x++)
        {
            // 原图坐标矩阵
            cv::Mat inputPoint = (cv::Mat_<double>(3, 1) << x, y, 1);
            // 变换后坐标矩阵
            cv::Mat outputPoint = M * inputPoint;


//            outputPoint /= outputPoint.at<double>(2, 0); //

            // 提取输出像素坐标的x和y值
            double newX = outputPoint.at<double>(0, 0);
            double newY = outputPoint.at<double>(1, 0);

            // 检查新位置是否在图像范围内
           if (newX >= 0 && newX < width && newY >= 0 && newY < height)
           {
             // 使用双线性插值计算新像素的值
             dstImage.at<cv::Vec3b>(y, x) = calti_vec_Interpol(bg_img, newX, newY);
           }
         }
    }
    dst = dstImage.clone();
}
void my_img_process::my_calcHist(const cv::Mat& src,cv::Mat& hist)
{
    // 灰度级别的数量
    int histSize = 256;

    // 初始化直方图
    hist = cv::Mat::zeros(histSize, 1, CV_32FC1);
    // 遍历图像像素，统计每个灰度级别的频率
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            int pixelValue = static_cast<int>(src.at<uchar>(y, x));
            hist.at<float>(pixelValue) += 1.0;
        }
    }
}

void my_img_process::my_medianBlur(const cv::Mat& src, cv::Mat& dst, int windowSize)
{
    // 创建一个与原始图像相同大小的输出图像
    cv::Mat filteredImage = src.clone();
    int halfWindowSize = windowSize / 2;

        for (int y = halfWindowSize; y < src.rows - halfWindowSize; ++y) {
            for (int x = halfWindowSize; x < src.cols - halfWindowSize; ++x) {
                // 提取窗口内的像素值
                std::vector<uchar> windowPixels;

                for (int j = -halfWindowSize; j <= halfWindowSize; ++j) {
                    for (int i = -halfWindowSize; i <= halfWindowSize; ++i) {
                        windowPixels.push_back(src.at<uchar>(y + j, x + i));
                    }
                }

                // 对窗口内的像素值进行排序
                std::sort(windowPixels.begin(), windowPixels.end());

                // 计算中值并赋值给输出图像
                filteredImage.at<uchar>(y, x) = windowPixels[windowPixels.size() / 2];
            }
        }

    dst = filteredImage.clone();
}

void my_img_process::my_sharpen(const cv::Mat& src,cv::Mat& dst,const std::string& operatorType)
{
    cv::Mat sharpened = src.clone();
    cv::Mat kernel;

    if (operatorType == "robert") {
        // Robert算子
        kernel = (cv::Mat_<float>(2, 2) << -1, 0, 0, 1);
    } else if (operatorType == "sobel") {
        // Sobel算子
        kernel = (cv::Mat_<float>(3, 3) << 2, 1, 0, 1, 0, -1, 0, -1, -2);
    } else if (operatorType == "laplace") {
        // Laplace算子
        kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);
    } else {
        std::cerr << "Error: Unsupported operator type." << std::endl;
        return;
    }

    // 进行卷积操作以实现锐化
//    cv::filter2D(src, sharpened, -1, kernel);
    sharpened = customConvolve(src,kernel);


    dst = sharpened.clone();
}

/**
 * @brief my_img_process::my_resize
 * @param src 原图片
 * @param dst 变换后图片
 * @param center 变换中心
 * @param scale 变量比例
 * @param way 算法类型
 */
void my_img_process::my_resize(cv::Mat src, cv::Mat &dst,double scale,int way)
{
    if(way == 1) // 1 ： 双线性插值算法
    {
        // 计算缩放后的图像大小
        int newWidth = static_cast<int>(src.cols * scale);
        int newHeight = static_cast<int>(src.rows * scale);

        // 创建目标图像
        cv::Mat dstImage(newHeight, newWidth, src.type());

        // 遍历图片像素点，由双线性插值算法计算像素，补全目标图像
        for(int y = 0;y < newHeight; y++) // 计算R1
        {
            for(int x = 0;x < newWidth; x++) // 计算R2
            {
                // 等效到原图比例下的坐标
                double src_x = x/scale;
                double src_y = y/scale;
                dstImage.at<cv::Vec3b>(y, x) = calti_vec_Interpol(src, src_x, src_y);
            }
        }
        dst = dstImage.clone();
    }
}

/**
 * @brief my_mat2pixmap
 * @param src
 * @param pixmap
 * 将mat格式图片转化为qpixmap格式图片
 */
void my_img_process::my_mat2pixmap(cv::InputArray src, QPixmap *pixmap)
{
    cv::Mat dst;
    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(src,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    *pixmap = QPixmap::fromImage(image);
}

/**
 * @brief my_img_process::my_rotale
 * @param src
 * @param angle
 * 无缺失旋转函数
 * 对旋转的进行改进，由于图形是一个矩形，旋转后的新图像的形状是一个原图像的外接矩形
 * 因此需要重新计算出旋转后的图形的宽和高
 */
void my_img_process::my_rotale(cv::Mat src,cv::Mat *dst,double angle)
{
    int width = src.cols;
    int height = src.rows;


    double radian= angle * CV_PI / 180.;//角度转换为弧度
    double width_rotate = fabs(width*cos(radian))+fabs(height*sin(radian)); // 旋转后，向外拓展，新的图片宽度
    double height_rotate= fabs(width*sin(radian)) + fabs(height*cos(radian)); // 旋转后，向外拓展，新的图片高度

    //旋转中心 原图像中心点
    cv::Point2f center((float)width / 2.0, (float)height/ 2.0);
    //旋转矩阵
    cv::Mat m1 = cv::getRotationMatrix2D(center, angle, 1.0);


    // 修正中心偏移
    // 变换矩阵的中心点相当于平移一样 原图像的中心点与新图像的中心点的相对位置
    m1.at<double>(0, 2) += (width_rotate - width) / 2.;
    m1.at<double>(1, 2) += (height_rotate - height) / 2.;

    // 适应不同图片格式
    if (src.channels() == 1)
    {
        cv::warpAffine(src, *dst, m1,cv::Size(width_rotate,height_rotate), cv::INTER_LINEAR, 0, cv::Scalar(255));
    }
    else if (src.channels() == 3)
    {
        cv::warpAffine(src, *dst, m1,cv::Size(width_rotate, height_rotate), cv::INTER_LINEAR, 0, cv::Scalar(255, 255, 255));
    }

}


