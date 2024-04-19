#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include "Global.h"
#include "GameState.h"

class NeuralNetworks
{
public:
    NeuralNetworks();
    ~NeuralNetworks();

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);

    void drawNeuralPerceptron(sf::RenderWindow& m_window, const vector_2d& m_neuralNetwork, const vector_3d& m_weights);
    void drawWeightText(int m_x, int m_y, const std::string& m_text, const sf::Color& m_color, sf::RenderWindow& m_window);

    void backPropagation(const std::vector<float>& m_target, std::vector<std::vector<float>>& m_errors, const std::vector<std::vector<float>>& m_neuralNetwork, std::vector<std::vector<std::vector<float>>>& m_weights);
    std::vector<float> forwardPropagation(bool m_state, const std::vector<float>& m_inputs, std::vector<std::vector<float>>& m_neuralNetwork, const std::vector<std::vector<std::vector<float>>>& m_weights);

    void addInput(float m_dot_x, float m_dot_y);
    void addTargetOutput(float m_r, float m_g, float m_b);

    void goToMainMenu(sf::Vector2i m_mousePosition, GameState& m_gameState);

    std::array<int, 2> getHiddenNeurons() const;
    std::array<int, 3> getBiasNeurons() const;

    const std::vector<std::vector<float>>& getInputs() const;
    const std::vector<std::vector<float>>& getTargetOutputs() const;

    const int INPUT_NEURONS = 2;
    const int OUTPUT_NEURONS = 3;

private:

    void initNeuralCircle();
    void initBackButton();
    void initMouseDotCircle();

    float activationFunction(bool m_derivative, float m_input);
    float activationFunctionSigmoid(bool m_derivative, float m_input);

    float get_min_element(const std::vector<float>& m_vector);
    float get_min_element(const std::vector<std::vector<float>>& m_vector);
    float get_min_element(const std::vector<std::vector<std::vector<float>>>& m_vector);

    float get_max_element(const std::vector<float>& m_vector);
    float get_max_element(const std::vector<std::vector<float>>& m_vector);
    float get_max_element(const std::vector<std::vector<std::vector<float>>>& m_vector);

    sf::CircleShape m_neuronCircle;
    sf::CircleShape m_mouseDotCircle;

    sf::RectangleShape m_toGoBackButton;
    sf::Texture m_buttonTexture;

    sf::Font m_font;
    sf::Text m_toGoBackText;
    sf::Text m_neuralWeightText;

    const float LEARNING_RATE = 0.03f;

    std::vector<std::vector<float>> m_inputs;
    std::vector<std::vector<float>> m_target_outputs;

    std::array<int, 2> HIDDEN_NEURONS;
    std::array<int, 3> BIAS_NEURONS;

};