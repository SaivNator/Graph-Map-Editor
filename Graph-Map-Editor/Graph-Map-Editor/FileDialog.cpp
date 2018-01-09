//Author: Sivert Andresen Cubedo

#include "FileDialog.hpp"

using namespace FileDialog;

Result Dialog::getStatus() {
	return result;
}

std::string Dialog::getPath() {
	return path;
}

std::string Dialog::convertString(nfdchar_t* in) {
	std::string str;
	std::size_t i = 0;
	while (in[i] != '\0') {
		str += in[i];
		++i;
	}
	return str;
}

void OpenFile::create() {
	nfdchar_t* c_path = NULL;
	nfdresult_t c_result = NFD_OpenDialog(NULL, NULL, &c_path);
	if (c_result == NFD_OKAY) {
		path = convertString(c_path);
		result = Success;
		free(c_path);
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}
void OpenFile::create(std::string filter) {
	nfdchar_t* c_path = NULL;
	nfdresult_t c_result = NFD_OpenDialog(filter.c_str(), NULL, &c_path);
	if (c_result == NFD_OKAY) {
		path = convertString(c_path);
		result = Success;
		free(c_path);
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}
void OpenFile::create(std::string filter, std::string default_path) {
	nfdchar_t* c_path = NULL;
	nfdresult_t c_result = NFD_OpenDialog(filter.c_str(), default_path.c_str(), &c_path);
	if (c_result == NFD_OKAY) {
		path = convertString(c_path);
		result = Success;
		free(c_path);
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}

void OpenFileMulti::create() {
	nfdpathset_t c_pathSet;
	nfdresult_t c_result = NFD_OpenDialogMultiple(NULL, NULL, &c_pathSet);
	if (c_result == NFD_OKAY) {
		for (std::size_t i = 0; i < NFD_PathSet_GetCount(&c_pathSet); ++i) {
			nfdchar_t* c_path = NFD_PathSet_GetPath(&c_pathSet, i);
			std::string temp = convertString(c_path);
			path_vec.push_back(temp);
			if (i > 0) path += "\n";
			path += temp;
		}
		NFD_PathSet_Free(&c_pathSet);
		result = Success;
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}	
}
void OpenFileMulti::create(std::string filter) {
	nfdpathset_t c_pathSet;
	nfdresult_t c_result = NFD_OpenDialogMultiple(filter.c_str(), NULL, &c_pathSet);
	if (c_result == NFD_OKAY) {
		for (std::size_t i = 0; i < NFD_PathSet_GetCount(&c_pathSet); ++i) {
			nfdchar_t* c_path = NFD_PathSet_GetPath(&c_pathSet, i);
			std::string temp = convertString(c_path);
			path_vec.push_back(temp);
			if (i > 0) path += "\n";
			path += temp;
		}
		NFD_PathSet_Free(&c_pathSet);
		result = Success;
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}

void OpenFileMulti::create(std::string filter, std::string default_path) {
	nfdpathset_t c_pathSet;
	nfdresult_t c_result = NFD_OpenDialogMultiple(filter.c_str(), default_path.c_str(), &c_pathSet);
	if (c_result == NFD_OKAY) {
		for (std::size_t i = 0; i < NFD_PathSet_GetCount(&c_pathSet); ++i) {
			nfdchar_t* c_path = NFD_PathSet_GetPath(&c_pathSet, i);
			std::string temp = convertString(c_path);
			path_vec.push_back(temp);
			if (i > 0) path += "\n";
			path += temp;
		}
		NFD_PathSet_Free(&c_pathSet);
		result = Success;
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}

std::vector<std::string> OpenFileMulti::getPathVec() {
	return path_vec;
}

void OpenDir::create() {
	nfdchar_t* c_path = NULL;
	nfdresult_t c_result = NFD_PickFolder(NULL, &c_path);
	if (c_result == NFD_OKAY) {
		path = convertString(c_path);
		result = Success;
		free(c_path);
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}
void OpenDir::create(std::string default_path) {
	nfdchar_t* c_path = NULL;
	nfdresult_t c_result = NFD_PickFolder(default_path.c_str(), &c_path);
	if (c_result == NFD_OKAY) {
		path = convertString(c_path);
		result = Success;
		free(c_path);
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}

void SaveFile::create() {
	nfdchar_t* c_path = NULL;
	nfdresult_t c_result = NFD_SaveDialog(NULL, NULL, &c_path);
	if (c_result == NFD_OKAY) {
		path = convertString(c_path);
		result = Success;
		free(c_path);
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}

void SaveFile::create(std::string filter) {
	nfdchar_t* c_path = NULL;
	nfdresult_t c_result = NFD_SaveDialog(filter.c_str(), NULL, &c_path);
	if (c_result == NFD_OKAY) {
		path = convertString(c_path);
		result = Success;
		free(c_path);
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}

void SaveFile::create(std::string filter, std::string default_path) {
	nfdchar_t* c_path = NULL;
	nfdresult_t c_result = NFD_SaveDialog(filter.c_str(), default_path.c_str(), &c_path);
	if (c_result == NFD_OKAY) {
		path = convertString(c_path);
		result = Success;
		free(c_path);
	}
	else if (c_result == NFD_CANCEL) {
		result = Closed;
	}
	else if (c_result == NFD_ERROR) {
		result = Error;
	}
}

//end
