#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QObject>
#include <QString>

#include <opencv2/opencv.hpp>
using namespace cv;

class pluginInterface{
public:
    virtual ~pluginInterface(){}

    virtual QString name() = 0;
    virtual void edit(const Mat &input, Mat &output) = 0;
};

#define PLUGIN_INTERFACE_IID "com.pahul.plugins"
Q_DECLARE_INTERFACE(pluginInterface, PLUGIN_INTERFACE_IID)

#endif // PLUGIN_INTERFACE_H
