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

#ifndef IMAGECPPNGENERATORNETWORK_H
#define IMAGECPPNGENERATORNETWORK_H

#include <qnn-global.h>

#include <network/abstractneuralnetwork.h>

/*!
 * \brief The ImageCPPNGeneratorNetwork class is a special network that do not generate output but creates images out of a gene.
 *
 * The image is encoded using a "compositional pattern producing networks". For more information see:
 *
 * KennethO. Stanley. 2007. Compositional pattern producing networks: A novel abstraction of development. Genetic Programming and Evolvable Machines 8, 2 (2007), 131–162.
 *
 * Although the ImageCPPNGeneratorNetwork contains a neural network, it is not a neural network in the sense that it takes an input and transforms it into an output.
 * It is a special wrapper to create images using QNeuralNetwork.
 */

class QNNSHARED_EXPORT ImageCPPNGeneratorNetwork : public AbstractNeuralNetwork
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
         * \brief The minimum number of hidden neurons
         *
         * Must be zero or greater.
         */
        qint32 min_size;

        /*!
         * \brief The minimum number of hidden neurons
         *
         * Must be zero or greater. Must be equal or greater then min_size.
         */
        qint32 max_size;

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
            min_size(0),
            max_size(10),
            image_path("./qnn-image-generators-CPPN.png")
        {
        }
    };

    /*!
     * \brief Constructor
     * \param len_input Length of the input
     * \param len_output Length of the output
     * \param config Configuration of the ImageCPPNGeneratorNetwork
     */
    ImageCPPNGeneratorNetwork(qint32 len_input, qint32 len_output, config config = config());


    /*!
     * \brief Destructor
     */
    ~ImageCPPNGeneratorNetwork();

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
    ImageCPPNGeneratorNetwork();

    /*!
     * \brief Overwritten function to initialise the network.
     */
    void _initialise();

    /*!
     * \brief Overwritten method to process input
     *
     * In comparison to other networks the input has no influence on the generated image.
     *
     * \param input Input to process
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

    /*!
     * \brief Applies an activation function to value.
     *
     * CPPN have multiple functions which can be applied to the neuron.
     * The type of function is determined by the gene.
     * With this function the activation function can be applied.
     *
     * Currently the following activation functions are used:
     *  - sinus
     *  - cosinus
     *  - tanh
     *  - identity
     *  - gaussian
     *  - sigmoid
     *
     * \param value The internal value of the neuron
     * \param geneValue The gene value. Will be decoded to an activation function
     * \return value with applied activation function
     */
    double applyFunction(double value, qint32 geneValue);

private:
    /*!
     * \brief The size (in pixel) of the network.
     *
     *  This value is precalculated in the constructor. It equals to width * height.
     */
    config _config;
};
#endif // IMAGECPPNGENERATORNETWORK_H
