#ifndef IMAGEDIRECTENCODINGGENERATORNETWORK_H
#define IMAGEDIRECTENCODINGGENERATORNETWORK_H

#include <qnn-global.h>

#include <network/abstractneuralnetwork.h>

class QNNSHARED_EXPORT ImageDirectEncodingGeneratorNetwork : public AbstractNeuralNetwork
{
public:
    struct config {
        qint32 width;
        qint32 height;
        QString image_path;
        config() :
            width(256),
            height(256),
            image_path("./qnn-image-generators-direct-encoding.png")
        {
        }
    };

    ImageDirectEncodingGeneratorNetwork(qint32 len_input, qint32 len_output, config config = config());
    ~ImageDirectEncodingGeneratorNetwork();
    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    ImageDirectEncodingGeneratorNetwork();
    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(qint32 i);
    bool _saveNetworkConfig(QXmlStreamWriter *stream);

private:
    config _config;
    qint32 _size;
};

#endif // IMAGEDIRECTENCODINGGENERATORNETWORK_H
