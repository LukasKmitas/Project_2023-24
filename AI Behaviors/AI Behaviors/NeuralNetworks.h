#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"
#include <random>

class NeuralNetworks
{
public:
    NeuralNetworks();
    ~NeuralNetworks();

    void draw_text(bool m_horizontal_center, bool m_vertical_center, int m_x, int m_y, const std::string& m_text, const sf::Color& m_color, sf::RenderWindow& m_window, const sf::Texture& m_font_texture);
    void draw_neural_network(sf::RenderWindow& m_window, const sf::Texture& m_fontTexture, const vector_2d& m_neural_network, const vector_3d& m_weights);

    void backPropagation(const std::vector<float>& m_target, std::vector<std::vector<float>>& m_errors, const std::vector<std::vector<float>>& m_neural_network, std::vector<std::vector<std::vector<float>>>& m_weights);
    std::vector<float> forwardPropagation(bool m_state, const std::vector<float>& m_inputs, std::vector<std::vector<float>>& m_neural_network, const std::vector<std::vector<std::vector<float>>>& m_weights);

    std::array<int, 2> getHiddenNeurons() const
    {
        return HIDDEN_NEURONS;
    }

    std::array<int, 3> getBiasNeurons() const 
    {
        return BIAS_NEURONS;
    }

    const int INPUT_NEURONS = 2;
    const int OUTPUT_NEURONS = 3;

private:

    sf::CircleShape neuron_shape;

    void setupNeuralCircle();

    float activationFunction(bool m_derivative, float m_input);

    float get_min_element(const std::vector<float>& m_vector);
    float get_min_element(const std::vector<std::vector<float>>& m_vector);
    float get_min_element(const std::vector<std::vector<std::vector<float>>>& m_vector);

    float get_max_element(const std::vector<float>& m_vector);
    float get_max_element(const std::vector<std::vector<float>>& m_vector);
    float get_max_element(const std::vector<std::vector<std::vector<float>>>& m_vector);

    const float LEARNING_RATE = 0.03f;

    std::array<int, 2> HIDDEN_NEURONS;
    std::array<int, 3> BIAS_NEURONS;

    //const std::array<int, 2> HIDDEN_NEURONS = { 3, 3 };
    //const std::array<int, 1 + HIDDEN_NEURONS.size()> BIAS_NEURONS = { 1, 0, 0 };

};