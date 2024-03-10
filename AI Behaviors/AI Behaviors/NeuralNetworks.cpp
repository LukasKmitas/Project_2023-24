#include "NeuralNetworks.h"

NeuralNetworks::NeuralNetworks()
	: HIDDEN_NEURONS{ 3, 3 }, BIAS_NEURONS{ 1, 0, 0 }
{
	setupNeuralCircle();
}

NeuralNetworks::~NeuralNetworks()
{
}

void NeuralNetworks::draw_text(bool m_horizontal_center, bool m_vertical_center, int m_x, int m_y, const std::string& m_text, const sf::Color& m_color, sf::RenderWindow& m_window, const sf::Texture& m_font_texture)
{
	int character_x = m_x;
	int character_y = m_y;

	int character_height = m_font_texture.getSize().y;
	int character_width = m_font_texture.getSize().x / 96;

	sf::Sprite character_sprite(m_font_texture);
	character_sprite.setColor(m_color);

	if (m_horizontal_center)
	{
		character_x -= static_cast<short>(round(0.5f * character_width * m_text.substr(0, m_text.find_first_of('\n')).size()));
	}
	if (m_vertical_center)
	{
		character_y -= static_cast<short>(round(0.5f * character_height * (1 + std::count(m_text.begin(), m_text.end(), '\n'))));
	}

	for (auto a = m_text.begin(); a != m_text.end(); ++a)
	{
		if (*a == '\n')
		{
			character_x = m_x - (m_horizontal_center ? static_cast<short>(round(0.5f * character_width * m_text.substr(0, m_text.find_first_of('\n')).size())) : 0);
			character_y += character_height;
			continue;
		}

		character_sprite.setPosition(character_x, character_y);
		character_sprite.setTextureRect(sf::IntRect(character_width * (*a - 32), 0, character_width, character_height));

		character_x += character_width;

		m_window.draw(character_sprite);
	}
}

void NeuralNetworks::draw_neural_network(sf::RenderWindow& m_window, const sf::Texture& m_fontTexture, const vector_2d& m_neural_network, const vector_3d& m_weights)
{
	float max_neuron = std::max(abs(get_max_element(m_neural_network)), abs(get_min_element(m_neural_network)));
	float min_neuron = -max_neuron;

	sf::VertexArray connection_shape(sf::Quads, 4);

	if (0 <= get_min_element(m_neural_network))
	{
		min_neuron = 0;
	}

	for (int i = 0; i < m_neural_network.size(); i++)
	{
		int neuron_x = (Global::S_WIDTH / 2) * (1 + i) / (1 + m_neural_network.size());

		for (int k = 0; k < m_neural_network[i].size(); k++)
		{
			// Rounding the value of the neuron
			float neuron_value = round(m_neural_network[i][k] * pow(10, 2)) / pow(10, 2);

			unsigned char neuron_color = round(255 * (m_neural_network[i][k] - min_neuron) / (max_neuron - min_neuron));
			unsigned char text_color = 255 * (128 > neuron_color);

			int neuron_y = Global::S_HEIGHT * (1 + k) / (1 + m_neural_network[i].size());

			std::ostringstream neuron_text_stream;
			neuron_text_stream << std::fixed << std::setprecision(2) << neuron_value; // to make it be like "0.02" and not "0.0234324"

			neuron_shape.setFillColor(sf::Color(neuron_color, neuron_color, neuron_color));
			neuron_shape.setPosition(neuron_x, neuron_y);

			if (i < m_neural_network.size() - 1)
			{
				float max_weight = std::max(abs(get_max_element(m_weights[i])), abs(get_min_element(m_weights[i])));
				float min_weight = -max_weight;

				int bias_neurons = 0;

				if (i < m_neural_network.size() - 2)
				{
					bias_neurons = BIAS_NEURONS[1 + i];
				}

				for (int j = bias_neurons; j < m_neural_network[1 + i].size(); j++)
				{
					if (0 <= m_weights[i][j - bias_neurons][k])
					{
						int connection_opacity = round(255 * m_weights[i][j - bias_neurons][k] / max_weight);

						connection_shape[0].color = sf::Color(0, 255, 0, connection_opacity);
						connection_shape[1].color = sf::Color(0, 255, 0, connection_opacity);
						connection_shape[2].color = sf::Color(0, 255, 0, connection_opacity);
						connection_shape[3].color = sf::Color(0, 255, 0, connection_opacity);
					}
					else
					{
						int connection_opacity = round(255 * m_weights[i][j - bias_neurons][k] / min_weight);

						connection_shape[0].color = sf::Color(255, 0, 0, connection_opacity);
						connection_shape[1].color = sf::Color(255, 0, 0, connection_opacity);
						connection_shape[2].color = sf::Color(255, 0, 0, connection_opacity);
						connection_shape[3].color = sf::Color(255, 0, 0, connection_opacity);
					}

					int previous_neuron_x = (Global::S_WIDTH / 2) * (2 + i) / (1 + m_neural_network.size());
					int previous_neuron_y = Global::S_HEIGHT * (1 + j) / (1 + m_neural_network[1 + i].size());

					connection_shape[0].position = sf::Vector2f(previous_neuron_x, previous_neuron_y - 0.5f * 2);
					connection_shape[1].position = sf::Vector2f(neuron_x, neuron_y - 0.5f * 2);
					connection_shape[2].position = sf::Vector2f(neuron_x, neuron_y + 0.5f * 2);
					connection_shape[3].position = sf::Vector2f(previous_neuron_x, previous_neuron_y + 0.5f * 2);

					m_window.draw(connection_shape);
				}
			}
			m_window.draw(neuron_shape);

			draw_text(1, 1, neuron_x, neuron_y, neuron_text_stream.str(), sf::Color(text_color, text_color, text_color), m_window, m_fontTexture);
		}
	}
}

void NeuralNetworks::backPropagation(const std::vector<float>& m_target, std::vector<std::vector<float>>& m_errors, const std::vector<std::vector<float>>& m_neural_network, std::vector<std::vector<std::vector<float>>>& m_weights)
{
	for (int i = m_neural_network.size() - 1; 0 < i; i--)
	{
		for (int j = 0; j < m_neural_network[i].size(); j++)
		{
			int bias_neurons = 0;

			if (i == m_neural_network.size() - 1)
			{
				m_errors[i - 1][j] = pow(m_neural_network[i][j] - m_target[j], 3);
			}
			else if (j >= BIAS_NEURONS[i])
			{
				int next_bias_neurons = 0;

				bias_neurons = BIAS_NEURONS[i];

				if (i < m_neural_network.size() - 2)
				{
					next_bias_neurons = BIAS_NEURONS[1 + i];
				}

				m_errors[i - 1][j - bias_neurons] = 0;

				for (int k = next_bias_neurons; k < m_neural_network[1 + i].size(); k++)
				{
					m_errors[i - 1][j - bias_neurons] += m_errors[i][k - next_bias_neurons] * m_weights[i][k - next_bias_neurons][j - bias_neurons];
				}
			}

			if (j >= bias_neurons)
			{
				float neuron_output = 0;

				for (int k = 0; k < m_weights[i - 1][j - bias_neurons].size(); k++)
				{
					neuron_output += m_neural_network[i - 1][k] * m_weights[i - 1][j - bias_neurons][k];
				}

				for (int k = 0; k < m_neural_network[i - 1].size(); k++)
				{
					m_weights[i - 1][j - bias_neurons][k] -= LEARNING_RATE * m_errors[i - 1][j - bias_neurons] * m_neural_network[i - 1][k] * activationFunction(1, neuron_output);
				}
			}
		}
	}
}

std::vector<float> NeuralNetworks::forwardPropagation(bool m_state, const std::vector<float>& m_inputs, std::vector<std::vector<float>>& m_neural_network, const std::vector<std::vector<std::vector<float>>>& m_weights)
{
	if (m_state)
	{
		for (int i = 0; i < m_neural_network[0].size(); i++)
		{
			if (i >= BIAS_NEURONS[0])
			{
				m_neural_network[0][i] = m_inputs[i - BIAS_NEURONS[0]];
			}
			else
			{
				// Bias neurons values are always 1.
				m_neural_network[0][i] = 1;
			}
		}

		for (int i = 0; i < m_weights.size(); i++)
		{
			int bias_neurons = 0;

			if (i < m_weights.size() - 1)
			{
				bias_neurons = BIAS_NEURONS[1 + i];
			}

			// At the beginning we assume that each neuron is a bias neuron.
			std::fill(m_neural_network[1 + i].begin(), m_neural_network[1 + i].end(), 1);

			for (int j = 0; j < m_weights[i].size(); j++)
			{
				m_neural_network[1 + i][j + bias_neurons] = 0;

				for (int k = 0; k < m_weights[i][j].size(); k++)
				{
					m_neural_network[1 + i][j + bias_neurons] += m_neural_network[i][k] * m_weights[i][j][k];
				}

				m_neural_network[1 + i][j + bias_neurons] = activationFunction(0, m_neural_network[1 + i][j + bias_neurons]);
			}
		}
		return m_neural_network[m_neural_network.size() - 1];
	}
	else
	{
		vector_2d neural_network = m_neural_network;

		for (int i = 0; i < neural_network[0].size(); i++)
		{
			if (i >= BIAS_NEURONS[0])
			{
				neural_network[0][i] = m_inputs[i - BIAS_NEURONS[0]];
			}
			else
			{
				neural_network[0][i] = 1;
			}
		}

		for (int i = 0; i < m_weights.size(); i++)
		{
			int bias_neurons = 0;

			if (i < m_weights.size() - 1)
			{
				bias_neurons = BIAS_NEURONS[1 + i];
			}

			std::fill(neural_network[1 + i].begin(), neural_network[1 + i].end(), 1);

			for (int j = 0; j < m_weights[i].size(); j++)
			{
				neural_network[1 + i][j + bias_neurons] = 0;

				for (int k = 0; k < m_weights[i][j].size(); k++)
				{
					neural_network[1 + i][j + bias_neurons] += neural_network[i][k] * m_weights[i][j][k];
				}

				neural_network[1 + i][j + bias_neurons] = activationFunction(0, neural_network[1 + i][j + bias_neurons]);
			}
		}
		return neural_network[neural_network.size() - 1];
	}
}

void NeuralNetworks::setupNeuralCircle()
{
	neuron_shape.setRadius(32);
	neuron_shape.setOrigin(32, 32);
	neuron_shape.setOutlineColor(sf::Color(255, 255, 255));
	neuron_shape.setOutlineThickness(2);
}

float NeuralNetworks::activationFunction(bool m_derivative, float m_input)
{
	if (m_derivative == 0)
	{
		if (m_input <= 0)
		{
			return std::pow(2, m_input - 1);
		}
		else
		{
			return 1 - std::pow(2, -1 - m_input);
		}
	}
	else
	{
		if (m_input <= 0)
		{
			return std::log(2) * std::pow(2, m_input - 1);
		}
		else
		{
			return std::log(2) * std::pow(2, -1 - m_input);
		}
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