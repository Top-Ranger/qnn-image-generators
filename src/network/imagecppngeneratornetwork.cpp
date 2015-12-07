#include "imagecppngeneratornetwork.h"

#include <network/lengthchanginggene.h>
#include <network/commonnetworkfunctions.h>
#include <network/networktoxml.h>

#include <QtCore/qmath.h>
#include <QImage>

// GENE ENCODING: function, (activated, weight)^4, (avtivated, weight)^n, (activated, weight)^3

using CommonNetworkFunctions::sigmoid;
using CommonNetworkFunctions::weight;
using NetworkToXML::writeConfigStart;
using NetworkToXML::writeConfigEnd;
using NetworkToXML::writeConfigNeuron;

ImageCPPNGeneratorNetwork::ImageCPPNGeneratorNetwork(qint32 len_input, qint32 len_output, config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config)
{
    if(Q_UNLIKELY(_config.max_size <= 0))
    {
        QNN_FATAL_MSG("Max size must be greater than 0");
    }
    if(Q_UNLIKELY(_config.min_size <= 0))
    {
        QNN_FATAL_MSG("Min size must be greater than 0");
    }
    if(Q_UNLIKELY(_config.min_size > _config.max_size))
    {
        QNN_FATAL_MSG("Min size must not be greater than max size");
    }
}

ImageCPPNGeneratorNetwork::~ImageCPPNGeneratorNetwork()
{
}

GenericGene *ImageCPPNGeneratorNetwork::getRandomGene()
{
    LengthChangingGene::config config;
    config.min_length = _config.min_size;
    config.max_length = _config.max_size;
    qint32 lengh = _config.min_size + qrand()%(_config.max_size - _config.min_size) + 3; // 3 Output neurons

    return new LengthChangingGene(lengh, 1 + 4*2 + _config.max_size * 2 + 3*2);
}

AbstractNeuralNetwork *ImageCPPNGeneratorNetwork::createConfigCopy()
{
    return new ImageCPPNGeneratorNetwork(_len_input, _len_output, _config);
}

ImageCPPNGeneratorNetwork::ImageCPPNGeneratorNetwork() :
    AbstractNeuralNetwork(),
    _config()
{
}

void ImageCPPNGeneratorNetwork::_initialise()
{
    if(_gene->segments()[0].size() < (1 + 4*2 + _gene->segments().size() * 2 + 3*2))
    {
        QNN_FATAL_MSG("Segment size do not fit");
    }
}

void ImageCPPNGeneratorNetwork::_processInput(QList<double> input)
{
    // TODO implement
    Q_UNUSED(input);
    QImage image(_config.width, _config.height, QImage::Format_RGB32);
    qint32 x_center = _config.width / 2;
    qint32 y_center = _config.height / 2;
    double max_distance = qSqrt((qreal) _config.width * (qreal) _config.height)/2;
    qint32 neurons = 4 + _gene->segments().size();

    for(qint32 width = 0; width < _config.width; ++width)
    {
        for(qint32 height = 0; height < _config.height; ++height)
        {
            double distance_to_center = qSqrt(qAbs(width - x_center) * qAbs(height - y_center)) / max_distance;
            double network[neurons];
            network[0] = 1.0;
            network[1] = (qreal) width / (qreal) _config.width;
            network[2] = (qreal) height / (qreal) _config.height;
            network[3] = distance_to_center;
            for(qint32 neuron = 0; neuron < _gene->segments().size(); ++neuron)
            {
                double value = 0.0;
                for(qint32 input = 0; input < neuron + 4; ++input)
                {
                    if(_gene->segments()[neuron][1 + (2 * input)] % 2)
                    {
                        value += network[input] * weight(_gene->segments()[neuron][1 + (2 * input) + 1], 1);
                    }
                }
                network[4 + neuron] = applyFunction(value, _gene->segments()[neuron][0]);
            }
            qint32 r = qFloor(qBound(0.0, network[neurons - 3] * 255, 255.0));
            qint32 g = qFloor(qBound(0.0, network[neurons - 2] * 255, 255.0));
            qint32 b = qFloor(qBound(0.0, network[neurons - 1] * 255, 255.0));
            QRgb pixel = qRgb(r, g, b);
            image.setPixel(width, height, pixel);
        }
    }
}

double ImageCPPNGeneratorNetwork::_getNeuronOutput(qint32 i)
{
    Q_UNUSED(i);
    QNN_WARNING_MSG("This network does not generate usefull output");
    return 0.0;
}

bool ImageCPPNGeneratorNetwork::_saveNetworkConfig(QXmlStreamWriter *stream)
{
    // TODO implement
    QMap<QString, QVariant> config_network;
    config_network["width"] = _config.width;
    config_network["height"] = _config.height;
    config_network["min hidden neurons"] = _config.min_size;
    config_network["max hidden neurons"] = _config.max_size;
    writeConfigStart("ImageCPPNGeneratorNetwork", config_network, stream);

    for(qint32 neuron = 0; neuron < _gene->segments().size(); ++neuron)
    {
        QMap<QString, QVariant> config_neuron;
        QMap<qint32, double> connection_neuron;

        QString function;
        switch(_gene->segments()[neuron][0]%5)
        {
        case 0:
            function = "cosinus";
            break;
        case 1:
            function = "sinus";
            break;
        case 2:
            function = "tangens";
            break;
        case 3:
            function = "identity";
            break;
        case 4:
            function = "sigmoid";
            break;
        default:
            function = "<unknown error>";
        }

        config_neuron["function"] = function;

        for(qint32 input = 0; input < neuron + 4; ++input)
        {
            if(_gene->segments()[neuron][1 + (2 * input)] % 2)
            {
                if(input <= 3)
                {
                    config_neuron[QString("input %1").arg(input)] = weight(_gene->segments()[neuron][1 + (2 * input) + 1], 1);
                }
                else
                {
                    connection_neuron[input - 4] = weight(_gene->segments()[neuron][1 + (2 * input) + 1], 1);
                }
            }
        }

        writeConfigNeuron(neuron - 4, config_neuron, connection_neuron, stream);
    }
    writeConfigEnd(stream);
    return true;
}

double ImageCPPNGeneratorNetwork::applyFunction(double value, qint32 geneValue)
{
    switch(geneValue%5)
    {
    case 0:
        return qCos(value);
        break;
    case 1:
        return qSin(value);
        break;
    case 2:
        return qTan(value);
        break;
    case 3:
        // Idendity
        return value;
        break;
    case 4:
        return sigmoid(value);
        break;
    default:
        QNN_CRITICAL_MSG("Unknown function" << geneValue%5);
        return value;
    }
}
