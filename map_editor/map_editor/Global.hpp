//Author: Sivert Andresen Cubedo
#pragma once

#ifndef Global_HEADER
#define Global_HEADER

#include <iostream>
#include <string>
#include <vector>

#include <SFML\Graphics.hpp>

namespace Global {
	const int numberOfTypes = 9;
	const enum GroundTypeNumber : int {
		Void = 0,
		Water = 1,
		GrassPlain = 2,
		Forrest = 3,
		Desert = 4,
		Mountain = 5,
		Artic = 6,
		Tundra = 7,
		Jungle = 8
	};

	namespace TexturePaths
	{
		const std::string void_texture = "textures/void.png";
		const std::string water_texture = "textures/water.png";
		const std::string grass_plain_texture = "textures/grass_plain.png";
		const std::string forrest_texture = "textures/forrest.png";
		const std::string desert_texture = "textures/desert.png";
		const std::string mountain_texture = "textures/mountain.png";
		const std::string artic_texture = "textures/artic.png";
		const std::string tundra_texture = "textures/tundra.png";
		const std::string jungle_texture = "textures/jungle.png";
		
		const std::string unit_texture_map = "textures/unit_texture_map.png";
	}
	const std::vector<std::string> mapTexturesPaths{
		TexturePaths::void_texture,
		TexturePaths::water_texture,
		TexturePaths::grass_plain_texture,
		TexturePaths::forrest_texture,
		TexturePaths::desert_texture,
		TexturePaths::mountain_texture,
		TexturePaths::artic_texture,
		TexturePaths::tundra_texture,
		TexturePaths::jungle_texture
	};

	class TextureContainer {
	private:

		bool isInit = false;

	public:

		std::vector<sf::Texture> groundTextureVector;
		sf::Texture unitTexture;

		bool init() {
			groundTextureVector = std::vector<sf::Texture>(numberOfTypes);
			for (int i = 0; i < numberOfTypes; i++) {
				if (!groundTextureVector[i].loadFromFile(mapTexturesPaths[i])) {
					std::cout << "Missing: " << mapTexturesPaths[i] << "\n";
					return false;
				}
				groundTextureVector[i].setRepeated(true);
				groundTextureVector[i].setSmooth(true);
			}

			if (!unitTexture.loadFromFile(TexturePaths::unit_texture_map)) {
				std::cout << "Missing: " << TexturePaths::unit_texture_map << "\n";
				return false;
			}
			isInit = true;
			return true;
		}


	};

	/*
	namespace Texture {
		std::vector<sf::Texture> initGroundTexture() {
			std::vector<sf::Texture> vec(numberOfTypes);
			for (int i = 0; i < numberOfTypes; i++) {
				if (!vec[i].loadFromFile(mapTexturesPaths[i])) {
					std::cout << "Missing: " << mapTexturesPaths[i] << "\n";
					return std::vector<sf::Texture>();
				}
				vec[i].setRepeated(true);
				vec[i].setSmooth(true);
			}
			return vec;
		}
		const sf::Texture * getGroundTexture(int i) {
			static std::vector<sf::Texture> groundTextureVector = initGroundTexture();
			return &groundTextureVector[i];
		}
		sf::Texture initUnitTexture() {
			sf::Texture tex;
			if (!tex.loadFromFile(TexturePaths::unit_texture_map)) {
				std::cout << "Missing: " << TexturePaths::unit_texture_map << "\n";
				return sf::Texture();
			}
			return tex;
		}
		const sf::Texture * getUnitTexture() {
			static sf::Texture unitTexture = initUnitTexture();
			return &unitTexture;
		}
	}

	static bool init() {
		
		
		return true;
	}
	*/

};

#endif // !Global_HEADER

