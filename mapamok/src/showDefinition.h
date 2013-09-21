#pragma once

class showResource
{
public:
	string filePath;
	string name;
};

class showDefinition
{
public:
	~showDefinition();

	void showDefinition::load(string configName);

	void addMovie(string filePath, string name);
	void addPicture(string filePath, string name);

	std::vector<showResource*> movies;
	std::vector<showResource*> pictures;
};
