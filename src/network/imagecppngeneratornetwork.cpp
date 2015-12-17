/*
 * Copyright (C) 2015 Marcus Soll
 * This file is part of qnn-image-generators.
 *
 * qnn-image-generators is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qnn-image-generators is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with qnn-image-generators.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "imagecppngeneratornetwork.h"

#include <network/lengthchanginggene.h>
#include <network/commonnetworkfunctions.h>
#include <network/networktoxml.h>

#include <QtCore/qmath.h>
#include <QImage>

// GENE ENCODING: function, (activated, weight)^4, (avtivated, weight)^n, (activated, weight)^3

using CommonNetworkFunctions::sigmoid;
using CommonNetworkFunctions::weight;
using CommonNetworkFunctions::floatFromGeneInput;
using NetworkToXML::writeConfigStart;
using NetworkToXML::writeConfigEnd;
using NetworkToXML::writeConfigNeuron;

ImageCPPNGeneratorNetwork::ImageCPPNGeneratorNetwork(qint32 len_input, qint32 len_output, config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config)
{
    if(Q_UNLIKELY(_config.max_size < 0))
    {
        QNN_FATAL_MSG("Max size must be greater than 0");
    }
    if(Q_UNLIKELY(_config.min_size < 0))
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
    if(_gene->segments()[0].size() < (1 + 4*2 + _config.max_size * 2 + 3*2))
    {
        QNN_FATAL_MSG("Segment size do not fit");
    }
}

void ImageCPPNGeneratorNetwork::_processInput(QList<double> input)
{
    Q_UNUSED(input);
    QImage image(_config.width, _config.height, QImage::Format_RGB32);
    qint32 x_center = _config.width / 2;
    qint32 y_center = _config.height / 2;
    double max_distance = qSqrt(qPow(_config.width, 2) + qPow(_config.height, 2))/2;
    qint32 neurons = 4 + _gene->segments().size();

    for(qint32 width = 0; width < _config.width; ++width)
    {
        for(qint32 height = 0; height < _config.height; ++height)
        {
            double distance_to_center = qSqrt(qPow(width - x_center, 2) + qPow(height - y_center, 2)) / max_distance;
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
                    if(weight(_gene->segments()[neuron][1 + (2 * input)], 1) > 0)
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

    if(!image.save(_config.image_path))
    {
        QNN_WARNING_MSG(QString("Could not save image to %1").arg(_config.image_path));
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
        switch(qFloor(floatFromGeneInput(_gene->segments()[neuron][0]%5, 5)))
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
        case 5: // 5 is an extreme corner case which should almost never occure
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
    switch(qFloor(floatFromGeneInput(geneValue, 5)))
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
        // Identity between 0,1
        return qBound(0.0, value, 1.0);
        break;
    case 4:
    case 5: // 5 is an extreme corner case which should almost never occure
        return sigmoid(value);
        break;
    default:
        QNN_CRITICAL_MSG("Unknown function" << qFloor(floatFromGeneInput(geneValue, 5)));
        return value;
    }
}
