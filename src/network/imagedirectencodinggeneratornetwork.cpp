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

#include "imagedirectencodinggeneratornetwork.h"

#include <network/networktoxml.h>
#include <network/commonnetworkfunctions.h>

#include <limits>
#include <QMap>
#include <QImage>
#include<QtCore/qmath.h>

using NetworkToXML::writeConfigStart;
using NetworkToXML::writeConfigEnd;
using CommonNetworkFunctions::floatFromGeneInput;

ImageDirectEncodingGeneratorNetwork::ImageDirectEncodingGeneratorNetwork(qint32 len_input, qint32 len_output, config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config),
    _size(0)
{
    if(Q_UNLIKELY(_config.width <= 0))
    {
        QNN_FATAL_MSG("Width must be greater than 0");
    }
    if(Q_UNLIKELY(_config.height <= 0))
    {
        QNN_FATAL_MSG("Width must be greater than 0");
    }
    qint64 size = _config.width * _config.height;
    if(Q_UNLIKELY(size > std::numeric_limits<qint32>::max()))
    {
        QNN_FATAL_MSG("Size gets to huge, decrease width or height");
    }
    size = _size;
}

ImageDirectEncodingGeneratorNetwork::~ImageDirectEncodingGeneratorNetwork()
{
}

GenericGene *ImageDirectEncodingGeneratorNetwork::getRandomGene()
{
    return new GenericGene(_size, 3);
}

AbstractNeuralNetwork *ImageDirectEncodingGeneratorNetwork::createConfigCopy()
{
    return new ImageDirectEncodingGeneratorNetwork(_len_input, _len_output, _config);
}

ImageDirectEncodingGeneratorNetwork::ImageDirectEncodingGeneratorNetwork() :
    AbstractNeuralNetwork(),
    _config(),
    _size(0)
{
}

void ImageDirectEncodingGeneratorNetwork::_initialise()
{
    if(Q_UNLIKELY(_gene->segments().size() != _size))
    {
        QNN_FATAL_MSG("Gene length does not fit");
    }
    if(Q_UNLIKELY(_gene->segments()[0].size() != 3))
    {
        QNN_FATAL_MSG("Gene segment size has to be 3");
    }
}

void ImageDirectEncodingGeneratorNetwork::_processInput(QList<double> input)
{
    Q_UNUSED(input);
    QImage image(_config.width, _config.height, QImage::Format_RGB32);

    for(qint32 height = 0; height < _config.height; ++height)
    {
        for(qint32 width = 0; width < _config.width; ++width)
        {
            qint32 r = qFloor(floatFromGeneInput(_gene->segments()[_config.width * height + width][0], 255));
            qint32 g = qFloor(floatFromGeneInput(_gene->segments()[_config.width * height + width][1], 255));
            qint32 b = qFloor(floatFromGeneInput(_gene->segments()[_config.width * height + width][2], 255));
            QRgb pixel = qRgb(r, g, b);
            image.setPixel(width, height, pixel);
        }
    }

    if(!image.save(_config.image_path))
    {
        QNN_WARNING_MSG(QString("Could not save image to %1").arg(_config.image_path));
    }
}

double ImageDirectEncodingGeneratorNetwork::_getNeuronOutput(qint32 i)
{
    Q_UNUSED(i);
    QNN_WARNING_MSG("This network does not generate usefull output");
    return 0.0;
}

bool ImageDirectEncodingGeneratorNetwork::_saveNetworkConfig(QXmlStreamWriter *stream)
{
    QMap<QString, QVariant> config_network;
    config_network["width"] = _config.width;
    config_network["height"] = _config.height;
    config_network["Save path"] = _config.image_path;
    writeConfigStart("ImageDirectEncodingGeneratorNetwork", config_network, stream);
    writeConfigEnd(stream);
    return true;
}
