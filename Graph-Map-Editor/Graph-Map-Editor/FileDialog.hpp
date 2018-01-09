//Author: Sivert Andresen Cubedo

#pragma once

#ifndef FileDialog_HEADER
#define FileDialog_HEADER

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <nfd.h>

namespace FileDialog {

	enum Result {
		Error,		//read failed
		Success,	//path was retrived from dialog
		Closed		//dialog was closed by user
	};

	class Dialog {
	protected:
		Result result = Error;
		std::string path;
		std::string convertString(nfdchar_t* in);
	public:
		virtual void create() = 0;												//BLOCK
		Result getStatus();
		std::string getPath();
	};

	class OpenFile : public Dialog {
	public:
		void create() override;
		void create(std::string filter);
		void create(std::string filter, std::string default_path);
	};

	class OpenFileMulti : public Dialog {
		std::vector<std::string> path_vec;
	public:
		void create() override;
		void create(std::string filter);
		void create(std::string filter, std::string default_path);
		std::vector<std::string> getPathVec();
	};

	class OpenDir : public Dialog {
	public:
		void create();
		void create(std::string default_path);
	};

	class SaveFile : public Dialog {
	public:
		void create() override;
		void create(std::string filter);
		void create(std::string filter, std::string default_path);
	};

}

#endif // !FileDialog_HEADER


