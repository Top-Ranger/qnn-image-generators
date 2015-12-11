#ifndef IMAGEDIRECTENCODINGGENERATORNETWORK_H
#define IMAGEDIRECTENCODINGGENERATORNETWORK_H

#include <qnn-global.h>

#include <network/abstractneuralnetwork.h>

/*!
 * \brief The ImageDirectEncodingGeneratorNetwork class is a special network that do not generate output but creates images out of a gene.
 *
 * The image is encoded into three values: red, green, blue. Each pixel of the image is encoded in an own gene segment.
 *
 * The ImageDirectEncodingGeneratorNetwork is not a neural network. It is a special wrapper to create images using QNeuralNetwork.
 */

class QNNSHARED_EXPORT ImageDirectEncodingGeneratorNetwork : public AbstractNeuralNetwork
{
public:
    /*!
     * \brief This struct contains all configuration option of GasNets
     */
    struct config {

        /*!
         * \brief The width of the image in pixel
         */
        qint32 width;
        /*!
         * \brief The height of the image in pixel
         */
        qint32 height;

        /*!
         * \brief The path where the resulting image is saved to
         */
        QString image_path;

        /*!
         * \brief Constructor for standard values
         */
        config() :
            width(256),
            height(256),
            image_path("./qnn-image-generators-direct-encoding.png")
        {
        }
    };

    /*!
     * \brief Constructor
     * \param len_input Length of the input
     * \param len_output Length of the output
     * \param config Configuration of the ImageDirectEncodingGeneratorNetwork
     */
    ImageDirectEncodingGeneratorNetwork(qint32 len_input, qint32 len_output, config config = config());

    /*!
     * \brief Destructor
     */
    ~ImageDirectEncodingGeneratorNetwork();

    /*!
     * \brief Returns a random gene which may be used with the current network configuration
     * \return Random gene. The caller must delete the gene
     */
    GenericGene *getRandomGene();

    /*!
     * \brief Creates a uninitialised copy of the network
     * \return Copy of the network. The caller must delete the gene
     */
    AbstractNeuralNetwork *createConfigCopy();

protected:
    /*!
     * \brief Empty constructor
     *
     * This constructor may be useful for subclasses
     */
    ImageDirectEncodingGeneratorNetwork();


    /*!
     * \brief Overwritten function to initialise the network.
     */
    void _initialise();

    /*!
     * \brief Overwritten method to process input.
     *
     * In comparison to other networks the input has no influence on the generated image.
     *
     * \param input Input to process. Will be ignored
     */
    void _processInput(QList<double> input);

    /*!
     * \brief Overwritten function to get output
     *
     * In this network the getNeuronOutput(qint32 i) function should never be called.
     *
     * \param i Number of neuron (0 <= i < len_output)
     * \return Output of neuron i. Always 0.o
     */
    double _getNeuronOutput(qint32 i);

    /*!
     * \brief Overwritten function to save network config
     * \param stream Stream to save config to. Stream is guaranteed to be a valid pointer
     * \return True if save is successfull
     */
    bool _saveNetworkConfig(QXmlStreamWriter *stream);

private:

    /*!
     * \brief The configuration of the network
     */
    config _config;

    /*!
     * \brief The size (in pixel) of the network.
     *
     *  This value is precalculated in the constructor. It equals to width * height.
     */
    qint32 _size;
};

#endif // IMAGEDIRECTENCODINGGENERATORNETWORK_H
