//Author: Sivert Andresen Cubedo

#include <iostream>

#include <wykobi.hpp>

#include "FileDialog.hpp"

int main() {

	wykobi::point2d<float> p0 = wykobi::make_point(100.f, 100.f);

	std::cout << p0.x << "\t" << p0.y << "\n";

	FileDialog::OpenFile dialog;

	dialog.create();

	std::cout << dialog.getPath() << "\n";


	return EXIT_SUCCESS;
}



//end