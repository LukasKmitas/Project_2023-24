#include "NeuralNetworks.h"

NeuralNetworks::NeuralNetworks()
	: HIDDEN_NEURONS{ 3, 3 }, BIAS_NEURONS{ 1, 0, 0 }
{
	if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
	{
		std::cout << "Error - problem loading font in Level Editor" << std::endl;
	}
	initNeuralCircle();
	initBackButton();
	initMouseDotCircle();
}

NeuralNetworks::~NeuralNetworks()
{
}

void NeuralNetworks::update(sf::Time t_deltaTime)
{
}

/// <summary>
/// renders neural networks stuff
/// </summary>
/// <param name="m_window"></param>
void NeuralNetworks::render(sf::RenderWindow& m_window)
{
	m_window.draw(m_toGoBackButton);
	m_window.draw(m_toGoBackText);
	//m_window.draw(m_neuralWeightText);

	for (unsigned i = 0; i < m_inputs.size(); i++)
	{
		m_mouseDotCircle.setFillColor(sf::Color(round(255 * m_target_outputs[i][0]), round(255 * m_target_outputs[i][1]), round(255 * m_target_outputs[i][2])));
		m_mouseDotCircle.setPosition(SCREEN_HEIGHT * (1 + m_inputs[i][0]), SCREEN_WITDH * m_inputs[i][1]);

		m_window.draw(m_mouseDotCircle);
	}
}

/// <summary>
/// Draws the Neural Perceptrons
/// </summary>
/// <param name="m_window"></param>
/// <param name="m_neuralNetwork"></param>
/// <param name="m_weights"></param>
void NeuralNetworks::drawNeuralPerceptron(sf::RenderWindow& m_window, const vector_2d& m_neuralNetwork, const vector_3d& m_weights)
{
	float m_maxNeuron = std::max(abs(get_max_element(m_neuralNetwork)), abs(get_min_element(m_neuralNetwork)));
	float m_minNeuron = -m_maxNeuron;

	sf::VertexArray m_connectionLine(sf::Quads, 4);

	if (0 <= get_min_element(m_neuralNetwork))
	{
		m_minNeuron = 0;
	}

	for (int i = 0; i < m_neuralNetwork.size(); i++)
	{
		int m_neuronX = (Global::S_WIDTH / 2) * (1 + i) / (1 + m_neuralNetwork.size());

		for (int k = 0; k < m_neuralNetwork[i].size(); k++)
		{
			// Rounding the value of the neuron
			float m_neuronValue = round(m_neuralNetwork[i][k] * pow(10, 2)) / pow(10, 2);

			int m_neuronColor = round(255 * (m_neuralNetwork[i][k] - m_minNeuron) / (m_maxNeuron - m_minNeuron));
			int m_textColor = 255 * (128 > m_neuronColor);
			int m_neuronY = Global::S_HEIGHT * (1 + k) / (1 + m_neuralNetwork[i].size());

			std::ostringstream m_neuronTextStream;
			m_neuronTextStream << std::fixed << std::setprecision(2) << m_neuronValue; // to make it be like "0.02" and not "0.0234324"

			m_neuronCircle.setFillColor(sf::Color(m_neuronColor, m_neuronColor, m_neuronColor));
			m_neuronCircle.setPosition(m_neuronX, m_neuronY);

			if (i < m_neuralNetwork.size() - 1)
			{
				float m_maxWeight = std::max(abs(get_max_element(m_weights[i])), abs(get_min_element(m_weights[i])));
				float m_minWeight = -m_maxWeight;
				int m_biasNeurons = 0;

				if (i < m_neuralNetwork.size() - 2)
				{
					m_biasNeurons = BIAS_NEURONS[1 + i];
				}

				for (int j = m_biasNeurons; j < m_neuralNetwork[1 + i].size(); j++)
				{
					if (0 <= m_weights[i][j - m_biasNeurons][k])
					{
						int m_connectionOpacity = round(255 * m_weights[i][j - m_biasNeurons][k] / m_maxWeight);

						m_connectionLine[0].color = sf::Color(0, 255, 0, m_connectionOpacity);
						m_connectionLine[1].color = sf::Color(0, 255, 0, m_connectionOpacity);
						m_connectionLine[2].color = sf::Color(0, 255, 0, m_connectionOpacity);
						m_connectionLine[3].color = sf::Color(0, 255, 0, m_connectionOpacity);
					}
					else
					{
						int m_connectionOpacity = round(255 * m_weights[i][j - m_biasNeurons][k] / m_minWeight);

						m_connectionLine[0].color = sf::Color(255, 0, 0, m_connectionOpacity);
						m_connectionLine[1].color = sf::Color(255, 0, 0, m_connectionOpacity);
						m_connectionLine[2].color = sf::Color(255, 0, 0, m_connectionOpacity);
						m_connectionLine[3].color = sf::Color(255, 0, 0, m_connectionOpacity);
					}

					int m_previousNeuronX = (Global::S_WIDTH / 2) * (2 + i) / (1 + m_neuralNetwork.size());
					int m_previousNeuronY = Global::S_HEIGHT * (1 + j) / (1 + m_neuralNetwork[1 + i].size());

					m_connectionLine[0].position = sf::Vector2f(m_previousNeuronX, m_previousNeuronY - 0.5f * 2);
					m_connectionLine[1].position = sf::Vector2f(m_neuronX, m_neuronY - 0.5f * 2);
					m_connectionLine[2].position = sf::Vector2f(m_neuronX, m_neuronY + 0.5f * 2);
					m_connectionLine[3].position = sf::Vector2f(m_previousNeuronX, m_previousNeuronY + 0.5f * 2);

					m_window.draw(m_connectionLine);
				}
			}
			m_window.draw(m_neuronCircle);
			drawWeightText(m_neuronX, m_neuronY, m_neuronTextStream.str(), sf::Color(m_textColor, m_textColor, m_textColor), m_window);
		}
	}
}

/// <summary>
/// Draws the text for the weight on each perceptron 
/// </summary>
/// <param name="m_x"></param>
/// <param name="m_y"></param>
/// <param name="m_text"></param>
/// <param name="m_color"></param>
/// <param name="m_window"></param>
void NeuralNetworks::drawWeightText(int m_x, int m_y, const std::string& m_text, const sf::Color& m_color, sf::RenderWindow& m_window)
{
	m_neuralWeightText.setString(m_text);
	m_neuralWeightText.setFillColor(m_color);
	m_neuralWeightText.setOrigin(m_neuralWeightText.getGlobalBounds().width / 2, m_neuralWeightText.getGlobalBounds().height / 2);
	m_neuralWeightText.setPosition(m_x, m_y - 5);

	m_window.draw(m_neuralWeightText);
}

/// <summary>
/// this is a method used for training the neural network, where the error between the predicted output and the actual output
/// is computed and propagated back through the network. 
/// This process adjusts the weights and biases to minimize the error, effectively "learning" from the data
/// </summary>
/// <param name="m_target"></param>
/// <param name="m_errors"></param>
/// <param name="m_neuralNetwork"></param>
/// <param name="m_weights"></param>
void NeuralNetworks::backPropagation(const std::vector<float>& m_target, std::vector<std::vector<float>>& m_errors, const std::vector<std::vector<float>>& m_neuralNetwork, std::vector<std::vector<std::vector<float>>>& m_weights)
{
	for (int i = m_neuralNetwork.size() - 1; 0 < i; i--)
	{
		for (int j = 0; j < m_neuralNetwork[i].size(); j++)
		{
			int m_biasNeurons = 0;

			if (i == m_neuralNetwork.size() - 1)
			{
				m_errors[i - 1][j] = pow(m_neuralNetwork[i][j] - m_target[j], 3);
			}
			else if (j >= BIAS_NEURONS[i])
			{
				int m_nextBiasNeuron = 0;

				m_biasNeurons = BIAS_NEURONS[i];

				if (i < m_neuralNetwork.size() - 2)
				{
					m_nextBiasNeuron = BIAS_NEURONS[1 + i];
				}

				m_errors[i - 1][j - m_biasNeurons] = 0;

				for (int k = m_nextBiasNeuron; k < m_neuralNetwork[1 + i].size(); k++)
				{
					m_errors[i - 1][j - m_biasNeurons] += m_errors[i][k - m_nextBiasNeuron] * m_weights[i][k - m_nextBiasNeuron][j - m_biasNeurons];
				}
			}

			if (j >= m_biasNeurons)
			{
				float m_neuronOutput = 0;

				for (int k = 0; k < m_weights[i - 1][j - m_biasNeurons].size(); k++)
				{
					m_neuronOutput += m_neuralNetwork[i - 1][k] * m_weights[i - 1][j - m_biasNeurons][k];
				}

				for (int k = 0; k < m_neuralNetwork[i - 1].size(); k++)
				{
					m_weights[i - 1][j - m_biasNeurons][k] -= LEARNING_RATE * m_errors[i - 1][j - m_biasNeurons] * m_neuralNetwork[i - 1][k] * activationFunctionSigmoid(1, m_neuronOutput);
				}
			}
		}
	}
}

/// <summary>
/// This is a process where input data is passed through the network, layer by layer, until it reaches the output layer.
/// Each neuron in the network processes the input by performing a weighted sum followed by an "activation function", 
/// which that introduces non-linearity
/// </summary>
/// <param name="m_state"></param>
/// <param name="m_inputs"></param>
/// <param name="m_neuralNetwork"></param>
/// <param name="m_weights"></param>
/// <returns></returns>
std::vector<float> NeuralNetworks::forwardPropagation(bool m_state, const std::vector<float>& m_inputs, std::vector<std::vector<float>>& m_neuralNetwork, const std::vector<std::vector<std::vector<float>>>& m_weights)
{
	if (m_state)
	{
		for (int i = 0; i < m_neuralNetwork[0].size(); i++)
		{
			if (i >= BIAS_NEURONS[0])
			{
				m_neuralNetwork[0][i] = m_inputs[i - BIAS_NEURONS[0]];
			}
			else
			{
				// Bias neurons values are always 1.
				m_neuralNetwork[0][i] = 1;
			}
		}

		for (int i = 0; i < m_weights.size(); i++)
		{
			int m_biasNeurons = 0;

			if (i < m_weights.size() - 1)
			{
				m_biasNeurons = BIAS_NEURONS[1 + i];
			}

			// At the beginning we assume that each neuron is a bias neuron.
			std::fill(m_neuralNetwork[1 + i].begin(), m_neuralNetwork[1 + i].end(), 1);

			for (int j = 0; j < m_weights[i].size(); j++)
			{
				m_neuralNetwork[1 + i][j + m_biasNeurons] = 0;

				for (int k = 0; k < m_weights[i][j].size(); k++)
				{
					m_neuralNetwork[1 + i][j + m_biasNeurons] += m_neuralNetwork[i][k] * m_weights[i][j][k];
				}

				m_neuralNetwork[1 + i][j + m_biasNeurons] = activationFunctionSigmoid(0, m_neuralNetwork[1 + i][j + m_biasNeurons]);
			}
		}
		return m_neuralNetwork[m_neuralNetwork.size() - 1];
	}
	else
	{
		vector_2d m_newNeuralNetwork = m_neuralNetwork;

		for (int i = 0; i < m_newNeuralNetwork[0].size(); i++)
		{
			if (i >= BIAS_NEURONS[0])
			{
				m_newNeuralNetwork[0][i] = m_inputs[i - BIAS_NEURONS[0]];
			}
			else
			{
				m_newNeuralNetwork[0][i] = 1;
			}
		}

		for (int i = 0; i < m_weights.size(); i++)
		{
			int bias_neurons = 0;

			if (i < m_weights.size() - 1)
			{
				bias_neurons = BIAS_NEURONS[1 + i];
			}

			std::fill(m_newNeuralNetwork[1 + i].begin(), m_newNeuralNetwork[1 + i].end(), 1);

			for (int j = 0; j < m_weights[i].size(); j++)
			{
				m_newNeuralNetwork[1 + i][j + bias_neurons] = 0;

				for (int k = 0; k < m_weights[i][j].size(); k++)
				{
					m_newNeuralNetwork[1 + i][j + bias_neurons] += m_newNeuralNetwork[i][k] * m_weights[i][j][k];
				}

				m_newNeuralNetwork[1 + i][j + bias_neurons] = activationFunctionSigmoid(0, m_newNeuralNetwork[1 + i][j + bias_neurons]);
			}
		}
		return m_newNeuralNetwork[m_newNeuralNetwork.size() - 1];
	}
}

/// <summary>
/// adds the input
/// </summary>
/// <param name="m_dot_x"></param>
/// <param name="m_dot_y"></param>
void NeuralNetworks::addInput(float m_dot_x, float m_dot_y)
{
	m_inputs.push_back({ m_dot_x, m_dot_y });
}

/// <summary>
/// adds the targetOutput thats the circle things for visualization
/// </summary>
/// <param name="m_r"></param>
/// <param name="m_g"></param>
/// <param name="m_b"></param>
void NeuralNetworks::addTargetOutput(float m_r, float m_g, float m_b)
{
	m_target_outputs.push_back({ m_r, m_g, m_b });
}

/// <summary>
/// To go back to main menu screen
/// </summary>
/// <param name="m_mousePosition"></param>
/// <param name="m_gameState"></param>
void NeuralNetworks::goToMainMenu(sf::Vector2i m_mousePosition, GameState& m_gameState)
{
	if (m_toGoBackButton.getGlobalBounds().contains(m_mousePosition.x, m_mousePosition.y))
	{
		m_gameState = GameState::MainMenu;
	}
}

std::array<int, 2> NeuralNetworks::getHiddenNeurons() const
{
	return HIDDEN_NEURONS;
}

std::array<int, 3> NeuralNetworks::getBiasNeurons() const
{
	return BIAS_NEURONS;
}

const std::vector<std::vector<float>>& NeuralNetworks::getInputs() const
{
	return m_inputs;
}

const std::vector<std::vector<float>>& NeuralNetworks::getTargetOutputs() const
{
	return m_target_outputs;
}

/// <summary>
/// Initializes neural circles
/// </summary>
void NeuralNetworks::initNeuralCircle()
{
	m_neuronCircle.setRadius(32);
	m_neuronCircle.setOrigin(32, 32);
	m_neuronCircle.setOutlineColor(sf::Color(255, 255, 255));
	m_neuronCircle.setOutlineThickness(2);

	m_neuralWeightText.setFont(m_font);
	m_neuralWeightText.setCharacterSize(24);
	m_neuralWeightText.setOrigin(m_neuralWeightText.getGlobalBounds().width / 2, m_neuralWeightText.getGlobalBounds().height / 2);
}

/// <summary>
/// Initializes back button
/// </summary>
void NeuralNetworks::initBackButton()
{
	if (!m_buttonTexture.loadFromFile("Assets\\Images\\GUI\\buttonStock2.png"))
	{
		std::cout << "Error - loading button texture" << std::endl;
	}

	m_toGoBackButton.setSize(sf::Vector2f(100, 50));
	m_toGoBackButton.setPosition(80, 50);
	m_toGoBackButton.setOrigin(m_toGoBackButton.getGlobalBounds().width / 2, m_toGoBackButton.getGlobalBounds().height / 2);
	m_toGoBackButton.setTexture(&m_buttonTexture);

	m_toGoBackText.setFont(m_font);
	m_toGoBackText.setString("Back");
	m_toGoBackText.setPosition(m_toGoBackButton.getPosition().x, m_toGoBackButton.getPosition().y - 5);
	m_toGoBackText.setCharacterSize(25U);
	m_toGoBackText.setFillColor(sf::Color(225, 245, 255));
	m_toGoBackText.setOutlineColor(sf::Color::Blue);
	m_toGoBackText.setOutlineThickness(1.0f);
	m_toGoBackText.setOrigin(m_toGoBackText.getGlobalBounds().width / 2, m_toGoBackText.getGlobalBounds().height / 2);
}

/// <summary>
/// Initializes the mouse Dot circle
/// </summary>
void NeuralNetworks::initMouseDotCircle()
{
	m_mouseDotCircle.setRadius(10);
	m_mouseDotCircle.setOrigin(m_mouseDotCircle.getRadius(), m_mouseDotCircle.getRadius());
	m_mouseDotCircle.setFillColor(sf::Color::White);
	m_mouseDotCircle.setOutlineColor(sf::Color::Black);
	m_mouseDotCircle.setOutlineThickness(2.0f);
}

/// <summary>
/// Using the sigmoid activation function
/// 
/// Sigmoid function: f(x) = 1 / (1 + exp(-x))
/// Derivative of sigmoid: f'(x) = f(x) * (1 - f(x))
/// 
/// </summary>
/// <param name="m_derivative"></param>
/// <param name="m_input"></param>
/// <returns></returns>
float NeuralNetworks::activationFunctionSigmoid(bool m_derivative, float m_input)
{
	if (!m_derivative)
	{
		return 1.0f / (1.0f + std::exp(-m_input));
	}
	else
	{
		float sigmoid = 1.0f / (1.0f + std::exp(-m_input));
		return sigmoid * (1 - sigmoid);
	}
}

float NeuralNetworks::get_min_element(const std::vector<float>& m_vector)
{
	return *std::min_element(m_vector.begin(), m_vector.end());
}

float NeuralNetworks::get_min_element(const std::vector<std::vector<float>>& m_vector)
{
	float minElement = std::numeric_limits<float>::max();
	for (const auto& subVector : m_vector)
	{
		float tempMin = get_min_element(subVector);
		if (tempMin < minElement)
		{
			minElement = tempMin;
		}
	}
	return minElement;
}

float NeuralNetworks::get_min_element(const std::vector<std::vector<std::vector<float>>>& m_vector)
{
	float minElement = std::numeric_limits<float>::max();
	for (const auto& subVector : m_vector)
	{
		float tempMin = get_min_element(subVector);
		if (tempMin < minElement)
		{
			minElement = tempMin;
		}
	}
	return minElement;
}

float NeuralNetworks::get_max_element(const std::vector<float>& m_vector)
{
	return *std::max_element(m_vector.begin(), m_vector.end());
}

float NeuralNetworks::get_max_element(const std::vector<std::vector<float>>& m_vector)
{
	float maxElement = std::numeric_limits<float>::lowest();
	for (const auto& subVector : m_vector)
	{
		float tempMax = get_max_element(subVector);
		if (tempMax > maxElement)
		{
			maxElement = tempMax;
		}
	}
	return maxElement;
}

float NeuralNetworks::get_max_element(const std::vector<std::vector<std::vector<float>>>& m_vector)
{
	float maxElement = std::numeric_limits<float>::lowest();
	for (const auto& subVector : m_vector)
	{
		float tempMax = get_max_element(subVector);
		if (tempMax > maxElement)
		{
			maxElement = tempMax;
		}
	}
	return maxElement;
}