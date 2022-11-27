#include "Evolution.h"
#include "GeneticAlgorithm.h"
#include <random>
#include <chrono>
#include <iostream>

namespace genetic
{
Evolution::Evolution(base::seed_t seed)
    : _window(sf::VideoMode(game::window_size, game::window_size), "SFML works!")
    , _feeds(_generate_feed(game::window_size))
    , _seed(seed) {

    genetic::GeneticAlgorithm::seed = seed;

    _agents.resize(size_of_population, Agent(game::Snake::number_of_input_neuron, {(base::count_t)6, (base::count_t)4}, (base::count_t)3, seed));
    for (size_t i = 0; i < size_of_population; ++i) {
        _agents[i] = Agent(game::Snake::number_of_input_neuron, {(base::count_t)6, (base::count_t)4}, (base::count_t)3, seed);
        _games.push_back(game::Game(game::Snake(_agents[i]), _feeds));
    }
    _last_score = 0;
}

Evolution::Evolution(std::string path1, std::string path2, base::seed_t seed)
    : _window(sf::VideoMode(game::window_size, game::window_size), "SFML works!")
    , _feeds(_generate_feed(game::window_size))
    , _seed(seed) {

    genetic::GeneticAlgorithm::seed = seed;

    _agents.push_back(Agent(path1));
    _agents.push_back(Agent(path2));
    _agents = GeneticAlgorithm::recombinate(_agents, size_of_population);

    for (size_t i = 0; i < size_of_population; ++i) {
        _games.push_back(game::Game(game::Snake(_agents[i]), _feeds));
    }
    _last_score = 0;
}

void Evolution::evolution() {
    _number_of_generation = 0;
    int dec = 1;

    while (do_evolution) {
        _simulate_game();
        GeneticAlgorithm::update(_agents);
        _number_of_generation++;

        if (_number_of_generation > 100 * dec) {
            _agents[0].save("nn1-1.bin");
            _agents[0].save("nn2-1.bin");
            dec++;
        }

        std::cout << _agents[0].get_score() << " " << _agents[1].get_score() << " in "
            << _number_of_generation << " generation\n";
        _agents = GeneticAlgorithm::recombinate(GeneticAlgorithm::select(_agents), size_of_population);
        _restart();
    }
    save();
}

void Evolution::save() const {
    _agents[0].save("nn9.bin");
}

void Evolution::load(std::string path) {
    Agent load = Agent(path);

    game::Game loaded = game::Game(game::Snake(load, sf::Color::Red), _generate_feed(game::window_size));
    _window.setFramerateLimit(30);
    while (!loaded.end_game()) {
        _window.clear();
        loaded.move();
        _check_save();
        loaded.draw(_window, true);
        _window.display();
    }
    while (true) {
        _check_save();
    }
}

Evolution::~Evolution() {
    _window.close();
}

void Evolution::_simulate_game() {
    _window.setFramerateLimit(30);
    _games[0].set_color(sf::Color::Red);

    size_t max_id = 0;
    bool continu = true;

    while (continu && _window.isOpen()) {

        continu = false;
        for (auto& game : _games) {
            game.move();

            continu = game.end_game() ? continu : 1;
        }

        if (!_check_save()) break;

        for (size_t i = 0; i < _games.size(); ++i) {
            if (_games[i].get_score() > _games[max_id].get_score()
                || _games[max_id].end_game()) {
                _games[max_id].set_color();
                max_id = i;
                _games[i].set_color(sf::Color::Red);
            }
        }

        if (!draw) continue;

        _window.clear();
        for (size_t i = 0; i < _games.size(); ++i) {
            if (i != max_id) {
                _games[i].draw(_window);
            }
        }
        _games[max_id].draw(_window, true);

        _window.display();
    }
}

void Evolution::_restart() {

    _games.clear();

    if (_agents.front().get_score() > _last_score || !wait) {
        _feeds = _generate_feed(game::window_size);
        _last_score = _agents.front().get_score();
    }

    for (auto& agent : _agents) {
        agent.set_score(0);
    }

    for (size_t i = 0; i < size_of_population; ++i) {
        _games.push_back(game::Game(game::Snake(_agents[i]), _feeds));
    }
}

bool Evolution::_check_save() {
    sf::Event event;

    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            do_evolution = false;
            return false;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::S) {
                do_evolution = false;
                return false;
            }
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::D) {
                draw = !draw;
                return true;
            }
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::W) {
                wait = !wait;
                return true;
            }
        }
    }

    return true;
}

std::vector<sf::Vector2f> Evolution::_generate_feed(long size) const {
    std::vector<sf::Vector2f> feeds;

    base::genererator_t gen(_seed);
    std::uniform_int_distribution<long> dist(0, long((float)size / game::size - 1.0f));
    sf::Vector2f new_coords;

    for (size_t i = 0; i < game::number_feed; ++i) {
        bool contin = true;
        do {
            contin = false;
            new_coords = {dist(gen) * game::size + game::size / 2.0f
                , dist(gen) * game::size + game::size / 2.0f};

            for (const auto& feed : feeds) {
                if (new_coords == feed) {
                    contin = true;
                    break;
                }
            }

            if (new_coords == sf::Vector2f(95, 95)) {
                contin = true;
            }
        }
        while (contin);

        feeds.push_back(new_coords);
    }

    return feeds;
}

bool Evolution::do_evolution = true;

bool Evolution::draw = false;

bool Evolution::wait = false;
}
