#ifndef IMAGECPPNGENERATORNETWORK_H
#define IMAGECPPNGENERATORNETWORK_H

#include <qnn-global.h>

#include <network/abstractneuralnetwork.h>

class QNNSHARED_EXPORT ImageCPPNGeneratorNetwork : public AbstractNeuralNetwork
{
public:
    struct config {
        qint32 width;
        qint32 height;
        qint32 min_size;
        qint32 max_size;
        QString image_path;
        config() :
            width(256),
            height(256),
            min_size(0),
            max_size(10),
            image_path("./qnn-image-generators-CPPN.png")
        {
        }
    };

    ImageCPPNGeneratorNetwork(qint32 len_input, qint32 len_output, config config = config());
    ~ImageCPPNGeneratorNetwork();
    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    ImageCPPNGeneratorNetwork();
    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(qint32 i);
    bool _saveNetworkConfig(QXmlStreamWriter *stream);

    double applyFunction(double value, qint32 geneValue);

private:
    config _config;
};
#endif // IMAGECPPNGENERATORNETWORK_H
